#pragma once

#include <LiquidCrystal.h>
#include "DS3231.h"

// ��������� �������
#define lowP 0.11 // ��������� �������� ��������, ��� ������� ����������� ���� �������� ���� � �������
#define pinPRES A0 // �������, �� ������� �������� ������ �� ������� ��������
#define pinRELAY1 A2 // ������� ���������� ���� 1
#define pinRELAY2 A3 // ������� ���������� ���� 2
#define adressP_EEPROM 10 // ����� � EEPROM �������� ���������� ��������, �������������� �������������
#define adressQ_EEPROM 14 // ����� � EEPROM ���������� ������������ �� ������� ������� ��������

// ��������� LCD ������
#define lcdRS 2 // �������, �� ������� ��������� ����� RS ������
#define lcdRW 3 // �������, �� ������� ��������� ����� RW ������
#define lcdEN 4 // �������, �� ������� ��������� ����� ENABLE ������
#define lcdD4 5 // �������, �� ������� ��������� ����� D4 ������
#define lcdD5 6 // �������, �� ������� ��������� ����� D5 ������
#define lcdD6 7 // �������, �� ������� ��������� ����� D6 ������
#define lcdD7 8 // �������, �� ������� ��������� ����� D7 ������
#define lcdCOL 16 // ���������� ������ � ������ ������ (�������� ������)
#define lcdROW 2  // ���������� ����� �� ������ (�������� ������)
#define lcdREFR 1000 // ����� � �� ��� ���������� ���������� �� ������

// ��������� ������
#define butPUMP 9 // ������ ���������� �������������� ��������
#define butLEFT 10 // ������ ����������� �������� ��� ��������� ���������
#define butRIGHT 11 // ������ ������������� �������� ��� ��������� ���������
#define butSET 12 // ������ �������� �� ��������� �������� ��� ���������
#define waitKey 5 // ����� � �� ����� �������� ��������� ������
#define retPAUSE 100 // ����� � �� ��� ���������� �������� ��� ������� ������

// �������� � ������������� ������
extern LiquidCrystal lcd; // ������������� ���������� ������ � LCD


// ��������, ������������� � ��������� �����, �������� ����������� ����������
extern DS3231 Clock; // �������� ������� �����

extern bool Century;
extern bool h24;
extern bool PM;
extern byte ADay, AHour, AMinute, ASecond, ABits;
extern bool ADy, A12h, Apm;

extern byte year, month, date, DoW, hour, minute, second;

// ���������� �������
extern int lyear,  // ���, � ������� ��������� ��������� ������������ �� ������� ��������
           lmonth, // ����� -//-
           ldate;  // ���� -//-

extern int quantEVENT; // ���������� ������� ��������� �������� � ������� ��������� �������
extern float userP; // ��������� �������� �������� ������������� �������������

extern int curPOSX; // ������� ������� ������ �� ��� X (�������)
extern int curPOSY; // ������� ������� ������ �� ��� Y (������)

/* � ���������� ������ �� ������ ������������ ������� ����, �����, ���� ���������� �������, ���������� ������� � ������� ��������.
������ �� ������������. ��� ������� ������ ����������� ����� ��� ������, ���������� ����� ������ ����������. �� ������ ������������
��������� �������� � ������ ������. �� ��������� ������� setmodeTIME, ����� ������ ���������� �����������. */
extern bool setMODE; // ����� ��������� ����������
extern int countSETMODE; // ������ ������ �� ������ ������
#define setmodeTIME 10000 // ����� � �� �� ������ �� ������ ������ ����������