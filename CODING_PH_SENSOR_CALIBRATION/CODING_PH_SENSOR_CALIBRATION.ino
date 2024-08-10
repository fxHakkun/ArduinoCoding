int pH_Value;
float Voltage;

void setup() {
  Serial.begin(9600);
  pinMode(32, INPUT); // Set pin 32 (A0) as input
}

void loop() {
  pH_Value = analogRead(32); // Read analog value from pin 32 (A0)
  Voltage = pH_Value * (3.3 / 4095.0); // Assuming 3.3V reference voltage, adjust if necessary
  Serial.println(Voltage);
  delay(1000);
}
