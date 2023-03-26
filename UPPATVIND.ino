#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

int Mode = 0;
int FilterChange = 0;
int ModeGoal = 0;

const char* ssid = "YOURSSID";
const char* password = "YOURPASSAWORD";
const char* mqtt_server = "YOURMQTTSERVER";
const char* Hostname = "UPPÅTVIND"; //OTA and MQTT Hostname
const char* OTApass = "OTAPASSWORD";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  ArduinoOTA.setHostname(Hostname);
  ArduinoOTA.setPassword(OTApass);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg[i] = (char)payload[i];
  }
  Serial.println();
  Serial.println(msg);
  topic.trim();
  if (topic == "/home/data/UPPATVIND/setRotationSpeed") {
    int Speed = atof(msg);
    Speed = Speed, 0;
    ModeGoal = map(Speed, 1, 100, 1, 3);
    modeSet();
  }
  if (topic == "/home/data/UPPATVIND/setActive") {
    if (atof(msg) == 0) {
      ModeGoal = 0;
      modeSet();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = Hostname;
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("/home/data/UPPATVIND/setActive");
      client.subscribe("/home/data/UPPATVIND/setCurrentAirPurifierState");
      client.subscribe("/home/data/UPPATVIND/setRotationSpeed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void modeSet() {
  modeDetect();
  for (int i = 0; i <= 4; i++) {
    modeDetect();
    if (ModeGoal != Mode) {
      digitalWrite(5, LOW);
      delay(100);
      digitalWrite(5, HIGH);
      delay(100);
    }
  }
}
void modeDetect() {
  int PWM = 0;
  if (digitalRead(4) == HIGH) {
    FilterChange = 1;
  } else {
    FilterChange = 0;
  }
  for (int i = 0; i <= 250; i++) {
    PWM = PWM + analogRead(A0);
  }
  PWM = PWM / 250;
  if (PWM < 10) {
    Mode = 0;
  }
  if (PWM > 20 && PWM < 100) {
    Mode = 1;
  }
  if (PWM > 100 && PWM < 500) {
    Mode = 2;
  }
  if (PWM > 500) {
    Mode = 3;
  }
  PWM = 0;
}

void setup() {
  pinMode(5, OUTPUT); // Speedbutton
  pinMode(4, INPUT);  // Filter Change LED
  pinMode(A0, INPUT); // Speed LED
  digitalWrite(5, LOW);
  delay(1500);
  digitalWrite(5, HIGH);
  modeDetect();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    modeDetect();
    int Speed = Mode * 33;
    client.publish("/home/data/UPPATVIND/mode", String(Mode).c_str(), true);
    client.publish("/home/data/UPPATVIND/getRotationSpeed", String(Speed).c_str(), true);
    client.publish("/home/data/UPPATVIND/getCurrentAirPurifierState", String("AUTO").c_str(), true);
    client.publish("/home/data/UPPATVIND/getFilterChangeIndication", String(FilterChange).c_str(), true);
    if (Mode == 0) {
      client.publish("/home/data/UPPATVIND/getActive", "0");
    } else {
      client.publish("/home/data/UPPATVIND/getActive", "1");
    }
  }
}
