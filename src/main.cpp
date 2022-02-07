/**
 * @file main.cpp
 * @author Andrey Belov (d_mak@rambler.ru)
 * @brief ѕрограмма контрол€ давлени€ в системе отоплени€
 * @details ѕрограмма читает показани€ с датчика давлени€ в системе отоплени€. ѕри низком уровне давлени€ lowP
 * включает реле, управл€ющее электромагнитным клапаном или насосом подкачки. ”стройство имеет LCD экран, на который
 * выводит текущее дату и врем€ (читает с модул€ DS3231 с резервной батереей), дату последнего срабатывани€ по низкому
 * давлению, количество срабатываний с начала работы устройства, текущее давление в системе. “акже устройство
 * имеет несколько кнопок (принудительное включение насоса/клапана, настройка параметров даты и времени,
 * порогового давлени€).
 * ћодуль часов работает по шине I2C (задействует ноги A4 - SDA, A5 - SCL)
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
 
  // Ќастройка выводов под кнопки и реле
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);

  // „тение из EEPROM пользовательских данных
  EEPROM.get(adressP_EEPROM, userP); // читаем значение порогового давлени€
  if(userP > 1023 || userP < 0) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // читаем количество событий срабатывани€ по низкому давлению
  if(quantEVENT > 99 || quantEVENT < 0) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  keyPRESS = 0; // сбрасываем номер нажатой кнопки
  int refrCount = 0; // счетчик обновлени€ информации на дисплее 

  while(!keyPRESS) {
    printScreen(); // выводим информацию на экран
    refrCount = lcdREFR / retPAUSE;
    while(refrCount) {
      if(!digitalRead(butPUMP)) keyPRESS = butPUMP; // провер€ем нажатие кнопок
      if(!digitalRead(butLEFT)) keyPRESS = butLEFT;
      if(!digitalRead(butRIGHT)) keyPRESS = butRIGHT;
      if(!digitalRead(butSET)) keyPRESS = butSET;

      if(keyPRESS) { // делаем паузу и провер€ем нажатие дл€ устранени€ дребезга
        delay(retPAUSE);
        if(!digitalRead(keyPRESS)) break;
        keyPRESS = 0;
      }
      
      if(!countSETMODE) { // по завершению правки необходимо записать значение в пам€ть и выключить режим правки
        setMODE = false;
        EEPROM.put(adressP_EEPROM, userP);
        delay(5);
      }
      else countSETMODE--;
      delay(retPAUSE);
      refrCount--;
    }
    
  }

  if(keyPRESS == butPUMP) { // отработка нажати€ кнопки принудительного запуска реле подкачки
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

  if(keyPRESS == butRIGHT) { // отработка нажати€ кнопки перемещени€ курсора вправо
    curPOSX++;
    if(curPOSX >= lcdCOL) { 
      curPOSX = 0;
      curPOSY++;
    }
    if(curPOSY >= lcdROW) curPOSY = 0;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butLEFT) { // отработка нажати€ кнопки перемещени€ курсора влево
    curPOSX--;
    if(curPOSX < 0) {
      curPOSX = lcdCOL - 1;
      curPOSY--;
    }
    if(curPOSY < 0) curPOSY = lcdROW - 1;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butSET && setMODE) { // отработка нажати€ кнопки правки параметров
  // первым параметром выводитс€ пороговое давление (4 цифры)
    if(curPOSX >= 0 && curPOSX < 4 && curPOSY == 0) {
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