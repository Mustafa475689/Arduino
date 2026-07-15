const int buttonPin = 8;

const int redLed = 2;
const int greenLed = 3;
const int blueLed = 4;

int currentState = 0;

bool lastButtonState = HIGH;

unsigned long previousMillis = 0;
const int interval = 200;

int animationStep = 0;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  bool buttonState = digitalRead(buttonPin);

  // Detect button press
  if (lastButtonState == HIGH && buttonState == LOW)
  {
    currentState++;

    if (currentState > 5)
    {
      currentState = 0;
    }

    Serial.print("Current State: ");
    Serial.println(currentState);

    // Reset animation whenever we enter another state
    animationStep = 0;

    delay(50); // debounce
  }

  lastButtonState = buttonState;

  // Handle LED states
  switch (currentState)
  {
    case 0: // All OFF
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, LOW);
      break;

    case 1: // Red
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, LOW);
      break;

    case 2: // Green
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      digitalWrite(blueLed, LOW);
      break;

    case 3: // Blue
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, HIGH);
      break;

    case 4: // All ON
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, HIGH);
      digitalWrite(blueLed, HIGH);
      break;

    case 5:
      animation();
      break;
  }
}

// ================= Animation =================

void animation()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    // Turn everything OFF first
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);

    switch (animationStep)
    {
      case 0:
        digitalWrite(redLed, HIGH);
        break;

      case 1:
        digitalWrite(greenLed, HIGH);
        break;

      case 2:
        digitalWrite(blueLed, HIGH);
        break;
    }

    animationStep++;

    if (animationStep > 2)
    {
      animationStep = 0;
    }
  }
}