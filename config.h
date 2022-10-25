#define SECRET_SSID "AP"
#define SECRET_PSW "lavit2ap_"

#define NTP_SERVER "10.2.10.8"
#define TIME_ZONE "CET-1CEST"

#define THINGNAME "remote_display"
#define MQTT_VERSION MQTT_VERSION_3_1_1
const char MQTT_HOST[] = "10.2.0.123";
const int MQTT_PORT = 1883;
//const char MQTT_USER[] = "";
//const char MQTT_PASSWORD[] = "";
#define TOPIC_PUB "display/button"
#define TOPIC_SUB "display/text"

void connectWiFi(void);
void NTPConnect(void);
void connectMQTT(void);
void publishMessage(void);
void subscribeCallback(char* topic, byte* payload, unsigned int length);