// Pins define karein
const int sensorPin = A0;  // Sensor Analog Pin A1
const int ledPin = 13;    // LED Digital Pin 13

// Threshold (Limit) - Isse aap adjust kar sakte hain
int threshold = 100;      

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);     // Serial monitor par values dekhne ke liye
}

void loop() {
  // 1. Sensor se value read karein
  int sensorValue = analogRead(sensorPin);
  
  // 2. Serial monitor par value print karein (testing ke liye)
  Serial.println(sensorValue);

  // 3. Logic: Check karein agar vibration limit se zyada hai
  if (sensorValue > threshold) {
    digitalWrite(ledPin, HIGH); // LED ON
    delay(50);                 // Thodi der jalaye rakhein
  } else {
    digitalWrite(ledPin, LOW);  // LED OFF
  }
}