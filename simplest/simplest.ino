#define KEY_TO_PRESS KEY_F8  // KEY_F8, KEY_ENTER, 'a'...

#include <Keyboard.h>

// Timing settings
const int PRESS_MIN = 45, PRESS_MAX = 120;
const int PAUSE_MIN = 1900, PAUSE_MAX = 2700;

bool pressed = false;
unsigned long t = 0;
int dur = 0, pause = 0;

void setup() {
  Keyboard.begin();
  randomSeed(analogRead(0));
  delay(3000);
  nextPress();
}

void loop() {
  unsigned long now = millis();
  if (pressed && now - t >= dur) {
    Keyboard.release(KEY_TO_PRESS);
    pressed = false;
    t = now;
    pause = random(PAUSE_MIN, PAUSE_MAX);
  }
  else if (!pressed && now - t >= pause) {
    nextPress();
  }
}

void nextPress() {
  dur = random(PRESS_MIN, PRESS_MAX);
  Keyboard.press(KEY_TO_PRESS);
  pressed = true;
  t = millis();
}
