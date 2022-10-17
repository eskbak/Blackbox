

#include "UbidotsEsp32Mqtt.h"
#include <Wire.h>
#include <analogWrite.h>        //needed for buzzer alarm





//WiFiClient ubidots;                    //set names for libraries
//PubSubClient client(ubidots);



/*
*/

const char* ssid = "Get-2G-348E01";
const char* password = "Y8MYXJDY8C";
const char* token = "BBFF-SYkwlxbJDZ3j58oMZpnNZYi8EiuRYO";        //token inne i Device
const char* mqtt_client_name = "Blackbox1";                       // hva heter enheten

const char* topic = "test_gauge";                                 // topic for å publishe
const char* temp_alarm_in = "test_lol";
//const char* wind_alarm_in = "";
const char* device_name = "test_hore";                             // device navnet

unsigned long time_now;


Ubidots ubidots(token);
/*
  // kan godt fjerne Serial.print - unødvendig, men nyttig for testing.  Kan evt returne stringen vår.
  void callback(char* topic, byte * message, unsigned int length) {     //set callback function
  //  Serial.print("Message arrived on topic: ");
  //  Serial.print(topic);
  //  Serial.print(". Message: ");
  String espIN;

  for (int i = 0; i < length; i++) {               //This function collects all ints being sent to the ESP.
    Serial.print((char)message[i]);
    espIN += (char)message[i];                      //Callback copied from RandomNerdsTutorials, references in report
  }
  if (espIN == "1.00") {
    Serial.println("Alarm_på");
    //     alarm_ICE();
    //  } else if (espIN == " noe annet ") {
    //    alarm_WIND();
    //}
  }

  }
*/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}



void setup() {                                  //sier seg selv
  Serial.begin(9600);
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  //   ubidots.subscribeLastValue(device_name, );


  time_now = millis();

}

void loop() {
  if (!ubidots.connected()) {    //passer på at vi er connected
    ubidots.reconnect();
    ubidots.subscribeLastValue(device_name, temp_alarm_in);        //re-sub  //da henter vi ut infor når det skjer noe  her.
  }



  // put your main code here, to run repeatedly:

  if (time_now + 100000000 < millis()) {
    float r = 1.0;
    ubidots.add(topic, r);                                    // setter opp melding
    ubidots.publish(device_name);                            //  publiserer melding
    time_now = millis();
  }

}
