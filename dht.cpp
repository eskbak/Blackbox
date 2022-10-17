
#include "UbidotsEsp32Mqtt.h"   //må ha lastet ned biblioteket


#define DHTTYPE DHT22 //setup of type of dht and pin 
#define DHTPIN 4


const char* ssid = "Get-2G-348E01";
const char* password = "Y8MYXJDY8C";
const char* token = "BBFF-SYkwlxbJDZ3j58oMZpnNZYi8EiuRYO";        //token inne i Device
const char* mqtt_client_name = "Blackbox1";                       // hva heter enheten

const char* temp_now = "temp_topic";                                 // topic for å publishe
const char* temp_alarm = "temp_alarm";
const char* wind_now = "wind_topic";
const char* wind_alarm = "wind_alarm";
const char* device_name = "blackbox_1";                             // device navnet

Ubidots ubidots(token);

RTC_DATA_ATTR float last_sendt_temp; // save value between deep sleep
RTC_DATA_ATTR float last_sendt_hum;

DHT dht(DHTPIN, DHTTYPE); //setup for DHT sensor


bool temp_alarm = 0;



void callback(char* topic, byte * message, unsigned int length) {     //set callback function
  String espIN;

  for (int i = 0; i < length; i++) {               //This function collects all ints being sent to the ESP.
    espIN += (char)message[i];                      //Callback copied from RandomNerdsTutorials, references in report
  }
}


void get_sensor_data() {
  float temp = dht.readTemperature(); //reading temprature
  float hum = dht.readHumidity(); //reading hum
  if ((temp > 4) & (temp_alarm)) {
    temp_alarm = 0;
  } else if (temp <= last_sendt_temp - 0.5 || temp >= last_sendt_temp + 0.5 ) { //seeing if we need to change temprarure
    //Serial.println(temp); //printing temprature
    last_sendt_temp = temp; //setting new temprature
    ubidots.add(temp_now, temp);
    unidots.publish(blackbox_1);

  } else if (temp <= 4) {
    unidots.add(temp_alarm, 1);
    unidots.publish(blackbox_1);
    temp_alarm = 1;
  }
  if (hum <= last_sendt_hum - 5 || hum >= last_sendt_hum + 5) { //seeing if we need to change hummidity
    //Serial.println(hum); //printing hummidity                  // do we need hum?
    last_sendt_hum = hum; //setting new hummidity
  }
}

void sleep(int time) { //function for the deep sleep
  //Serial.println("going to sleep");
  //Serial.flush(); //flushing out the serial monitor
  esp_deep_sleep(time * 1000000); //going in to the esp deep sleep
}
void setup() {
  Serial.begin(115200); //starting the Serial communication
  dht.begin();  //starting dht sensor
}
d
void loop() {
  if (!ubidots.connected()) {    //passer på at vi er connected
    ubidots.reconnect();
    ubidots.subscribeLastValue(device_name, temp_alarm_in);        //re-sub  //da henter vi ut infor når det skjer noe  her.
  }
  ubidots.loop();


  get_sensor_data(); //running function of the dht sensor


  sleep(5); //running the sleep function (seconds)

}
