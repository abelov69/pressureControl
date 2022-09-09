#include "settings.h"

// Создание и инициализация экрана
LiquidCrystal_I2C lcd(0x27, lcdCOL, lcdROW); // Инициализация библиотеки работы с LCD (адрес на шине I2C 0x27 и размер дисплея)

// Создание, инициализация и настройка часов, создание необходимых переменных
DS3231 Clock; // создание объекта часов

bool Century = false;
bool h24;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte year, month, date, DoW, hour, minute, second;

// Переменные системы
int lyear = 0,
    lmonth = 0,
    ldate = 0;

int curPOSX = 0;
int curPOSY = 0;

bool setMODE = false;

int quantEVENT = 0;
float userP = 0;

int countSETMODE = 0;