#include "UbidotsEsp32Mqtt.h"
#include <Wire.h>
//#include <analogWrite.h>        //needed for buzzer alarm
#include <MPU6050_light.h>
#include "TinyGPS++.h"

const char* ssid = "Vegard sin iPhone";
const char* password = "88888888";
const char* token = "BBFF-SYkwlxbJDZ3j58oMZpnNZYi8EiuRYO";        //token inne i Device
const char* mqtt_client_name = "Blackbox1";                       // hva heter enheten




/*
-------------------------------------- -
Setting API labels
-------------------------------------- -
*/

const char* max_g_logged = "crash_g";
const char* crash_alarm = "crash_alarm";
const char* temp_alarm = "temp_alarm";
char *pos = "gps";
const char* device_name = "blackbox_1";
const char* gps_topic = "gps";


/*
  --------------------------------
  Declearing variables
  --------------------------------
*/
String _lat;
String _lng;
const int LED = 18;
unsigned long time_now;
unsigned long LED_time; 
bool last_status_istilted = 0;
bool led_is_on;
int tolerance_angle_fallen = 45;
float max_g = 0;
float current_acc_y;

#define RXD2 16
#define TXD2 17



Ubidots ubidots(token);

MPU6050 mpu(Wire);


TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data





void callback(char* topic, byte * message, unsigned int length) {     //set callback function
    String espIN;
    
    for (int i = 0; i < length; i++) {                 //This function collects all ints being sent to the ESP.
        espIN += (char)message[i];                       //Callback copied from RandomNerdsTutorials, references in report
    }
        Serial.println(espIN);
    if (espIN == "1.0") {
        Serial.println("alarm_på");
    }
    if (espIN == "0.0") {
        Serial.println("alarm_av");
    }
}


/*
  -----------------------------------------------------------
  This function sends the location of a crash to the ubidots
  dashboard. 
  -----------------------------------------------------------
 */
void crash_location() {
    while (Serial2.available()){ //While there are characters to come from the GPS
        gps.encode(Serial2.read());//This feeds the serial NMEA data into the library one char at a time
    }
    if (gps.location.isUpdated()) { //This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
        char context[150];
    
        _lat = String(gps.location.lat(), 4);
        _lng = String(gps.location.lng(), 4);
    
        String tempString1 = ('"' + _lng + '"');
        String tempString2 = ('"' + _lat + '"');
    
        sprintf(context, "\"lat\":%s, \"lng\":%s", tempString2, tempString1);
    
        char* context_char = context; 
        Serial.println(context);
        
        ubidots.add(gps_topic, 1.0, context_char);
        ubidots.publish(device_name);
    }
}

void blink_LED() {
    if (LED_time + 1000 < millis()){
        led_is_on = !led_is_on;
        digitalWrite(LED, led_is_on); 
        LED_time = millis(); 
    }
}



void setup() {                                 
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);//This opens up communications to the GPS
    ubidots.connectToWifi(ssid, password);
    ubidots.setCallback(callback);
    ubidots.setup();
    ubidots.reconnect();
    ubidots.subscribeLastValue(device_name, temp_alarm);
    
    Wire.begin();
    byte status = mpu.begin();
    mpu.calcOffsets(true, true); 
    
    
    ubidots.add(crash_alarm, 0);                //sets initial value for alarm and max_g
    ubidots.add(max_g_logged, 0);
    ubidots.publish(device_name);
  
    pinMode(LED, OUTPUT); 
  
    
    
    time_now = millis();
    LED_time = millis(); 
}

void loop() {
    if (!ubidots.connected()) {                                   // if broken, fix (and resub)
        ubidots.reconnect();
        ubidots.subscribeLastValue(device_name, temp_alarm);        
    }
    
    mpu.update();                         
    
    if(last_status_istilted){
        blink_LED();
    }
      

    current_acc_y = mpu.getAccY();                                //checking for maximum g-forces detected.                       
    if (abs(current_acc_y) > max_g) {                             
        max_g = abs(current_acc_y);
    }
    
    
    /*
    * This if statement checks if a crash has occured. In the event of a crash 
    * the crash alarm will be set of and published, max_g and location of the crash 
    * is published. This information is the essence of the blackbox.  
    */
    float angle = mpu.getAngleZ();
    Serial.println(angle);
    if (abs(angle) > tolerance_angle_fallen && !last_status_istilted) {         
        last_status_istilted = 1;
        ubidots.add(crash_alarm, last_status_istilted); 
        ubidots.add(max_g_logged, max_g);
        ubidots.publish(device_name);
        max_g = 0; 
        crash_location();
    }
    // if the alarm is turned off, then crash alarms are turned off. 
    if (abs(angle) < tolerance_angle_fallen && last_status_istilted) {
        last_status_istilted = 0;
        ubidots.add(crash_alarm, last_status_istilted);
        ubidots.publish(device_name);
        Serial.println(last_status_istilted);
        led_is_on = false;
        digitalWrite(LED, led_is_on);
    }
    ubidots.loop();
}
