#include "settings.h"
#include <math.h>

/**
 * @brief ����� �� ������� ����� � ��������� ����������� ��������, �.�. �������� ���� ����� 7, � �������� 3, �� ���������� 007
 * 
 * @param value ����� ��� ������
 * @param ndig ���������� �������� ��� �����������
 * @return int ���������� 0, ���� ��� � �������
 */
int printUserDig(int value, int ndig) {
    if(ndig == 4 && value < 1000) lcd.print("0");
    if(ndig == 4 && value < 100) lcd.print("0");
    if(ndig == 4 && value < 10) lcd.print("0");
    if(ndig == 3 && value < 100) lcd.print("0");
    if(ndig == 3 && value < 10) lcd.print("0");
    if(ndig == 2 && value < 10) lcd.print("0");
    lcd.print(value);
    return 0;
}

/**
 * @brief ������� ���������������� ������ ���������� �� �����
 * 
 * @return int ���������� 0 - ���� ��� ��
 */
int printScreen(void) {
  if(!setMODE) {
    int minute, hour, date, month, year;

    minute = Clock.getMinute();
    hour = Clock.getHour(h24, PM);
    date = Clock.getDate();
    month = Clock.getMonth(Century);
    year = Clock.getYear();

    lcd.clear();
    lcd.noCursor();
    printUserDig(date, 2);
    lcd.print(".");
    printUserDig(month, 2);
    lcd.print(".");
    printUserDig(year, 2);
    lcd.print(" ");
    printUserDig(hour, 2);
    lcd.print(":");
    printUserDig(minute, 2); 

    lcd.setCursor(0, 1);
    printUserDig(ldate, 2);
    lcd.print(".");
    printUserDig(lmonth, 2);
    lcd.print(".");
    printUserDig(lyear, 2);
    lcd.print(" ");
    printUserDig(quantEVENT, 2);
    lcd.print(" ");
    printUserDig(analogRead(pinPRES), 4);

  } 
  else {    
    lcd.cursor();
    lcd.blink();
    lcd.clear();
    printUserDig(userP, 4);
    lcd.setCursor(curPOSX, curPOSY);
  }

  return 0;
}