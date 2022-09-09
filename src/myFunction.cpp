#include "settings.h"
#include <math.h>
#include <EEPROM.h>

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

int printScreen(void) {
  int minute, hour, date, month, year;
  minute = Clock.getMinute();
  hour = Clock.getHour(h24, PM);
  date = Clock.getDate();
  month = Clock.getMonth(Century);
  year = Clock.getYear();

  if(!setMODE) {
    lcd.clear();
    lcd.noCursor();
    lcd.noBlink();
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
    // Читаем текущее значение давления, преобразуем его в МПа, и обрезаем до 2х знаков после запятой
    float readP = float(roundf((float(analogRead(pinPRES)) - 90) * 0.145)) / 100;
    lcd.print(readP);
  } 
  else {    
    lcd.cursor();
    lcd.blink();
    lcd.clear();
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
    lcd.print(userP);
    lcd.setCursor(curPOSX, curPOSY);
  }

  return 0;
}

int testKey(int key) {
  if(key) {  // если указали конкретную кнопку, то проверяем ее
    if(!digitalRead(key)) return key;
    return 0;
  }
  // иначе проверяем все возможные
  if(!digitalRead(butPUMP)) return butPUMP;
  if(!digitalRead(butLEFT)) return butLEFT;
  if(!digitalRead(butRIGHT)) return butRIGHT;
  if(!digitalRead(butSET)) return butSET;
  return 0;
}

int functionKey(int key) {
  if(key == butPUMP) { // отработка нажатия кнопки принудительного запуска реле подкачки
    lcd.clear();
    lcd.print("Pump active.");
    digitalWrite(pinRELAY1, HIGH);
    delay(50);
    while(!digitalRead(butPUMP)) {
      delay(retPAUSE);
    }
    quantEVENT++;
    if(quantEVENT > 99) quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
    digitalWrite(pinRELAY1, LOW);
  }

  if(key == butRIGHT || key == butLEFT || key == butSET) { // включает режим правки и устанавливаем таймер выхода из режима
    setMODE = true;
    countSETMODE = setmodeTIME / waitKey;
  }

  if(key == butRIGHT) { // отработка нажатия кнопки перемещения курсора вправо
    curPOSX++;
    if(curPOSX >= lcdCOL) { 
      curPOSX = 0;
      curPOSY++;
    }
    if(curPOSY >= lcdROW) curPOSY = 0;
  }

  if(key == butLEFT) { // отработка нажатия кнопки перемещения курсора влево
    curPOSX--;
    if(curPOSX < 0) {
      curPOSX = lcdCOL - 1;
      curPOSY--;
    }
    if(curPOSY < 0) curPOSY = lcdROW - 1;
  }

  if(key == butSET && setMODE) { // отработка нажатия кнопки правки параметров
    // первым параметром выводится дата (dd.mm.yy), потом время (hh:mm), на второй строке пороговое давление (4 цифры)
    if(curPOSY == 0) {
      if(curPOSX == 0) { // вторая цифра числа (дата)
        int date = Clock.getDate() + 10;
        if(date > 31) date = 0;
        Clock.setDate(date);
      }
      if(curPOSX == 1) { // первая цифра числа (дата)
        int dig = Clock.getDate() + 1;        
        if(dig > 31) dig = 0;
        Clock.setDate(dig);
      }
      if(curPOSX == 3) { // вторая цифра месяца (дата)
        int month = Clock.getMonth(Century) + 10;
        if(month > 12) month = 0;
        Clock.setMonth(month);
      }
      if(curPOSX == 4) { // первая цифра месяца (дата)
        int month = Clock.getMonth(Century) + 1;        
        if(month > 12) month = 0;
        Clock.setMonth(month);
      }
      if(curPOSX == 6) { // вторая цифра года (дата)
        int year = Clock.getYear() + 10;
        if(year > 99) year = 0;
        Clock.setYear(year);
      }
      if(curPOSX == 7) { // первая цифра года (дата)
        int year = Clock.getYear() + 1;        
        if(year > 99) year = 0;
        Clock.setYear(year);
      }
      if(curPOSX == 9) { // вторая цифра часа (время)
        int hour = Clock.getHour(h24, PM) + 10;
        if(hour > 24) hour = 0;
        Clock.setHour(hour);
      }
      if(curPOSX == 10) { // первая цифра часа (время)
        int hour = Clock.getHour(h24, PM) + 1;        
        if(hour > 24) hour = 0;
        Clock.setHour(hour);
      }
      if(curPOSX == 12) { // вторая цифра минут (время)
        int minute = Clock.getMinute() + 10;
        if(minute > 59) minute = 0;
        Clock.setMinute(minute);
      }
      if(curPOSX == 13) { // первая цифра минут (время)
        int minute = Clock.getMinute() + 1;        
        if(minute > 59) minute = 0;
        Clock.setMinute(minute);
      }

    }
    if(curPOSX >= 0 && curPOSX < 4 && curPOSY == 1) {
      if(curPOSX == 0) userP = userP + 1;
      if(curPOSX == 2) userP = userP + 0.1;
      if(curPOSX == 3) userP = userP + 0.01;
      if(userP > 9.99) userP = 0;
    }

  }  
  return 0;
}