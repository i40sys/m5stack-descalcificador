#include <M5Stack.h>
#include <WiFi.h>
#include <ESPDateTime.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiClient net;
PubSubClient client(net);

void connectWiFi(void) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(1000);

  WiFi.begin(SECRET_SSID, SECRET_PSW);
  Serial.println(String("Connecting SSID: ") + String(SECRET_SSID));
  M5.Lcd.println(String("Connecting SSID: ") + String(SECRET_SSID));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

  M5.Lcd.println("Connected!");
  M5.Lcd.print("IP: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.print("SSID: ");
  M5.Lcd.println(WiFi.SSID());
  M5.Lcd.print("RSSI: ");
  M5.Lcd.println(WiFi.RSSI());
}

void NTPConnect(void) {
  DateTime.setServer(NTP_SERVER);
  DateTime.setTimeZone(TIME_ZONE);
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("NTP server: fail.");
  } else {
    Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    Serial.printf("Timestamp is %ld\n", DateTime.now());
  }
}

void connectMQTT(void) {
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(subscribeCallback);
  Serial.println("Connecting MQTT Broker...");
  M5.Lcd.println("Connecting MQTT Broker...");

  //while (!client.connect(THINGNAME, MQTT_USER, MQTT_PASSWORD))
  while (!client.connect(THINGNAME))
  {
    Serial.println(client.state());
    M5.Lcd.print(".");
    delay(1000);
  }
  if (!client.connected()) {
    Serial.println("Timeout!");
    M5.Lcd.println("Timeout!");
    return;
  }
  Serial.println("Connected!");
  M5.Lcd.println("Connected!");
  client.subscribe(TOPIC_SUB);
}

void publishMessage(void) {
  StaticJsonDocument<200> doc;
  doc["ts"] = DateTime.toISOString();
  doc["epoch"] = time(nullptr);
  doc["button"] = "A";
  doc["status"] = "pressed";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
 
  client.publish(TOPIC_PUB, jsonBuffer);
}

// Callback function
void subscribeCallback(char* topic, byte* payload, unsigned int length) {
  String s = "";
  for (int i = 0; i < length; i++) {  
    s.concat((char)payload[i]);
  }

  M5.Lcd.clear();
  M5.Lcd.setCursor(0,0);
  M5.lcd.setTextSize(2);
  M5.Lcd.printf("%s\n", DateTime.toISOString().c_str());
  M5.Lcd.println("");
  Serial.printf("%s\n", DateTime.toISOString().c_str());
  M5.lcd.setTextSize(5);
  M5.Lcd.println(s);
  Serial.println(s);
}

void setup(){
  M5.begin(115200);
  M5.Power.begin();
  
  M5.lcd.setTextSize(2);
  M5.Lcd.println("Booting...");
  Serial.println("Booting...");

  connectWiFi();
  NTPConnect();
  connectMQTT();
  delay(5000);
}

void loop() {
  M5.update();
  if (!client.connected()) {
    connectMQTT();
  } else {
    client.loop();
    if (M5.BtnA.wasPressed()) {
      M5.lcd.setTextSize(2);
      M5.Lcd.println("");
      M5.Lcd.println("Processing...");
      Serial.println("Processing...");
      publishMessage();
      Serial.println("Button A, pressed.");
    }
  }
}
