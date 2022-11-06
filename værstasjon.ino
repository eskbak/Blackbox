#include "DHT.h"
#include "UbidotsEsp32Mqtt.h" 


#define DHTTYPE DHT22
#define DHTPIN 4

const char* ssid = "Vegard sin iPhone";
const char* password = "88888888";
const char* token = "BBFF-LFwcUHHKuZGSbGnUFQiv2qfqQIaTQ1";             // Token in the device weatherstation (ubidots)
const char* mqtt_client_name = "weatherstation";                       // Name shown in ubidots

const char* temp_now = "temp_topic";                                   // Api label in variable-names (ubidots)
const char* temp_alarm = "temp_alarm"; 
const char* device_name = "weatherstation";                            // Api label in device (ubidots)

Ubidots ubidots(token);

float last_sendt_temp = 999; 
float last_sensor_alarm;
bool sensor_alarm = 0;
int temp_threshold = 4;                                                // Lower limit for low temp warning

DHT dht(DHTPIN, DHTTYPE); 





void get_sensor_data() {
    float live_temp = dht.readTemperature();
    
    if ((live_temp > temp_threshold) & (last_sensor_alarm == 1)){                // If temp is above threshold (4 celcius) and temp warning is active, turn off temp warning
        sensor_alarm = 0;
        ubidots.add(temp_alarm, 0);
        ubidots.publish(device_name);
        last_sensor_alarm = sensor_alarm;
    }
    if(abs(live_temp - last_sendt_temp) > 0.2){                                  // If the change in temperature is greater than 0.2 Send the new temperature to ubidots
        last_sendt_temp = live_temp;
        ubidots.add(temp_now, live_temp);
        ubidots.publish(device_name);
    }
    if (live_temp <= temp_threshold) {                                           // If current temoperature is lower then warning threshold, Send temp warning to ubidots
        ubidots.add(temp_alarm, 1);
        ubidots.publish(device_name);
        sensor_alarm = 1;
        last_sensor_alarm = sensor_alarm;
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin(); 
    ubidots.connectToWifi(ssid, password);
    ubidots.setup();
    ubidots.reconnect();

    ubidots.add(temp_alarm, 0);
    ubidots.publish(device_name);
}
void loop() {
    if (!ubidots.connected()){ 
        ubidots.reconnect();
    }
    ubidots.loop();
    
    get_sensor_data();
    delay(500);
}
