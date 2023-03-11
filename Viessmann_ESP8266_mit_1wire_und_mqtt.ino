
// Import required libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#define ONEWIRE_PIN 2
//#define WEBSERIAL_DEBUG 1

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature DS18B20(&oneWire);

//mqtt
const char* mqtt_server = "ip-of-your-mqtt-server";
const int mqtt_port = 1883;
const char* mqtt_username = "mqtt-username";
const char* mqtt_password = "mqtt-password";
const char* mqtt_client = "esp_viessmann";

const char* wifi_ssid = "your-wifi-ssid";
const char* wifi_password = "your-wifi-password";
int wifi_port = 8888;

IPAddress staticIP(192,168,1,20);
IPAddress dns(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WiFiServer* server = NULL;
AsyncWebServer webserialServer(80);
WiFiClient serverClient;
WiFiClient mqttClient;
PubSubClient client(mqttClient);

unsigned long mqttPreviousMillis = 0;
unsigned long mqttInterval = 30000;   // 1-Wire alle 30 Sek ins mqtt publishen

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

void setup() {
  yield();

  //Test, ob der ESP nicht jedes Mal die WIFI Verbindung verliert
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  WebSerial.begin(&webserialServer);
  WebSerial.msgCallback(recvMsg);
  webserialServer.begin();
  
  WebSerial.print("Booting");

  Serial.begin(4800, SERIAL_8E2); // Vitotronic connection runs at 4800,E,2
  WebSerial.println("Serial port to Vitotronic opened at 4800bps, 8E2");

  // Initialize WiFi
  WiFi.disconnect();
  yield();
  WiFi.mode(WIFI_STA);
  yield();

  //set static IP configuration, if available
  WiFi.config(staticIP, dns, gateway, subnet);
  yield();

  uint8_t wifiAttempts = 0;
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED && wifiAttempts++ < 20) {
    WebSerial.println("Try WIFI Connect");
    delay(1000);
  }

  server = new WiFiServer(wifi_port);  
  server->begin();
  
  ArduinoOTA.setHostname(mqtt_client);

  ArduinoOTA.begin();

  client.setServer(mqtt_server, mqtt_port);

  // DS18B20 initialisieren
  DS18B20.begin();
}

void wifiSerialLoop() 
{
  uint8_t i;
  //check if there is a new client trying to connect
  if (server && server->hasClient()) {
    //check, if no client is already connected
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) serverClient.stop();
      serverClient = server->available();
      WebSerial.println("New client connected.\n");
    }
    else {
      //reject additional connection requests.
      WiFiClient serverClient = server->available();
      serverClient.stop();
    }
  }

  yield();

  //check client for data
  if (serverClient && serverClient.connected()) {
    size_t len = serverClient.available();
    if (len) {
      uint8_t sbuf[len];
      serverClient.read(sbuf, len);

      WebSerial.print("ServerClient Buffer: ");
      for (uint8_t n = 0; n < len; n++) {
        WebSerial.print(sbuf[n]);
      }
      
      // Write received WiFi data to serial
      Serial.write(sbuf, len);

      yield();

      // Debug output received WiFi data to Serial1
      WebSerial.println();
      WebSerial.print("WiFi: ");
      for (uint8_t n = 0; n < len; n++) {
        WebSerial.print(String(sbuf[n], HEX));
      }
      WebSerial.println();
    }
  }

  yield();

  //check UART for data
  size_t len = Serial.available();
  if (len) {
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);

    // Debug output received Serial data to Serial1
    WebSerial.println();
    WebSerial.print("Serial: ");
    for (uint8_t n = 0; n < len; n++) {
      //WebSerial.print(sprintf(sbuf[n], "%02x,").c_str());
      WebSerial.print(sbuf[n]);
    }
    WebSerial.println();

    yield();

    //push UART data to connected WiFi client
    if (serverClient && serverClient.connected()) {
      serverClient.write(sbuf, len);
    }
  }
}

void reconnectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    WebSerial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (!client.connect(mqtt_client, mqtt_username, mqtt_password)) {
      // Wait 5 seconds before retrying
      delay(5000);
    }
    WebSerial.println("MQTT Connected");
  }
}

void loop() {
  ArduinoOTA.handle();

  wifiSerialLoop();

  reconnectMqtt();

  unsigned long currentMillis = millis();
  
  if (currentMillis - mqttPreviousMillis >= mqttInterval) 
  {  
    mqttPreviousMillis = currentMillis;
    
    DS18B20.requestTemperatures(); 

    WebSerial.println("1-Wire Devices found: " + String(DS18B20.getDeviceCount()));

    for (int x = 0;  x < DS18B20.getDeviceCount(); x++)
    {
      byte addr[8];

      DS18B20.getAddress(addr, x);
      
      String MQTT_PUB_STRING = String(mqtt_client) + "/";
      for( byte i = 0; i < 8; i++) {
        MQTT_PUB_STRING = MQTT_PUB_STRING + String(addr[i]);
      }
      MQTT_PUB_STRING = MQTT_PUB_STRING + "/Temperature";

      client.publish(MQTT_PUB_STRING.c_str(), String(DS18B20.getTempCByIndex(x)).c_str());
      
      WebSerial.println("Publishing on topic " + MQTT_PUB_STRING +" with value " + String(DS18B20.getTempCByIndex(x)));
    }
  }
}
