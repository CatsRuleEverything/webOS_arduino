#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>

//sensor
int digitalPin = 22;
char msg[50];
int value;

// WiFi
const char *ssid = "Arduino"; // Enter your WiFi name
const char *password = "kernel1234";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *subTopic = "esp/door/open/get";
const char *pubTopic = "esp/door/open";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(String topic, byte*message, unsigned int length){
  Serial.println("callback");
  String messageTemp;
  for(int i=0;i<length;i++){
    messageTemp += (char)message[i];
  }
  if(topic == subTopic){
    Serial.println("doorPING");
    pubSensor();
  }
}

void pubSensor(){

  sprintf(msg, "%c", value);
  if(!value){
    client.publish(pubTopic, "open"); 
  }
  else client.publish(pubTopic, "closed"); 
    Serial.println("publish");
}

void setup() {

  // sensor
 pinMode(digitalPin, INPUT);
 value = digitalRead(digitalPin);
  
 // Set software serial baud to 115200;
 Serial.begin(9600);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str())) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 client.setCallback(callback);
 client.subscribe(subTopic); 

}


void loop() {
  value = digitalRead(digitalPin);
  //pubSensor();
 client.loop();
}
