#include "DHT.h"
#include "UbidotsEsp32Mqtt.h"   


#define DHTTYPE DHT22 
#define DHTPIN 4


const char* ssid = "Qwerty";
const char* password = "Lysestake1";
const char* token = "BBFF-LFwcUHHKuZGSbGnUFQiv2qfqQIaTQ1";               // Token in the Device (ubidots)
const char* mqtt_client_name = "weatherstation";                         // Name of the unit in (ubidots) 

const char* temp_now = "temp_topic";                                     // Topic for publishing under.(Api label in a variable)(ubidots)
const char* temp_alarm = "temp_alarm"; 
const char* device_name = "weatherstation";                              // Api label of the device (ubidots)

Ubidots ubidots(token);

RTC_DATA_ATTR float last_sent_temp;                                      // save value between deep sleep
RTC_DATA_ATTR float last_sensor_alarm;


DHT dht(DHTPIN, DHTTYPE);                                                //setup for DHT sensor


bool sensor_alarm = 0;
int temp_alarm_threshold = 4;                                            //minimum temperature for warning to be sent



void get_sensor_data() {
  float temp_reading = dht.readTemperature();

  if ((temp_reading > temp_alarm_threshold) && (last_sensor_alarm)) {     // If the temp warninng is active and our new reading is above warning threshold
    temp_alarm = 0;                                                       // Turn the temp warning off
    ubidots.add(temp_alarm, 0);
    ubidots.publish(device_name);
    last_sensor_alarm = sensor_alarm;
  }

  if (abs(temp_reading - last_sent_temp) >= 0.5) {                        // If change in temp from last sent value is bigger than 0.5 degrees 
    last_sent_temp = temp_reading;                                        // Publish the new temp to ubidots
    ubidots.add(temp_now, temp_reading);
    ubidots.publish(device_name);
  }

  if (temp_reading <= temp_alarm_threshold) {                             // If the temp reading is under the threshold 
    ubidots.add(temp_alarm, 1);                                           // Send low temp warning 
    ubidots.publish(device_name); 
    sensor_alarm = 1;
    last_sensor_alarm = sensor_alarm;
  }
}


void sleep(int time) {                                                    // Using deep sleep but as a function of seconds
  Serial.println("going to sleep");
  Serial.flush();
  esp_deep_sleep(time * 1000000); 
}

void setup() {
  Serial.begin(115200); 
  dht.begin(); 
  ubidots.connectToWifi(ssid, password);
  ubidots.setup();
  ubidots.reconnect();
}

void loop() {
  if (!ubidots.connected()) {                                             // Making sure we are connected
    ubidots.reconnect();       
  }
  ubidots.loop();

  get_sensor_data();

  sleep(300);                                                           
  
}
