#include <WiFi.h>

const char* ssid = "MyUKM";  // Replace with your college Wi-Fi network name (SSID)
const char* loginUrl = " ";  // Replace with the login page URL

const char* studentId = "A162966";  // Replace with your student ID
const char* password = "Huhu121996";    // Replace with your password

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Perform authentication
  if (performAuthentication()) {
    // Authentication successful, proceed with your code here
    Serial.println("Authentication successful");
    // Rest of your setup code...
  } else {
    // Authentication failed, handle the failure case here
    Serial.println("Authentication failed");
  }
}

void loop() {
  // Your code here
}

bool performAuthentication() {
  WiFiClient client;

  // Connect to the login page
  if (client.connect(loginUrl, 80)) {
    Serial.println("Connected to login page");

    // Send the POST request with the form data (student ID and password)
    String postData = "student_id=" + String(studentId) + "&password=" + String(password);
    String request = "POST " + String(loginUrl) + " HTTP/1.1\r\n" +
                     "Host: " + String(loginUrl) + "\r\n" +
                     "Content-Type: application/x-www-form-urlencoded\r\n" +
                     "Content-Length: " + String(postData.length()) + "\r\n\r\n" +
                     postData;

    client.print(request);

    // Wait for the response from the login page
    while (!client.available()) {
      delay(100);
    }

    // Read the response
    String response = "";
    while (client.available()) {
      response += client.readStringUntil('\r');
    }

    // Check if the authentication was successful
    if (response.indexOf("Authentication successful") != -1) {
      return true;
    }
  }

  return false;
}
