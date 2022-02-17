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
 * ƒл€ получени€ давлени€ в ћѕа от датчика давлени€, необходимо (ADC - 90) * 0.00145
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
  lcd.begin(lcdCOL, lcdROW);

  // «апуск интерфейса I2C
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
  if(userP > 10 || userP < 0 || isnanf(userP)) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // читаем количество событий срабатывани€ по низкому давлению
  if(quantEVENT > 99 || quantEVENT < 0 || isnan(quantEVENT)) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {
  int keyPRESS = 0; // сбрасываем номер нажатой кнопки
  int refrScreen = lcdREFR / waitKey; // счетчик обновлени€ информации на дисплее 

  while(refrScreen) {
    keyPRESS = testKey(0); // провер€ем нажатие кнопок
    if(keyPRESS) { // делаем паузу и провер€ем нажатие дл€ устранени€ дребезга
      delay(retPAUSE);
      if(testKey(keyPRESS)) {
        while(testKey(0)) {
          delay(waitKey);
        }
        break;
      }
      keyPRESS = 0; // иначе считаем дребезгом и восстанавливаем начальное состо€ние
    }
      
    if(countSETMODE) { // по завершению правки необходимо записать значение в пам€ть и выключить режим правки      
      if(countSETMODE == 1) {
        setMODE = false;
        EEPROM.put(adressP_EEPROM, userP);
        delay(5);  
      }
      countSETMODE--;
    }

    delay(waitKey);
    refrScreen--;
  }

  if(keyPRESS) functionKey(keyPRESS);
  printScreen(); // выводим информацию на экран

}