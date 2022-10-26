#include "DHT.h"
#include "UbidotsEsp32Mqtt.h"   //må ha lastet ned biblioteket


#define DHTTYPE DHT22 //setup of type of dht and pin 
#define DHTPIN 4


//const char* ssid = "Get-2G-348E01";
//const char* password = "Y8MYXJDY8C";
const char* ssid = "Qwerty";
const char* password = "Lysestake1";
const char* token = "BBFF-LFwcUHHKuZGSbGnUFQiv2qfqQIaTQ1";        //token in the Device
const char* mqtt_client_name = "weatherstation";                       // unit name 

const char* temp_now = "temp_topic";                                 // topic for å publishe
const char* temp_alarm = "temp_alarm"; 
const char* wind_now = "wind_topic";
const char* wind_alarm = "wind_alarm";
const char* device_name = "weatherstation";                             // device navnet

Ubidots ubidots(token);

RTC_DATA_ATTR float last_sendt_temp; // save value between deep sleep
RTC_DATA_ATTR float last_sensor_alarm;


DHT dht(DHTPIN, DHTTYPE); //setup for DHT sensor


bool sensor_alarm = 0;



void callback(char* topic, byte * message, unsigned int length) {     //set callback function
  String espIN;

  for (int i = 0; i < length; i++) {               //This function collects all ints being sent to the ESP.
    espIN += (char)message[i];                      //Callback copied from RandomNerdsTutorials, references in report
  }
}


void get_sensor_data() {
  float temp = dht.readTemperature(); //reading temprature
  float hum = dht.readHumidity(); //reading hum
  //if ((temp > 4) & (sensor_alarm) & (ubidots.connected())) {
  if ((temp > 23) & (last_sensor_alarm == 1)) {
    sensor_alarm = 0;
    ubidots.add(temp_alarm, 0);
    ubidots.publish(device_name);
    last_sensor_alarm = sensor_alarm;
  }
  if (temp <= last_sendt_temp - 0.2 || temp >= last_sendt_temp + 0.2 ) { //seeing if we need to change temprarure
    //Serial.println(temp); //printing temprature
    last_sendt_temp = temp; //setting new temprature
    ubidots.add(temp_now, temp);
    ubidots.publish(device_name);
  }
  if (temp <= 23) {
    ubidots.add(temp_alarm, 1);
    ubidots.publish(device_name);
    sensor_alarm = 1;
    last_sensor_alarm = sensor_alarm;
  }
}


void sleep(int time) { //function for the deep sleep
  Serial.println("going to sleep");
  Serial.flush(); //flushing out the serial monitor
  esp_deep_sleep(time * 1000000); //going in to the esp deep sleep
}
void setup() {
  Serial.begin(115200); //starting the Serial communication
  dht.begin();  //starting dht sensor
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  if ((last_sensor_alarm == 0) & (last_sendt_temp > 23)){
    ubidots.add(temp_alarm, 0);
    ubidots.publish(device_name);
  }
}
void loop() {
  if (!ubidots.connected()) {    //passer på at vi er connected
    ubidots.reconnect();
    //ubidots.subscribeLastValue(device_name, temp_alarm);        //re-sub  //da henter vi ut infor når det skjer noe  her.
  }
  ubidots.loop();
  for (int i; i < 10; i++){
     get_sensor_data(); //running function of the dht sensor
  }
  
  sleep(5); //running the sleep function (seconds)

}
