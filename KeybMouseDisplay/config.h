#ifndef CONFIG_H
#define CONFIG_H

// ===== Пины управления =====
#define PIN_KEYB_SW   5 // Вкл/Выкл клавиатуры
#define PIN_KEYB_ADJ  6 // Настройка задержки клавиатуры
#define PIN_MOUSE_ADJ 7 // Настройка задержки мыши
#define PIN_MOUSE_SW  8 // Вкл/Выкл мыши

// ===== Целевые действия =====
const uint8_t KEY_TO_PRESS   = KEY_F8;     // KEY_F1..F12, 'a', 'b' и т.д.
const uint8_t MOUSE_TO_PRESS = MOUSE_LEFT; // MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE

// ===== Тайминги и задержки (мс) =====
const unsigned long DEBOUNCE_MS          = 50;   // Антидребезг кнопок
const unsigned long MIN_PRESS_DURATION   = 45;   // Мин. время удержания нажатия
const unsigned long MAX_PRESS_DURATION   = 120;  // Макс. время удержания нажатия
const unsigned long MIN_INTERVAL         = 800;  // Мин. интервал между нажатиями
const unsigned long MAX_INTERVAL         = 1200; // Макс. интервал между нажатиями
const unsigned long LONG_PRESS_THRESHOLD = 800;  // Порог долгого нажатия для настройки задержки

// ===== Дисплей =====
const unsigned long DISPLAY_UPDATE_MS = 100; // Частота обновления экрана

// ===== Шаг изменения задержки =====
const long DELAY_STEP_SHORT = 1000;  // +1 сек при коротком нажатии
const long DELAY_STEP_LONG  = 15000; // -15 сек при долгом нажатии

#endif // CONFIG_H
