#include <ESP8266WiFi.h>
#include "myconfig.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>

unsigned long  next_timestamp = 0;
volatile unsigned long last_micros;
long debouncing_time = 5; //in millis
int input_pin = 0;
int led_pin = 13;
int output_pin = 12;
char charBuffer[32];
bool relaisstate = LOW;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(input_pin, INPUT_PULLUP);
  pinMode(output_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(output_pin, relaisstate); 
  // We start by connecting to a WiFi network
  if(debugOutput){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int maxWait = 500;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led_pin, LOW);          // Blink to indicate setup
    delay(500);
    if(debugOutput) Serial.print(".");
    if(maxWait <= 0)
     ESP.restart();
    maxWait--;
    digitalWrite(led_pin, HIGH);
  }
  if(debugOutput){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  delay(500);
  do_update();
  client.setServer(mqtt_host, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void sendRelaisStatus()
{
  String strBuffer;
  strBuffer =  String(relaisstate);
  strBuffer.toCharArray(charBuffer,10);
  if (!client.publish(mqtt_topic_state, charBuffer, false))
  {
    ESP.restart();
    delay(100);
  }
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  if(debugOutput){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  }
  if ((char)payload[0] == '0') {   
    relaisstate = LOW;
    digitalWrite(led_pin, HIGH);
  } else if ((char)payload[0] == '1') { 
    relaisstate = HIGH;
    digitalWrite(led_pin, LOW);
  } else if ((char)payload[0] == '2') {
    digitalWrite(led_pin, relaisstate);
    relaisstate = !relaisstate;
  }
  if(debugOutput){ Serial.print("relaisstate = "); Serial.println(relaisstate); }
  digitalWrite(output_pin, relaisstate);
  
}

void loop() 
{
  if (millis() > next_timestamp )    
  {    
    next_timestamp  = millis()+10000; //intervall is 10s
    sendRelaisStatus();
  }
  if(WiFi.status() != WL_CONNECTED) {
      ESP.restart();
      delay(100);
  }
  client.loop();
  yield(); 
}

void reconnect() {
  int maxWait = 0;
  while (!client.connected()) {
    if(debugOutput) Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_id)) {
      if(debugOutput) Serial.println("connected");
      client.subscribe(mqtt_topic_set);
    } else {
      if(debugOutput){ 
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
      delay(5000);
      if(maxWait > 10)
        ESP.restart();
      maxWait++;
    }
  }
}

void do_update(){
  if(debugOutput) Serial.println("do update");
  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri, firmware_version);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        if(debugOutput) Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        if(debugOutput )Serial.println("[update] no Update needed");
        break;
    case HTTP_UPDATE_OK:
        if(debugOutput) Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
