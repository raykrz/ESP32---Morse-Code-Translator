// Morse code translator.
// Tap Morse code on the board's Boot button. The red LED lights while the
// button is held, and the serial monitor shows the dots, dashes and decoded message.
//
// Wiring:
//   Button: none needed. The Boot button is wired to GPIO 0 inside the board.
//   LED:    GPIO 23 -> 200 ohm resistor -> LED long leg; LED short leg -> GND.
#include <Arduino.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Settings you can change
// ---------------------------------------------------------------------------
const int BUTTON_PIN = 0;             // The board's Boot button. Use 19 for an external button.
const int LED_PIN = 23;

const uint32_t DASH_MIN_MS = 300;     // A press this long or longer is a dash.
const uint32_t LETTER_GAP_MS = 800;   // A pause this long ends a letter.
const uint32_t WORD_GAP_MS = 2000;    // A pause this long ends a word.
const uint32_t DEBOUNCE_MS = 20;      // Ignore contact bounce shorter than this.

// ---------------------------------------------------------------------------
// Morse table: MORSE_CODES[i] is the code for MORSE_LETTERS[i].
// ---------------------------------------------------------------------------
const int MORSE_COUNT = 36;
const char MORSE_LETTERS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char* const MORSE_CODES[MORSE_COUNT] = {
  ".-",    "-...",  "-.-.",  "-..",   ".",     "..-.",  "--.",   "....",  "..",
  ".---",  "-.-",   ".-..",  "--",    "-.",    "---",   ".--.",  "--.-",  ".-.",
  "...",   "-",     "..-",   "...-",  ".--",   "-..-",  "-.--",  "--..",
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.",
};

// Returns the letter for a code such as ".-", or '?' if it isn't in the table.
char decodeMorse(const char code[]) {
  for (int i = 0; i < MORSE_COUNT; i++) {
    if (strcmp(code, MORSE_CODES[i]) == 0) return MORSE_LETTERS[i];
  }
  return '?';
}

// ---------------------------------------------------------------------------
// Button reading with debouncing
// ---------------------------------------------------------------------------
bool stablePressed = false;  // What the program believes the button is doing.
bool lastReading = false;    // The raw reading from the previous loop.
uint32_t lastChangeMs = 0;   // When the raw reading last changed.

// Returns true only when the button has just been pressed or released.
bool updateButton(uint32_t now) {
  bool reading = digitalRead(BUTTON_PIN) == LOW;  // LOW means pressed (pull-up).
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeMs = now;
  }
  if (reading != stablePressed && now - lastChangeMs >= DEBOUNCE_MS) {
    stablePressed = reading;
    return true;
  }
  return false;
}

// ---------------------------------------------------------------------------
// Translator state
// ---------------------------------------------------------------------------
const int MAX_SYMBOLS = 6;
char symbols[MAX_SYMBOLS + 1] = "";  // Dots and dashes of the letter being typed.
char message[81] = "";               // The decoded message, up to 80 characters.
uint32_t pressStartMs = 0;
uint32_t releaseMs = 0;
bool wordEnded = true;               // Stops the message starting with a space.

void addToString(char text[], int maxLength, char c) {
  int length = strlen(text);
  if (length < maxLength) {
    text[length] = c;
    text[length + 1] = '\0';
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  delay(500);
  Serial.println();
  Serial.println("Morse translator ready.");
  Serial.println("Short press = dot, long press = dash.");
  Serial.println("Pause about 1 second between letters, 2 seconds between words.");
}

void loop() {
  uint32_t now = millis();

  // 1. The button was just pressed or released.
  if (updateButton(now)) {
    digitalWrite(LED_PIN, stablePressed ? HIGH : LOW);
    if (stablePressed) {
      pressStartMs = now;
    } else {
      releaseMs = now;
      char symbol = (now - pressStartMs >= DASH_MIN_MS) ? '-' : '.';
      if ((int)strlen(symbols) < MAX_SYMBOLS) {
        addToString(symbols, MAX_SYMBOLS, symbol);
        Serial.printf("  %s\n", symbols);
      } else {
        Serial.println("  Too many dots and dashes for one letter, starting again.");
        symbols[0] = '\0';
      }
    }
    return;
  }

  // 2. The button is held: nothing to decide yet.
  if (stablePressed) return;

  // 3. The button is up: check how long the pause has lasted.
  uint32_t pause = now - releaseMs;

  if (pause >= LETTER_GAP_MS && symbols[0] != '\0') {
    char letter = decodeMorse(symbols);
    addToString(message, 80, letter);
    Serial.printf("%s = %c   Message: %s\n", symbols, letter, message);
    symbols[0] = '\0';
    wordEnded = false;
  }

  if (pause >= WORD_GAP_MS && !wordEnded) {
    addToString(message, 80, ' ');
    wordEnded = true;
    Serial.printf("Message: %s\n", message);
  }
}
