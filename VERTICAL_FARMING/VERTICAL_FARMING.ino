#include <WiFi.h>
#include <PubSubClient.h>

// Update the following constants with your MQTT broker information
const char* mqttServer = "10.39.63.26";
const int mqttPort = 1883;
const char* mqttUsername = "your_username";
const char* mqttPassword = "1234";
const char* clientId = "esp32-client";

// Water level sensor
const int waterLevelPin = 33; // Pin connected to water level sensor
const int maxWaterLevel = 4095; // Maximum water level threshold
const int minWaterLevel = 0; // Minimum water level threshold

// pH electrode probe
const int pHProbePin = 32; // Pin connected to pH electrode probe
const int pHRange = 1; // Range within which pH is considered balanced

// Wi-Fi credentials
const char* ssid = "Redmi Note 7";
const char* password = "12345678";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  // Loop until we're reconnected to the MQTT broker or timeout occurs
  unsigned long startTime = millis();
  while (!client.connected() && millis() - startTime < 10000) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId, mqttUsername, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging

  // Enable MQTT debugging
  // client.setDebug(true);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Set MQTT server and port
  client.setServer(mqttServer, mqttPort);

  // Print the MQTT server and port information
  Serial.print("MQTT Server: ");
  Serial.println(mqttServer);
  Serial.print("MQTT Port: ");
  Serial.println(mqttPort);

  // Print the ESP-WROOM-32 IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read water level
  int waterLevel = analogRead(waterLevelPin);
  waterLevel = constrain(waterLevel, minWaterLevel, maxWaterLevel);
  Serial.print("Water Level: ");
  Serial.println(waterLevel);

  // Publish water level data to MQTT topic
  String waterLevelTopic = "sensors/water-level";
  String waterLevelPayload = String(waterLevel);
  client.publish(waterLevelTopic.c_str(), waterLevelPayload.c_str());

  // Read pH value
  float voltage = analogRead(pHProbePin) * (5 / 4095.0);
  float pHValue = map(voltage, 0.0, 5, 0.0, 14.0);
  Serial.print("pH Value: ");
  Serial.println(pHValue);

  // Publish pH value to MQTT topic
  String pHValueTopic = "sensors/pH-value";
  String pHValuePayload = String(pHValue);
  client.publish(pHValueTopic.c_str(), pHValuePayload.c_str());

  delay(5000); // Publish sensor data every 5 seconds
}
