/**
 * @file main.cpp
 * @author Andrey Belov (d_mak@rambler.ru)
 * @brief ��������� �������� �������� � ������� ���������
 * @details ��������� ������ ��������� � ������� �������� � ������� ���������. ��� ������ ������ �������� lowP
 * �������� ����, ����������� ���������������� �������� ��� ������� ��������. ���������� ����� LCD �����, �� �������
 * ������� ������� ���� � ����� (������ � ������ DS3231 � ��������� ��������), ���� ���������� ������������ �� �������
 * ��������, ���������� ������������ � ������ ������ ����������, ������� �������� � �������. ����� ����������
 * ����� ��������� ������ (�������������� ��������� ������/�������, ��������� ���������� ���� � �������,
 * ���������� ��������).
 * ������ ����� �������� �� ���� I2C (����������� ���� A4 - SDA, A5 - SCL)
 * ��� ��������� �������� � ��� �� ������� ��������, ���������� (ADC - 90) * 0.00145
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

  // ������ ���������� I2C
	Wire.begin();
 
  // ��������� ������� ��� ������ � ����
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);

  // ������ �� EEPROM ���������������� ������
  EEPROM.get(adressP_EEPROM, userP); // ������ �������� ���������� ��������
  if(userP > 10 || userP < 0 || isnanf(userP)) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // ������ ���������� ������� ������������ �� ������� ��������
  if(quantEVENT > 99 || quantEVENT < 0 || isnan(quantEVENT)) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {
  int keyPRESS = 0; // ���������� ����� ������� ������
  int refrScreen = lcdREFR / waitKey; // ������� ���������� ���������� �� ������� 

  while(refrScreen) {
    keyPRESS = testKey(0); // ��������� ������� ������
    if(keyPRESS) { // ������ ����� � ��������� ������� ��� ���������� ��������
      delay(retPAUSE);
      if(testKey(keyPRESS)) {
        while(testKey(0)) {
          delay(waitKey);
        }
        break;
      }
      keyPRESS = 0; // ����� ������� ��������� � ��������������� ��������� ���������
    }
      
    if(countSETMODE) { // �� ���������� ������ ���������� �������� �������� � ������ � ��������� ����� ������      
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
  printScreen(); // ������� ���������� �� �����

}