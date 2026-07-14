const int buttonPin = 2;
const int potPin = A0;

const int leds[] = {13, 12, 11, 10, 9, 8};
const int totalLEDs = 6;

bool systemOn = false;
bool lastButtonState = HIGH;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = 0; i < totalLEDs; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  Serial.begin(9600);
}

void loop()
{
  bool buttonState = digitalRead(buttonPin);

  // Toggle only when button is pressed
  if (lastButtonState == HIGH && buttonState == LOW)
  {
    systemOn = !systemOn;
    delay(200);   // Simple debounce
  }

  lastButtonState = buttonState;

  if (!systemOn)
  {
    // Turn OFF all LEDs
    for (int i = 0; i < totalLEDs; i++)
    {
      digitalWrite(leds[i], LOW);
    }

    return;
  }

  // Read potentiometer
  int value = analogRead(potPin);

  // Convert 0-1023 to 0-6 LEDs
  int level = map(value, 0, 1023, 0, totalLEDs);

  // Turn LEDs ON/OFF
  for (int i = 0; i < totalLEDs; i++)
  {
    if (i < level)
      digitalWrite(leds[i], HIGH);
    else
      digitalWrite(leds[i], LOW);
  }
}