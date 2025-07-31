const int outputPins[] = {2, 5, 4, 18, 19};
const int numOutputs = sizeof(outputPins) / sizeof(outputPins[0]);

const int potPin = 25;
const int buttonPin = 23;

// Button debounce
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int lastButtonReading = HIGH;
int currentButtonReading = HIGH;
bool buttonPressed = false;

// Pattern control
int buttonPushCounter = 0;
int activePattern = 1;

// Potentiometer
int potValue = 0;
int speed = 500;

// Timing
unsigned long lastStepTime = 0;

// Pattern state
int chaseIndex = 0;
int fallingIndex = 0;
bool fallingDirection = true;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potPin, INPUT);

  for (int i = 0; i < numOutputs; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], HIGH); // OFF (active-low)
  }

  Serial.begin(9600);
  activePattern = 1;
}

void loop() {
  potValue = analogRead(potPin);
  speed = map(potValue, 0, 4095, 100, 1000);
  updateButtonState();

  if (buttonPressed) {
    buttonPressed = false;
    buttonPushCounter++;
    activePattern = (buttonPushCounter % 5) + 1;

    // Reset pattern state
    chaseIndex = 0;
    fallingIndex = numOutputs - 1;
    fallingDirection = true;
    lastStepTime = millis();

    allOff(); // Turn everything off between pattern switches
  }

  switch (activePattern) {
    case 1: allOn(); break;
    case 2: singleLetterChase(); break;
    case 3: simpleBlink(); break;
    case 4: lettersFallingLeft(); break;
    case 5: lettersFallingRight(); break;
    default: allOff(); break;
  }
}

void updateButtonState() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentButtonReading) {
      currentButtonReading = reading;
      if (currentButtonReading == LOW) {
        buttonPressed = true;
      }
    }
  }

  lastButtonReading = reading;
}

// --- Patterns ---

void allOn() {
  for (int i = 0; i < numOutputs; i++) {
    digitalWrite(outputPins[i], LOW); // ON (active-low)
  }
}

void allOff() {
  for (int i = 0; i < numOutputs; i++) {
    digitalWrite(outputPins[i], HIGH); // OFF
  }
}

void singleLetterChase() {
  if (millis() - lastStepTime >= speed) {
    lastStepTime = millis();

    allOff();
    digitalWrite(outputPins[chaseIndex], LOW); // ON
    chaseIndex = (chaseIndex + 1) % numOutputs;
  }
}

void simpleBlink() {
  static bool phase = false;
  if (millis() - lastStepTime >= speed) {
    lastStepTime = millis();
    for (int i = 0; i < numOutputs; i++) {
      digitalWrite(outputPins[i], phase ? HIGH : LOW);
    }
    phase = !phase;
  }
}

void lettersFallingLeft() {
  if (millis() - lastStepTime >= speed) {
    lastStepTime = millis();
    if (fallingDirection) {
      digitalWrite(outputPins[fallingIndex], LOW);
    } else {
      digitalWrite(outputPins[fallingIndex], HIGH);
    }
    fallingIndex--;
    if (fallingIndex < 0) {
      fallingDirection = !fallingDirection;
      fallingIndex = numOutputs - 1;
    }
  }
}

void lettersFallingRight() {
  if (millis() - lastStepTime >= speed) {
    lastStepTime = millis();
    if (fallingDirection) {
      digitalWrite(outputPins[fallingIndex], LOW);
    } else {
      digitalWrite(outputPins[fallingIndex], HIGH);
    }
    fallingIndex++;
    if (fallingIndex >= numOutputs) {
      fallingDirection = !fallingDirection;
      fallingIndex = 0;
    }
  }
}
