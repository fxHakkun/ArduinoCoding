#include <Arduino.h>

const int sensorPin = 32; // GPIO32 as the analog input pin

void setup() {
  Serial.begin(115200); // Initialize serial communication
}

void loop() {
  // Read analog voltage from the pH sensor
  int sensorValue = analogRead(sensorPin);

  // Print the raw sensor value to the serial monitor
  Serial.print("Raw sensor value: ");
  Serial.println(sensorValue);

  delay(3000); // Delay for stability between measurements
}
