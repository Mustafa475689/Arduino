const int trigPin = 12;
const int echoPin = 13;
const int ledPin = 8;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  // Read echo
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance
  float distance = duration * 0.0343 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // LED control
  if (distance > 0 && distance <= 20) {
    digitalWrite(ledPin, HIGH);   // LED ON
  } else {
    digitalWrite(ledPin, LOW);    // LED OFF
  }

  delay(200);
}