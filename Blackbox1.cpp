
//Including libraries
#include <WiFi.h>         
#include <PubSubClient.h>
#include <Wire.h>

WiFiClient espClient;                    //set names for libraries
PubSubClient client(espClient);

const char* ssid = "Prosjektnett";
const char* password = "14159265";       //ssid and passord

const char* id = "Blackbox1";             // Naming unit

const char* mqtt_server =  "10.0.0.6";   //mqtt server on the RPi

// set variables

unsigned long time_now = 0;
unsigned long time_now2 = 0; 



const char* GPS_topic = "skriv in topic";
const char* speed_topic = "skriv in topic";
const char* ALARM_topic = "skriv in topic"; 


float current_speed;




void alarm(){
  while (crash_alarm == 1 and gyro_alarm == 1){
    bip bop; 
    sound sound; 
    if ( gyro < crash vinkel & reset_button == pressed){
      crash_alarm = 0;
      gyro_alarm= 0; 
      espOUT(alarm_over, ALARM_topic);
    }
  }
}






void get_SPEED() {                        //assu ming x is forward direction. Get libby to red acc, and test. Check with standard angle to see if we need
  if(acc_x >= 0){                         // to incorporate any acceleration from the other direvtions. We should make a correctio to direction based on startup gyro values. 
  current_speed += 1/2 * acc**2;          //add the integrated value to the speed. 
  } else if (acc_x < 0 ) {
    current_speed -= 1/2 * acc**2;        // subtract the integrated value from the speed. 
  }


String convert_SPEED(){
  char speed_string[10]; 
  dtostrf(current_speed, 1, 1, speed_string)
  return speed_string;
}



String get_GPS{
  ???
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
    if (corrected_angle > theta_max){
      gyro_ALARM = 1; 
    }
//    return corrected_angle;
  }
}



// hva gjør denne? 
float theta_to_phi(float theta) {
  float theta_rad = (theta - 180) * 0.01745329251; // pi/180 = 0.01745329251
  return asin(R * sin(theta_rad)) * 57.2957795131;
}







// kan godt fjerne Serial.print - unødvendig, men nyttig for testing.  Kan evt returne stringen vår. 
void callback(char* topic, byte* message, unsigned int length) {      //set callback function
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String espIN;
  
  for (int i = 0; i < length; i++) {               //This function collects all ints being sent to the ESP. 
    Serial.print((char)message[i]);                
    espIN += (char)message[i];                      //Callback copied from RandomNerdsTutorials, references in report
  }
  if(espIN == " noe? ") {
    alarm_ICE();
  } else if (espIN == " noe annet "){
    alarm_WIND(); 
  }
}


 // test with string input.
void esp_string_out( String payload, String topic) {                      //Publishing strings to mqtt. 
  String message = payload;                                               //Can be used if we read from another ESP used as a slave. 
  String destination = topic;
  char Buf1[50];                                                          //creating a  buffer char array, which is used to store messages.
  char Buf2[50];                                                            
  if ((gyroskopet over en viss vinkel) & (Krasjet)) {                     //check to see which message to transmit. 
    message.toCharArray(Buf1, 50); 
    topic.toCharArray(Buf2, 50);                                          //gjør om beskjeden til char array. Buf er nå array med beskjeden
    const char* a = Buf1;                                                 //Set topic
    const char* b = Buf2;                                                 //choose message
    client.publish(a, b);                                                 //publishing to MQTT topic
}
       



void setupWifi() {                //wifi setup
  Serial.print("Kobler til ");    
  Serial.println(ssid);
  WiFi.disconnect();              //making sure of a clean connection
  delay(2000);                      
  WiFi.begin(ssid, password);     //connect

  while (WiFi.status() != WL_CONNECTED) {     //wait till connected
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi tilkoblet");
  Serial.println("IP Adresse: ");
  Serial.println(WiFi.localIP());    // print IP adress when connected
  //Connected :) 
}





void setupMQTT() {                         
  client.setServer(mqtt_server, 1883);     //sets the server and port on  RPi
  client.setCallback(callback);            // sets callback function
  client.setClient(espClient);             
  client.connect(id);                      //connects, and connects as "id"
  while (!client.connected()) {
    if ((time_now + 1000) < millis()) {
      time_now = millis();
      Serial.print(client.state());
      Serial.print(".");                   
    }
  }
  if (client.connected()) {                   //When connected, connect to all the topics we need
    client.subscribe("topic1");    
    client.subscribe("topic2");
    client.subscribe("topic3");
    client.subscribe("topic4");
    client.subscribe("topic5");
    Serial.println("mqtt is up and running");            //confirms MQTT is operational
                                                     
  
  }
  // sjekk topics for dårlig vær her, hvis ok, så blink 3 ganger med grønt lys. 
}

                                     



void setup() {                    
  Serial.begin(115200);           
  setupWifi();                   
  setupMQTT();
  pinMode();
  pinMode(); 
  pinMode();
  current_speed = 0;                      
}



void loop() {
  if (!client.connected()) {  // Ensures MQTT connection
    client.connect(id);       
  }
  client.loop();              //Checks topics we are subscribed to. 

  get_SPEED();


   
  if (time_now + 1000 > millis()) {              // publishes every 5 seconds
    String SPEED = speed_convert(current_SPEED); 
    espOUT(SPEED, speed_topic);                 //publiserer speed, gps location to mqtt when driving
 
    String GPS = get_GPS(); 
    espOUT(GPS, GPS_topic);
    time_now = millis(); 
  }


 if(time_now2 + 5000 > millis() {
    read_angle(); 

    if(gyro_alarm == 1 & crash_alarm == 1){     
      espOUT(alarm, ALARM_topic);
      alarm(); 
    }
    time_now2 = millis(); 
    
  } 
 //  deep.sleep eller hvordan man gjør det. Jeg vil at dette skal interruptes når Sparkesykkelen starter. ESP er aktiv  så lenge sparkesykkel står stille
  // skal den automatisk gå i deep sleep
}
