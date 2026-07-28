#include <Servo.h>

// Pins define karein
const int trigPin = 2;
const int echoPin = 3;
const int servoPin = 9;

Servo gateServo;

// Variables
long duration;
int distance;
const int maxDistance = 100; // 2 meters = 200 cm

void setup() {
  pinMode(trigPin, OUTPUT); // Trig pin output hai
  pinMode(echoPin, INPUT);  // Echo pin input hai
  
  gateServo.attach(servoPin);
  gateServo.write(0);       // Shuru mein gate close (0 degrees) rahega
  
  Serial.begin(9600);       // Testing ke liye Serial Monitor
}

void loop() {
  // Ultrasonic sensor se pulse bhejna
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Pulse wapas aane ka time measure karna
  duration = pulseIn(echoPin, HIGH);
  
  // Distance ko centimeters mein calculate karna
  distance = duration * 0.034 / 2;
  
  // Serial Monitor par distance check karne ke liye
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Agar distance 0 se bari hai (error na ho) aur 200cm (2m) se kam hai
  if (distance > 0 && distance <= maxDistance) {
    gateServo.write(90);  // Gate open (90 degrees)
    delay(500);           // Thoda wait takay baar baar jhatkay na lagay
  } 
  else {
    gateServo.write(0);   // Gate close (0 degrees)
    delay(500);
  }
  
  delay(100); // Har 0.1 second baad check karega
}
