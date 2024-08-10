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
const char* nodeId = "0062501010103";
const char* postUrl = "http://thingssentral.io/postlong";
const char* readUrl = "http://thingssentral.io/ReadNode?Params=tokenid|xxx@NodeId|";

const int waterLevelPin = 33;
const int pHProbePin = 32;
const int nutrientPumpPin = 26;
const int waterPumpPin = 25;
const float pHThreshold = 1.0;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWiFi();

  // Configure peristaltic motor pin as output
  pinMode(nutrientPumpPin, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);
}

void loop() {
  static unsigned long previousTime = 0;
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - previousTime;

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to: ");
    Serial.println(WiFi.SSID());

    // Read water level
    int rawWaterLevel = analogRead(waterLevelPin);

    // Convert water level above 100 to 200
    int waterLevel = rawWaterLevel > 100 ? 200 : rawWaterLevel;

    Serial.print("Water Level: ");
    if (waterLevel == 0) {
      Serial.println("Refilling in process");
      digitalWrite(waterPumpPin, HIGH); // Start water pump
    } else {
      Serial.println("Water level in good condition");
      digitalWrite(waterPumpPin, LOW); // Stop water pump
    }

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

    // Read target pH value from the cloud
    float targetPHValue = readTargetPHValue();

    Serial.print("Target pH Value: ");
    Serial.println(targetPHValue);

    // Adjust peristaltic motor based on pH value
    if (pHValue > targetPHValue + pHThreshold) {
      // pH value is higher than target + threshold, reduce nutrient
      digitalWrite(nutrientPumpPin, LOW);
    } else if (pHValue < targetPHValue - pHThreshold) {
      // pH value is lower than target - threshold, increase nutrient
      digitalWrite(nutrientPumpPin, HIGH);
    } else {
      // pH value is within the acceptable range, stop nutrient adjustment
      digitalWrite(nutrientPumpPin, LOW); 
    }

    // Check if 5 minutes have elapsed since the last data sending
    if (elapsedTime >= 300000) {
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
      http.POST("");

      previousTime = currentTime; // Update the previous time for the next 5-minute interval

      http.end();

    }
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

float readTargetPHValue() {
  String readLink = String(readUrl) + String(nodeId);

  HTTPClient http;
  http.begin(client, readLink);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();
    int separatorIndex = response.indexOf('|');
    if (separatorIndex != -1) {
      String targetPHValueString = response.substring(separatorIndex + 1, response.lastIndexOf('|'));
      return targetPHValueString.toFloat();
    }
  }

  return 0.0; // Default value if reading fails
}

