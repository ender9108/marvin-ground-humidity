#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SENSOR_H_ANALOG         0
#define SENSOR_H_DIGIT          2

struct Config {
  char* wifiSsid              = "{{ wifi_ssid }}";
  char* wifiPassword          = "{{ wifi_password }}";
  char* mqttIp                = "{{ mqtt_ip }}";
  int   mqttPort              = 1883;
  char* mqttUsername          = "{{ mqtt_username }}";
  char* mqttPassword          = "{{ mqtt_password }}";
  char* mqttPublishChannel    = "{{ mqtt_publish_channel }}";
  char* uuid                  = "{{ module_uuid }}";
};

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Config config;

int hground;  //Humidite su sol, mesure analogique
int dryness;  //0 ou 1 si seuil atteint

void setup(){ // Initialisation
    Serial.begin(9600);  //Connection série à 9600 baud

    // Init pins
    pinMode(SENSOR_H_ANALOG, INPUT);
    pinMode(SENSOR_H_DIGIT,  INPUT);

    // Init wifi
    wifiConnect();

    // Init mqtt
    mqttConnect();
    client.loop();

    // Read values
    hground = analogRead(SENSOR_H_ANALOG);
    dryness = digitalRead(SENSOR_H_DIGIT);

    String payload = "{\"name\":\"marvin-ground-humidity\", \"uuid\":\"dsf564sdfs64df6sdfsfs\",\"humidity\":" + String(humidityValueToPercent(hground)) + ",\"dryness\":" + String(dryness) + "}";

    if (false == client.publish(config.mqttPublishChannel, payload.c_str())) {
      Serial.println("Publish fail !");
    }

    // 3600e6 = 1 heure
    ESP.deepSleep(3600e6);
    // to debug deep sleep = 10s
    // ESP.deepSleep(10e6);
}

void loop() {}

float humidityValueToPercent(int value) {
  float humidityPercent = (value / 1024) * 100;
  return humidityPercent;
}

void wifiConnect() {
    WiFi.begin(config.wifiSsid, config.wifiPassword);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void mqttConnect() {
    client.setServer(config.mqttIp, config.mqttPort);

    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266", config.mqttUsername, config.mqttPassword)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

