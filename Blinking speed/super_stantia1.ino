int button = 2;
int red = 11;
int green = 12;
int blue = 13;
int delaytime = 50;
int bright = 255;
void setup()
{
  pinMode(button, INPUT_PULLUP); // Use Arduino's internal pull-up resistor
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  Serial.begin(9600);
  
}

void loop()
{
  
  
    int buttonState = digitalRead(button);
  
  if (buttonState == LOW) { // LOW means button is pressed
    
    digitalWrite(red, HIGH);
    analogWrite(red, bright);
  	delay(delaytime); // Wait for 1000 millisecond(s)
  
    digitalWrite(red, LOW);
  delay(delaytime); // Wait for 1000 millisecond(s)
  
    digitalWrite(green, HIGH);
    analogWrite(green, bright);
  delay(delaytime); // Wait for 1000 millisecond(s)
  
    digitalWrite(green, LOW);
  delay(delaytime); // Wait for 1000 millisecond(s)
    
    digitalWrite(blue, HIGH);
    analogWrite(blue, bright);
  delay(delaytime); // Wait for 1000 millisecond(s)
  
    digitalWrite(blue, LOW);
  delay(delaytime); // Wait for 1000 millisecond(s)
    
    Serial.println("Button Pressed!");
  
  } else {
    
    digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  
    Serial.println("Button Released");
  }

}