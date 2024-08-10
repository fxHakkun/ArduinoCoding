// Water level sensor
const int waterLevelPin = 36; // Pin connected to water level sensor

void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging
}

void loop() {
  // Read water level
  int waterLevel = analogRead(waterLevelPin);

  // Print water level value
  Serial.print("Water Level: ");
  Serial.println(waterLevel);

  delay(3000); // Delay between readings
}
