const int pumpPin1 = 32;  // GPIO32 (Pin 32) connected to the pump
const int pumpPin2 = 33;  // GPIO32 (Pin 33) connected to the pump

void setup() {
}

void loop() {
  // Turn on the pump
   pinMode(pumpPin1, OUTPUT);  // Set the pump pin as an output
  digitalWrite(pumpPin1, HIGH);
  pinMode(pumpPin2, OUTPUT);  // Set the pump pin as an output
  digitalWrite(pumpPin2, HIGH);
  delay(5000);  // Run the pump for 5 seconds

  // Turn off the pump
  pinMode(pumpPin1, OUTPUT);  // Set the pump pin as an output
  digitalWrite(pumpPin1, LOW);
  pinMode(pumpPin2, OUTPUT);  // Set the pump pin as an output
  digitalWrite(pumpPin2, LOW);
  delay(1000);  // Run the pump for 5 seconds
}
