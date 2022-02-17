#include "settings.h"

// �������� � ������������� ������
LiquidCrystal lcd(lcdRS, lcdRW, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7); // ������������� ���������� ������ � LCD


// ��������, ������������� � ��������� �����, �������� ����������� ����������
DS3231 Clock; // �������� ������� �����

bool Century = false;
bool h24;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte year, month, date, DoW, hour, minute, second;

// ���������� �������
int lyear = 0,
    lmonth = 0,
    ldate = 0;

int curPOSX = 0;
int curPOSY = 0;

bool setMODE = false;

int quantEVENT = 0;
float userP = 0;

int countSETMODE = 0;