#include "UbidotsEsp32Mqtt.h" 

#define DHTTYPE DHT22
#define DHTPIN 4

const char* ssid = "Get-2G-348E01";
const char* password = "Y8MYXJDY8C";
const char* token = "BBFF-LFwcUHHKuZGSbGnUFQiv2qfqQIaTQ1";                              // Token in the device (ubidots)
const char* mqtt_client_name = "Weatherstation";                                        // Name of the unit in (ubidots)

const char* temp_now = "temp_topic";                                                    // Topics for publishing under.(Api label in a variable)(ubidots)
const char* temp_alarm = "temp_alarm";
const char* device_name = "weatherstation";                                             // Api label of the device (ubidots)

Ubidots ubidots(token);

RTC_DATA_ATTR float last_sendt_temp;                                                    // Saves values in between deep sleep
RTC_DATA_ATTR float last_sendt_hum;

DHT dht(DHTPIN, DHTTYPE);                                                               // Setup for DHT sensor

bool temp_alarm = 0;
int temp_alarm_threshold = 4;                                                           // Minimum temperature for the warning to be sent

void callback(char* topic, byte * message, unsigned int length){
    String espIN;
    for (int i = 0; i < length; i++) {                                                  // This function collects all ints being sent to the ESP.
        espIN += (char)message[i];                                                      // Callback copied from RandomNerdsTutorials, references in report
    }
}

void get_sensor_data(){
    float temp_reading = dht.readTemperature();
    
    if ((temp_alarm) & (temp_reading > temp_alarm_threshold)){                          // If the temp warning is active and our new reading is above the warning threshold.                                                                                                // Turn warning off
        temp_alarm = 0;                                                                 // Turn the temp warning off
    } 
    else if (abs(temp_reading - last_sent_temp) >= 0.5)                                 // If the change in temp from last sent value is bigger than 0.5 degrees
        last_sendt_temp = temp_reading;                                                 // Publish the new temp to ubidots
        ubidots.add(temp_now, temp_reading);
        unidots.publish(device_name);
    } 
    else if (temp_reading <= temp_alarm_threshold){                                     // If the temp reading us under the threshold
        unidots.add(temp_alarm, 1);                                                     // Send low temp warning
        unidots.publish(device_name);
        temp_alarm = 1;
    }
}

void sleep(int time){                                                                   // Using deep sleep but as a function of seconds.
    esp_deep_sleep(time * 1000000);
}
void setup(){
    Serial.begin(115200);
    dht.begin();
}

void loop(){
    if (!ubidots.connected()){                                                          // Makes sure we keep a connection to ubidots
        ubidots.reconnect();
        ubidots.subscribeLastValue(device_name, temp_alarm_in);                  
    }
    ubidots.loop();
    
    get_sensor_data();
    sleep(5);
}
