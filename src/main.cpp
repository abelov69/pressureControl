/**
 * @file main.cpp
 * @author Andrey Belov (d_mak@rambler.ru)
 * @brief Программа контроля давления в системе отопления
 * @details Программа читает показания с датчика давления в системе отопления. При низком уровне давления lowP
 * включает реле, управляющее электромагнитным клапаном или насосом подкачки. Устройство имеет LCD экран, на который
 * выводит текущее дату и время (читает с модуля DS3231 с резервной батереей), дату последнего срабатывания по низкому
 * давлению, количество срабатываний с начала работы устройства, текущее давление в системе. Также устройство
 * имеет несколько кнопок (принудительное включение насоса/клапана, настройка параметров даты и времени,
 * порогового давления).
 * Модуль часов работает по шине I2C (задействует ноги A4 - SDA, A5 - SCL)
 * @version 0.1
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "DS3231.h"
#include "settings.h"
#include "myFunction.h"

void setup() {
  // put your setup code here, to run once:
  lcd.begin(lcdCOL, lcdROW);

  // Start the I2C interface
	Wire.begin();
 
  // Настройка выводов под кнопки и реле
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);

  // Чтение из EEPROM пользовательских данных
  EEPROM.get(adressP_EEPROM, userP); // читаем значение порогового давления
  if(userP > 1023 || userP < 0) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // читаем количество событий срабатывания по низкому давлению
  if(quantEVENT > 99 || quantEVENT < 0) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  keyPRESS = 0; // сбрасываем номер нажатой кнопки
  int refrCount = 0; // счетчик обновления информации на дисплее 

  while(!keyPRESS) {
    printScreen(); // выводим информацию на экран
    refrCount = lcdREFR / retPAUSE;
    while(refrCount) {
      if(!digitalRead(butPUMP)) keyPRESS = butPUMP; // проверяем нажатие кнопок
      if(!digitalRead(butLEFT)) keyPRESS = butLEFT;
      if(!digitalRead(butRIGHT)) keyPRESS = butRIGHT;
      if(!digitalRead(butSET)) keyPRESS = butSET;

      if(keyPRESS) { // делаем паузу и проверяем нажатие для устранения дребезга
        delay(retPAUSE);
        if(!digitalRead(keyPRESS)) break;
        keyPRESS = 0;
      }
      
      if(!countSETMODE) { // по завершению правки необходимо записать значение в память и выключить режим правки
        setMODE = false;
        EEPROM.put(adressP_EEPROM, userP);
        delay(5);
      }
      else countSETMODE--;
      delay(retPAUSE);
      refrCount--;
    }
    
  }

  if(keyPRESS == butPUMP) { // отработка нажатия кнопки принудительного запуска реле подкачки
    lcd.clear();
    lcd.print("Pump active.");
    digitalWrite(pinRELAY1, HIGH);
    while(!digitalRead(butPUMP)) {
      delay(retPAUSE);
    }
    quantEVENT++;
    if(quantEVENT > 99) quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
    digitalWrite(pinRELAY1, LOW);
  }

  if(keyPRESS == butRIGHT) { // отработка нажатия кнопки перемещения курсора вправо
    curPOSX++;
    if(curPOSX >= lcdCOL) { 
      curPOSX = 0;
      curPOSY++;
    }
    if(curPOSY >= lcdROW) curPOSY = 0;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butLEFT) { // отработка нажатия кнопки перемещения курсора влево
    curPOSX--;
    if(curPOSX < 0) {
      curPOSX = lcdCOL - 1;
      curPOSY--;
    }
    if(curPOSY < 0) curPOSY = lcdROW - 1;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butSET && setMODE) { // отработка нажатия кнопки правки параметров
    // первым параметром выводится дата (dd.mm.yy), потом время (hh:mm), на второй строке пороговое давление (4 цифры)
    if(curPOSY == 0) {
      if(curPOSX == 0) { // вторая цифра числа (дата)
        int fdig = Clock.getDate();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 3) sdig = 0;
        if(sdig == 3 && fdig > 1) fdig = 1;
        Clock.setDate(sdig * 10 + fdig);
      }
      if(curPOSX == 1) { // первая цифра числа (дата)
        int dig = Clock.getDate() + 1;        
        if(dig > 31) dig = 0;
        Clock.setDate(dig);
      }
      if(curPOSX == 3) { // вторая цифра месяца (дата)
        int fdig = Clock.getMonth(Century);
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 1) sdig = 0;
        if(sdig == 1 && fdig > 2) fdig = 2;
        Clock.setMonth(sdig * 10 + fdig);
      }
      if(curPOSX == 4) { // первая цифра месяца (дата)
        int dig = Clock.getMonth(Century) + 1;        
        if(dig > 12) dig = 0;
        Clock.setMonth(dig);
      }
      if(curPOSX == 6) { // вторая цифра года (дата)
        int fdig = Clock.getYear();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 9) sdig = 0;
        Clock.setYear(sdig * 10 + fdig);
      }
      if(curPOSX == 7) { // первая цифра года (дата)
        int dig = Clock.getYear() + 1;        
        if(dig > 99) dig = 0;
        Clock.setYear(dig);
      }
      if(curPOSX == 9) { // вторая цифра часа (время)
        int fdig = Clock.getHour(h24, PM);
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 2) sdig = 0;
        if(sdig == 2 && fdig > 4) fdig = 4;
        Clock.setHour(sdig * 10 + fdig);
      }
      if(curPOSX == 10) { // первая цифра часа (время)
        int dig = Clock.getHour(h24, PM) + 1;        
        if(dig > 24) dig = 0;
        Clock.setHour(dig);
      }
      if(curPOSX == 12) { // вторая цифра минут (время)
        int fdig = Clock.getMinute();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 5) sdig = 0;        
        Clock.setMinute(sdig * 10 + fdig);
      }
      if(curPOSX == 13) { // первая цифра минут (время)
        int dig = Clock.getMinute() + 1;        
        if(dig > 59) dig = 0;
        Clock.setMinute(dig);
      }

    }
    if(curPOSX >= 0 && curPOSX < 4 && curPOSY == 1) {
      int mod = 1;
      if(curPOSX == 0) mod = 1000;
      if(curPOSX == 1) mod = 100;
      if(curPOSX == 2) mod = 10;
      if(curPOSX == 3) mod = 1;
      int dig = userP / mod;
      dig = dig % 10 + 1;
      if(dig > 9) dig = 0;
      userP = userP / (mod * 10) * (mod * 10) + dig * mod + userP % mod;
    }

  }

  delay(300);

}