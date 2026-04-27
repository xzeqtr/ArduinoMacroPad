#include <Keyboard.h>
#include <Mouse.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"

// ===== Универсальный класс управления вводом =====
class AutoInput {
public:
    typedef void (*ActionFunc)(uint8_t);

    AutoInput(uint8_t swPin, uint8_t adjPin, uint8_t actionCode, 
              ActionFunc pressFn, ActionFunc releaseFn)
      : _swPin(swPin), _adjPin(adjPin), _code(actionCode),
        _pressFn(pressFn), _releaseFn(releaseFn) {
        _lastSwState = HIGH;
        _lastAdjState = HIGH;
    }

    void begin() {
        pinMode(_swPin, INPUT_PULLUP);
        pinMode(_adjPin, INPUT_PULLUP);
    }

    void update() {
        handleTrigger();
        handleAdjust();
        handleAutoPress();
    }

    bool isActive() const { return _active; }
    long getExtraDelay() const { return _extraDelay; }

private:
    uint8_t _swPin, _adjPin, _code;
    ActionFunc _pressFn, _releaseFn;

    bool _active = false;
    bool _isPressed = false;
    long _extraDelay = 0;

    unsigned long _lastSwChange = 0;
    byte _lastSwState;
    unsigned long _lastAdjChange = 0;
    byte _lastAdjState;
    unsigned long _adjPressTime = 0;
    bool _adjPressed = false;

    unsigned long _timer = 0;
    unsigned long _currentPressDuration = 0;
    unsigned long _currentInterval = 0;

    void handleTrigger() {
        if (millis() - _lastSwChange > DEBOUNCE_MS) {
            byte state = digitalRead(_swPin);
            if (state != _lastSwState) {
                _lastSwChange = millis();
                _lastSwState = state;
                if (state == LOW) {
                    _active = !_active;
                    if (_active) {
                        _currentPressDuration = random(MIN_PRESS_DURATION, MAX_PRESS_DURATION);
                        _currentInterval = random(MIN_INTERVAL, MAX_INTERVAL);
                        _pressFn(_code);
                        _isPressed = true;
                        _timer = millis();
                    } else {
                        if (_isPressed) {
                            _releaseFn(_code);
                            _isPressed = false;
                        }
                    }
                }
            }
        }
    }

    void handleAdjust() {
        byte state = digitalRead(_adjPin);
        if (state != _lastAdjState) {
            delay(20); // Как в оригинале
            state = digitalRead(_adjPin);
            if (state != _lastAdjState) {
                _lastAdjState = state;
                if (state == LOW) {
                    _adjPressTime = millis();
                    _adjPressed = true;
                } else {
                    if (_adjPressed) {
                        unsigned long dur = millis() - _adjPressTime;
                        if (dur >= LONG_PRESS_THRESHOLD) _extraDelay -= DELAY_STEP_LONG;
                        else _extraDelay += DELAY_STEP_SHORT;
                        
                        if (_extraDelay < 0) _extraDelay = 0;
                        _adjPressed = false;
                    }
                }
                _lastAdjChange = millis();
            }
        }
    }

    void handleAutoPress() {
        if (!_active) return;
        unsigned long now = millis();
        if (_isPressed) {
            if (now - _timer >= _currentPressDuration) {
                _releaseFn(_code);
                _isPressed = false;
                _timer = now;
                _currentInterval = random(MIN_INTERVAL, MAX_INTERVAL);
            }
        } else {
            if (now - _timer >= (unsigned long)(_currentInterval + _extraDelay)) {
                _currentPressDuration = random(MIN_PRESS_DURATION, MAX_PRESS_DURATION);
                _pressFn(_code);
                _isPressed = true;
                _timer = now;
            }
        }
    }
};

// ===== Обёртки для статических методов библиотек =====
void keyPress(uint8_t c)   { Keyboard.press(c); }
void keyRelease(uint8_t c) { Keyboard.release(c); }
void mousePress(uint8_t c) { Mouse.press(c); }
void mouseRelease(uint8_t c) { Mouse.release(c); }

// ===== Создание экземпляров =====
AutoInput keyboard(PIN_KEYB_SW, PIN_KEYB_ADJ, KEY_TO_PRESS, keyPress, keyRelease);
AutoInput mouse(PIN_MOUSE_SW, PIN_MOUSE_ADJ, MOUSE_TO_PRESS, mousePress, mouseRelease);

// ===== Дисплей =====
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
unsigned long lastDisplayUpdate = 0;

void setup() {
    keyboard.begin();
    mouse.begin();
    Keyboard.begin();
    Mouse.begin();
    randomSeed(analogRead(0));
    delay(3000);
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tr);
}

void loop() {
    keyboard.update();
    mouse.update();

    if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_MS) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }
}

void updateDisplay() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    
    u8g2.drawStr(0, 10, keyboard.isActive() ? "Keyb  ON" : "Keyb  OFF");
    u8g2.setCursor(0, 24);
    u8g2.print("Delay "); u8g2.print(keyboard.getExtraDelay() / 1000); u8g2.print("s");
    
    u8g2.drawStr(65, 10, mouse.isActive() ? "Mouse ON" : "Mouse OFF");
    u8g2.setCursor(65, 24);
    u8g2.print("Delay "); u8g2.print(mouse.getExtraDelay() / 1000); u8g2.print("s");
    
    u8g2.sendBuffer();
}
