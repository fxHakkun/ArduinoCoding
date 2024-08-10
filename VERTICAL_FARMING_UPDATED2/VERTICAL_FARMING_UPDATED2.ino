#include <WiFi.h>
#include <HTTPClient.h>

// Define the maximum number of Wi-Fi networks
#define MAX_NETWORKS 2

// Structure to store Wi-Fi credentials
struct WiFiCredential {
  const char* ssid;
  const char* password;
};

// Array of Wi-Fi credentials
WiFiCredential wifiCredentials[MAX_NETWORKS] = {
  {"Redmi Note 7", "12345678"},
  {"MyUKM", ""}
};

const char* userId = "00625";
const char* pHSensorId = "0062501010101";
const char* waterLevelSensorId = "0062501010102";
const char* postUrl = "http://thingssentral.io/postlong";

const int waterLevelPin = 33;
const int maxWaterLevel = 4095;
const int minWaterLevel = 0;
const int pHProbePin = 32;

const int peristalticMotorPin = 26;
const float targetPHValue = 9.0;
const float pHThreshold = 1.0;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWiFi();

  // Configure peristaltic motor pin as output
  pinMode(peristalticMotorPin, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to: ");
    Serial.println(WiFi.SSID());

    // Read water level
    int waterLevel = analogRead(waterLevelPin);
    waterLevel = constrain(waterLevel, minWaterLevel, maxWaterLevel);
    Serial.print("Water Level: ");
    Serial.println(waterLevel);

    // Read pH value
    int avgValue = 0;
    for (int i = 0; i < 10; i++) {
      avgValue += analogRead(pHProbePin);
      delay(10);
    }
    avgValue /= 10;

    float voltage = avgValue * (3.3 / 4095.0);
    float pHValue = 3.5 * voltage;

    Serial.print("pH Value: ");
    Serial.println(pHValue);

    // Adjust peristaltic motor based on pH value
    if (pHValue > targetPHValue + pHThreshold) {
      // pH value is higher than target + threshold, reduce nutrient
      digitalWrite(peristalticMotorPin, HIGH);
    } else if (pHValue < targetPHValue - pHThreshold) {
      // pH value is lower than target - threshold, increase nutrient
      digitalWrite(peristalticMotorPin, LOW);
    } else {
      // pH value is within the acceptable range, stop nutrient adjustment
      digitalWrite(peristalticMotorPin, HIGH); // or LOW, depending on your relay setup
    }

    // Construct the HTTP POST request
    String data = "userid|" + String(userId) + "@" +
                  waterLevelSensorId + "|" + String(waterLevel) + "@" +
                  pHSensorId + "|" + String(pHValue);

    // Print the simulated HTTP POST URL
    String url = String(postUrl) + "?data=" + data;
    Serial.print("Simulated HTTP POST URL: ");
    Serial.println(url);

    HTTPClient http;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send the POST request with an empty payload
    int httpResponseCode = http.POST("");
    if (httpResponseCode >= 200 && httpResponseCode < 300) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("HTTP POST request failed. Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    delay(300000); // Publish sensor data every 5 minutes
  } else {
    // Attempt to reconnect to Wi-Fi if disconnected
    connectToWiFi();
  }
}

void connectToWiFi() {
  int numNetworks = sizeof(wifiCredentials) / sizeof(wifiCredentials[0]);

  for (int i = 0; i < numNetworks; i++) {
    Serial.printf("Connecting to Wi-Fi network: %s\n", wifiCredentials[i].ssid);
    WiFi.begin(wifiCredentials[i].ssid, wifiCredentials[i].password);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 5) {
      delay(1000);
      Serial.print(".");
      attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to Wi-Fi");
      break;
    } else {
      Serial.println("\nFailed to connect to Wi-Fi");
    }
  }
}
