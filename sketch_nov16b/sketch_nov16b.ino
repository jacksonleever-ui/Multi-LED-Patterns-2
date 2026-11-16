// 4 LEDs + 2 buttons that cycle through patterns
const int ledPins[] = {5, 6, 9, 10}; // array makes it easy to loop over all LEDs
const int numLeds = 4;
const int btnPrev = 2; // go to previous pattern
const int btnNext = 3; // go to next pattern

int pattern = 0;                     // STATE VARIABLE: 0=off, 1=chase, 2=blink
unsigned long prevMillis = 0;        // remembers when we last changed the LEDs
int step = 0;                        // used by the chase and blink patterns
const unsigned long interval = 200;  // animation speed in milliseconds

// Function prototypes (Arduino IDE adds these automatically, other tools don't)
void allOff();
void runPattern(int p);
void changePattern(int newPattern);

void setup() {
  // Configure every LED pin as output
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  // Buttons use the internal pull-up resistors -> read LOW when pressed
  pinMode(btnPrev, INPUT_PULLUP);
  pinMode(btnNext, INPUT_PULLUP);

  Serial.begin(9600); // always useful for debugging
  Serial.println("Exercise 1 ready");
}

void loop() {
  // ----- 1. Handle buttons (they only change the state) -----
  if (digitalRead(btnNext) == LOW) {     // pressed?
    changePattern((pattern + 1) % 3);    // 0 -> 1 -> 2 -> 0
    delay(250);                          // crude but effective debounce
  }
  if (digitalRead(btnPrev) == LOW) {
    changePattern((pattern + 2) % 3);    // equivalent to -1 in modulo 3
    delay(250);
  }

  // ----- 2. Continuously run whatever the current pattern is -----
  runPattern(pattern);
}

// Helper: switch to a new pattern and reset its animation state
void changePattern(int newPattern) {
  pattern = newPattern;
  step = 0;                // start every pattern from a clean state
  prevMillis = millis();   // restart the timer
  allOff();
  Serial.print("Pattern now: ");
  Serial.println(pattern);
}

// Helper: turn every LED off
void allOff() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// The heart of the program - different behaviour per state
void runPattern(int p) {
  unsigned long now = millis(); // capture current time once

  switch (p) {
    case 0: // ALL OFF
      allOff();
      break;

    case 1: // CHASE (one LED moves left to right)
      if (now - prevMillis >= interval) {
        prevMillis = now;                   // restart the timer
        allOff();
        digitalWrite(ledPins[step], HIGH);  // light only the current LED
        step = (step + 1) % numLeds;        // next position, wrap around
      }
      break;

    case 2: // ALL BLINK together
      if (now - prevMillis >= interval) {
        prevMillis = now;
        step = 1 - step;                    // toggle 0 <-> 1
        for (int i = 0; i < numLeds; i++) {
          digitalWrite(ledPins[i], step ? HIGH : LOW);
        }
      }
      break;
  }
}