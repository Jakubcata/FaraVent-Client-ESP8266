#if 0
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

String ssid;
String password;
String mqtt_server;
String mqtt_name;
String mqtt_password;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}



#endif 

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Wire.h>
#include "Interrupt.h"
#include "MotionSensor.h"
#include "ShtSensor.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <Arduino.h>
//#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

String ssid;
String password;
String mqtt_server;
String mqtt_name;
String mqtt_password;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


ShtSensor TempHumSesnor = ShtSensor( 21, 22 );
MotionSensor MotSensor = MotionSensor( 15 );

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  ssid = "Jakubcata & BW";//read_String(0);
  password = "NaJednejCeste";//read_String(30);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.print("\n");

  if(strcmp(topic,"nova_skusobna_update") == 0){
    DynamicJsonDocument json(150);
    deserializeJson(json, (char*)payload);
    JsonObject update_meta = json.as<JsonObject>();
    const char* host = update_meta["host"];
    const char* path = update_meta["path"];

    Serial.printf("%s %s\n", host, path);
 /*   t_httpUpdate_return ret = ESPhttpUpdate.update(host, 80, path);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      ESP.restart();
    }
  } else{
    Serial.print("no update\n");
  }
  */
  }
    
  

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW);
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (  client.connect("nova_skusobna", mqtt_name.c_str(), mqtt_password.c_str()) )
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("nova_skusobna_in");
      client.subscribe("nova_skusobna_update");

    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  Serial.printf("Version 2.2\n");
  EEPROM.begin(512);

  mqtt_server = "mqtt.faravent.jakubcata.eu";//read_String(60);
  mqtt_name = "jakubcata";//read_String(100);
  mqtt_password = "jakubcata2005";//read_String(130);
  setup_wifi();
  client.setServer(mqtt_server.c_str(), 1883);
  client.setCallback(callback);
}

void loop()
{
    static uint64_t timestamp;

    if (!client.connected())
    {
      reconnect();
    }
    client.loop();


    // Do update of the sensor data
    TempHumSesnor.Update();

    digitalWrite( BUILTIN_LED, !MotSensor.IsMovement() );

    uint64_t now = millis();
    if (now - timestamp >= 2000)
    {   


        timestamp = now;

        Serial.println("500ms elapsed!");

        Serial.print("Temperature: ");
        Serial.println( TempHumSesnor.GetTemperature() );
        Serial.print("Humidity: ");
        Serial.println( TempHumSesnor.GetHumidity() );
        StaticJsonDocument<200> doc;
        doc["temp"] = TempHumSesnor.GetTemperature();
        doc["hum"] = TempHumSesnor.GetHumidity();
        doc["signl"] = WiFi.RSSI();

        doc["version"] = "0.2";

        serializeJson(doc, msg);
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("nova_skusobna_out", msg);
    }
}