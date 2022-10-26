#include "UbidotsEsp32Mqtt.h"
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


/*
  --------------------------------
  Declearing variables
  --------------------------------
*/
String _lat;
String _lng;
const int LED = 13;
unsigned long time_now;
bool last_status_istilted = 0;
int tolerance_angle_fallen = 50;
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

  char* context = (char*)malloc(sizeof(char) * 60);          //

  _lat = String(gps.location.lat(), 6);
  _lng = String(gps.location.lng(), 6);

  char buf1[15];
  char buf2[15]; 

  _lat.toCharArray(buf1, 9);
  _lng.toCharArray(buf2, 9);

  ubidots.addContext("lat", buf1);
  ubidots.addContext("lng", buf2);

  ubidots.getContext(context);

  ubidots.add(pos, 1, context);

  ubidots.publish(device_name);

  Serial.println(buf1);
  Serial.println(buf2);
}


void set_LED() {}



void setup() {                                 
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);//This opens up communications to the GPS
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(device_name, temp_alarm);

  mpu.begin();
  mpu.calcOffsets(true, true); 


  ubidots.add(crash_alarm, 0);                //sets initial value for alarm and max_g
  ubidots.add(max_g_logged, 0);
  ubidots.publish(device_name);

  time_now = millis();
}

void loop() {
  if (!ubidots.connected()) {                                   // if broken, fix (and resub)
    ubidots.reconnect();
    ubidots.subscribeLastValue(device_name, temp_alarm);        
  }

  mpu.update();                         

                          
  current_acc_y = mpu.getAccY();                                //checking for maximum g-forces detected.                       
  if (abs(current_acc_y) > max_g) {                             
    max_g = abs(current_acc_y);
  }


/*
 * This if statement checks if a crash has occured. In the event of a crash 
 * the crash alarm will be set of and published, max_g and location of the crash 
 * is published. This information is the essence of the blackbox.  
 */
  if (abs(mpu.getAngleZ()) > tolerance_angle_fallen && !last_status_istilted) {         
    last_status_istilted = 1;
    ubidots.add(crash_alarm, last_status_istilted); 
    ubidots.add(max_g_logged, max_g);
    ubidots.publish(device_name);
    max_g = 0; 
    crash_location();
    
  }
// if the alarm is turned off, then crash alarms are turned off. 
  if (abs(mpu.getAngleZ()) < tolerance_angle_fallen && last_status_istilted) {
    last_status_istilted = 0;
    ubidots.add(crash_alarm, last_status_istilted);
    ubidots.publish(device_name);
    Serial.println(last_status_istilted);
  }

  if (time_now + 1000 < millis()){
    crash_location();
    time_now = millis(); 
  }

  ubidots.loop();
}
