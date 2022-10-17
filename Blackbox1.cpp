
//Including libraries

#include "UbidotsEsp32Mqtt.h"

#include <Wire.h>
#include <analogWrite.h>        //needed for buzzer alarm



WiFiClient ubidots;                    //set names for libraries
PubSubClient client(ubidots);


const int blackbox_number = 1;

const char* ssid = "Get-2G-348E01";
const char* password = "Y8MYXJDY8C";
const char* token = "BBFF-SYkwlxbJDZ3j58oMZpnNZYi8EiuRYO";        //token inne i Device
const char* mqtt_client_name = "Blackbox1";                       // hva heter enheten

const char* crash_alarm = "crash_reported";                                 // topic for å publishe
const char* temp_alarm = "temperature_alarm";
const char* wind_alarm = "wind_alarm";
const char* device_name = "test_hore";                             // device navnet


char mqtt_broker[] = "industrial.api.ubidots.com";
char payload[100];
char topics[150];
char topic_subscribe[100];


Ubidots ubidots(token);


unsigned long time_now;
unsigned long time_now2;

bool temperature_alarm;
bool wind_alarm;

/*

  const char* GPS_topic = "skriv in topic";
  const char* speed_topic = "skriv in topic";
  const char* ALARM_topic = "skriv in topic";
*/

// float current_speed;    //brukes når vi skal gjøre et forsøk på aks--> fart


void crash_detected() {
  if ((crash_alarm) & (gyro_alarm)) {
    ubidots.add(crash_alarm, 1);                                    // setter opp melding
    ubidots.publish(device_name);                            //  publiserer melding
    sound_and_blink();
    sound_alarm = 1;
  }
}



void alarm() {
  sound_and_blink();
  ubidots.loop();
  if (reset_button) {
    while (button) {}
    sound_alarm = 0;
  }
}
}


void check_gyro();
// lag et sjekk gyro, og lagre.
// hvis gyro blir satt til 1, så initier tidsforsinkelse på 10 sek. Sjekk om gyro
// fortsatt er fucka, og hvis ja, så sound_alarm.
}



Float get_GPS(){
  ?? ?
}







float read_angle(int chan) {   //reads angle from sparkesykkel

  int byte_low = 0;
  int byte_high = 0;
  int raw_angle = 0;
  float deg_angle = 0;

  if (chan == 1) {
    I2Cone.beginTransmission(0x36);
    I2Cone.write(0x0D);
    I2Cone.endTransmission();

    I2Cone.requestFrom(0x36, 1);
    while (I2Cone.available()) {
      byte_low = I2Cone.read();
    }

    I2Cone.beginTransmission(0x36);
    I2Cone.write(0x0C);
    I2Cone.endTransmission();

    I2Cone.requestFrom(0x36, 1);
    while (I2Cone.available()) {
      byte_high = I2Cone.read();
    }

    byte_high = byte_high & 0b00001111;
    byte_high = byte_high << 8;
    raw_angle = byte_high | byte_low;
    deg_angle = raw_angle * 0.087890625;

    float corrected_angle = (deg_angle - initial_angle_1);

    if (corrected_angle >= 360) {
      corrected_angle -= 360;
    }
    if (corrected_angle < 0) {
      corrected_angle += 360;
    }

    return corrected_angle;
  }

  else if (chan == 2) {
    I2Ctwo.beginTransmission(0x36);
    I2Ctwo.write(0x0D);
    I2Ctwo.endTransmission();

    I2Ctwo.requestFrom(0x36, 1);
    while (I2Ctwo.available()) { // slave may send less than requested
      byte_low = I2Ctwo.read();
    }

    I2Ctwo.beginTransmission(0x36);
    I2Ctwo.write(0x0C);
    I2Ctwo.endTransmission();

    I2Ctwo.requestFrom(0x36, 1);
    while (I2Ctwo.available()) { // slave may send less than requested
      byte_high = I2Ctwo.read();
    }

    byte_high = byte_high & 0b00001111;
    byte_high = byte_high << 8;
    raw_angle = byte_high | byte_low;
    deg_angle = raw_angle * 0.087890625; //12 bits to 360 degrees

    float corrected_angle = (deg_angle - initial_angle_2);

    if (corrected_angle >= 360) {
      corrected_angle -= 360;
    }
    if (corrected_angle < 0) {
      corrected_angle += 360;
    }
    if (corrected_angle > theta_max) {
      gyro_ALARM = 1;
    }
    //    return corrected_angle;
  }
}



void callback(char* topic, byte * message, unsigned int length) {     //set callback function
  String espIN;

  for (int i = 0; i < length; i++) {               //This function collects all ints being sent to the ESP.
    espIN += (char)message[i];                      //Callback copied from RandomNerdsTutorials, references in report
  }
  Serial.println(espIN);
  if (espIN == "1.0") {
    temp_alarm = 1;
    Serial.println("alarm_på");
  } else if (espIN == "0.0") {
    Serial.println("alarm_av");
    temp_alarm = 0;
  } else if (espIN == "2.0") {
    wind_alarm = 1;
  } else if (espIN == "3.0") {
    wind_alarm = 0;
  }
}






void setup() {
  Serial.begin(9600);
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(device_name, wind_alarm);
  ubidots.subscribeLastValue(device_name, temp_alarm);

  pinMode();
  pinMode();
  pinMode();

  time_now = millis();
  time_now2 = millis();


  //  current_speed = 0;        //Dette er til når vi skal prøve oss på hastighet basert på aks


}



void loop() {
  if (!ubidots.connected()) {    //passer på at vi er connected
    ubidots.reconnect();
    ubidots.subscribeLastValue(device_name, temp_alarm);        //re-sub  //da henter vi ut infor når det skjer noe  her.
    ubidots.subscribeLastValue(device_name, wind_alarm);
  }
  ubidots.loop();



  check_gyro();

  if (time_now + 5000 < millis()) {
    get_gps();
  }


  if ((temperature_alarm) || (wind_alarm) || (sound_alarm)) {
    alarm();
  }

  if ((gyro_alarm) & (!sound_alarm)) {
    sound_alarm = 1;
    crash_detected();
  }
}


