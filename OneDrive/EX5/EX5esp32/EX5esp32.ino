#include <WiFi.h>
#include <PubSubClient.h>
#define LED2 19
#define LED3 18
#define LED4 5
#define ptm 34
#define mq2 35
#define echoPin 12
#define trigPin 13

char* ssid = "Galaxy A71E343";
const char* password = "11111111";
const char* mqtt_server = "192.168.103.208";
const int mqtt_port = 1883;
const char* mqtt_Client = "esp32";
const char* mqtt_username = "training";
const char* mqtt_password = "123456789";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[100];
int maximumRange = 200; 
int minimumRange = 0; 
long duration, distance;
long past = 0;
float voltage = 3.3;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/led2");
      client.subscribe("/led3");
      client.subscribe("/led4");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == "/led2") {
    if (message == "on") {
      digitalWrite(LED2, 0);
      client.publish("/led2/update", "1");
      Serial.println("LED on");
    }
    else if (message == "off") {
      digitalWrite(LED2, 1);
      client.publish("/led2/update", "0");
      Serial.println("LED off");
    }
  }
   if (String(topic) == "/led3") {
    if (message == "on") {
      digitalWrite(LED3, 0);
      client.publish("/led3/update", "1");
      Serial.println("LED on");
    }
    else if (message == "off") {
      digitalWrite(LED3, 1);
      client.publish("/led3/update", "0");
      Serial.println("LED off");
    }
  }
   if (String(topic) == "/led4") {
    if (message == "on") {
      digitalWrite(LED4, 0);
      client.publish("/led4/update", "1");
      Serial.println("LED on");
    }
    else if (message == "off") {
      digitalWrite(LED4, 1);
      client.publish("/led4/update", "0");
      Serial.println("LED off");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(LED2, OUTPUT),pinMode(LED2, OUTPUT),pinMode(LED2, OUTPUT);
  digitalWrite(LED2, 1),digitalWrite(LED3, 1),digitalWrite(LED4, 1);
}

void loop() { 
  int volts = int((voltage*analogRead(ptm))/4096);
  int gas = int(analogRead(mq2));
  Serial.println(volts)+("V");
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
   if (distance >= maximumRange || distance <= minimumRange) {
    Serial.println("-1");  //เมื่ออยู่นอกระยะให้ใช้ Print -1
  }
  else {
    Serial.println(distance);  //แสดงค่าระยะทาง
  }
  delay(10);
if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long start = millis();
  if (start - past >= 1000) {
    String payload = String(volts) + ":" + String(distance) + ":" + String(gas);
    Serial.println(payload);
    payload.toCharArray(msg, (payload.length() + 1));
    client.publish("/data2", msg);
    past = start;
  }
}
