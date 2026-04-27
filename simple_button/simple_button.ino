#define KEY_TO_PRESS KEY_F8 // Клавиша для эмуляции
#define BUTTON_PIN 5

#include <Keyboard.h>

// ===== Настройки кнопки =====
byte lastButtonState = LOW;
unsigned long debounce = 50; // задержка подавления дребезга, мс
unsigned long lastChange = 0;

// ===== Настройки эмуляции клавиши =====
const int PRESS_MIN = 45, PRESS_MAX = 120;    // длительность нажатия, мс
const int PAUSE_MIN = 1900, PAUSE_MAX = 2700; // интервал между нажатиями, мс

// ===== Переменные состояния =====
bool active = false;  // флаг: активна ли отправка нажимаемой клавиши
unsigned long t = 0;  // таймер для неблокирующей задержки
bool pressed = false; // состояние: нажата ли сейчас клавиша
int dur = 0;          // текущая длительность нажатия
int pause = 0;        // текущий интервал до следующего нажатия

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // кнопка с подтяжкой к VCC
  Keyboard.begin();                  // инициализация клавиатуры
  randomSeed(analogRead(0));         // генератор случайных чисел
  delay(3000);                       // ожидание инициализации USB
}

void loop() {
  // ===== Обработка кнопки с подавлением дребезга =====
  if (millis() - lastChange > debounce) {
    byte buttonState = digitalRead(BUTTON_PIN);
    
    if (buttonState != lastButtonState) {
      lastChange = millis();
      lastButtonState = buttonState;
      
      // Реагируем на нажатие (LOW из-за INPUT_PULLUP)
      if (buttonState == LOW) {
        active = !active;

        if (!active && pressed) {
          Keyboard.release(KEY_TO_PRESS);
          pressed = false;
        }
      }
    }
  }
  
  // ===== Неблокирующая отправка нажимаемой клавиши =====
  if (active) {
    unsigned long now = millis();
    
    if (pressed) {
      // Клавиша нажата: проверяем, пора ли её отпустить
      if (now - t >= dur) {
        Keyboard.release(KEY_TO_PRESS);
        pressed = false;
        t = now;
        // Генерируем новый интервал до следующего нажатия
        pause = random(PAUSE_MIN, PAUSE_MAX);
      }
    } else {
      // Клавиша отпущена: проверяем, пора ли нажать снова
      if (now - t >= pause) {
        // Генерируем новую длительность нажатия
        dur = random(PRESS_MIN, PRESS_MAX);
        Keyboard.press(KEY_TO_PRESS);
        pressed = true;
        t = now;
      }
    }
  }
}
