#include <Stepper.h>

// Define steps per revolution for a standard 28BYJ-48 motor
const int stepsPerRevolution = 2048; 

// Initialize the library. 
// Note the pin order (8, 10, 9, 11) is required for this specific motor to sequence right.
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  // Set the speed in RPM (Revolutions Per Minute)
  myStepper.setSpeed(10); 

}

void loop() {
  // Rotate one full turn clockwise
  myStepper.step(stepsPerRevolution);
  delay(1000); // Wait 1 second
  
  // Rotate one full turn counter-clockwise
  myStepper.step(-stepsPerRevolution);
  delay(1000); // Wait 1 second
}
