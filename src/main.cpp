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
 * Модуль часов и экран работают по шине I2C (задействует ноги A4 - SDA, A5 - SCL)
 * Для получения давления в МПа от датчика давления, необходимо (ADC - 90) * 0.00145
 * @version 0.1
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DS3231.h"
#include "settings.h"
#include "myFunction.h"

void setup() {
  // Запуск интерфейса I2C
  Wire.begin();

  // Инициализация экрана
  lcd.init();
  lcd.backlight();

  // Настройка выводов под кнопки и реле
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);

  // Чтение из EEPROM пользовательских данных
  EEPROM.get(adressP_EEPROM, userP); // читаем значение порогового давления
  if(userP > 10 || userP < 0 || isnanf(userP)) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // читаем количество событий срабатывания по низкому давлению
  if(quantEVENT > 99 || quantEVENT < 0 || isnan(quantEVENT)) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {

  int keyPRESS = 0; // сбрасываем номер нажатой кнопки
  int refrScreen = lcdREFR / waitKey; // счетчик обновления информации на дисплее 

  while(refrScreen) {
    keyPRESS = testKey(0); // проверяем нажатие кнопок
    if(keyPRESS == butPUMP) break; // если нажата кнопка подкачки, то сразу уходим на ее обработку
    if(keyPRESS) { // делаем паузу и проверяем нажатие для устранения дребезга
      delay(retPAUSE);
      if(testKey(keyPRESS)) {
        while(testKey(0)) {
          delay(waitKey);
        }
        break;
      }
      keyPRESS = 0; // иначе считаем дребезгом и восстанавливаем начальное состояние
    }
      
    if(countSETMODE) { // по завершению правки необходимо записать значение в память и выключить режим правки      
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