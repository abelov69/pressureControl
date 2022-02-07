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
 
  // ��������� ������� ��� ������ � ����
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);

  // ������ �� EEPROM ���������������� ������
  EEPROM.get(adressP_EEPROM, userP); // ������ �������� ���������� ��������
  if(userP > 1023 || userP < 0) {
    userP = 0;
    EEPROM.put(adressP_EEPROM, userP);
    delay(5);
  }
  EEPROM.get(adressQ_EEPROM, quantEVENT); // ������ ���������� ������� ������������ �� ������� ��������
  if(quantEVENT > 99 || quantEVENT < 0) {
    quantEVENT = 0;
    EEPROM.put(adressQ_EEPROM, quantEVENT);
    delay(5);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  keyPRESS = 0; // ���������� ����� ������� ������
  int refrCount = 0; // ������� ���������� ���������� �� ������� 

  while(!keyPRESS) {
    printScreen(); // ������� ���������� �� �����
    refrCount = lcdREFR / retPAUSE;
    while(refrCount) {
      if(!digitalRead(butPUMP)) keyPRESS = butPUMP; // ��������� ������� ������
      if(!digitalRead(butLEFT)) keyPRESS = butLEFT;
      if(!digitalRead(butRIGHT)) keyPRESS = butRIGHT;
      if(!digitalRead(butSET)) keyPRESS = butSET;

      if(keyPRESS) { // ������ ����� � ��������� ������� ��� ���������� ��������
        delay(retPAUSE);
        if(!digitalRead(keyPRESS)) break;
        keyPRESS = 0;
      }
      
      if(!countSETMODE) { // �� ���������� ������ ���������� �������� �������� � ������ � ��������� ����� ������
        setMODE = false;
        EEPROM.put(adressP_EEPROM, userP);
        delay(5);
      }
      else countSETMODE--;
      delay(retPAUSE);
      refrCount--;
    }
    
  }

  if(keyPRESS == butPUMP) { // ��������� ������� ������ ��������������� ������� ���� ��������
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

  if(keyPRESS == butRIGHT) { // ��������� ������� ������ ����������� ������� ������
    curPOSX++;
    if(curPOSX >= lcdCOL) { 
      curPOSX = 0;
      curPOSY++;
    }
    if(curPOSY >= lcdROW) curPOSY = 0;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butLEFT) { // ��������� ������� ������ ����������� ������� �����
    curPOSX--;
    if(curPOSX < 0) {
      curPOSX = lcdCOL - 1;
      curPOSY--;
    }
    if(curPOSY < 0) curPOSY = lcdROW - 1;
    setMODE = true;
    countSETMODE = setmodeTIME / retPAUSE;
  }

  if(keyPRESS == butSET && setMODE) { // ��������� ������� ������ ������ ����������
  // ������ ���������� ��������� ��������� �������� (4 �����)
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