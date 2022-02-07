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
    // ������ ���������� ��������� ���� (dd.mm.yy), ����� ����� (hh:mm), �� ������ ������ ��������� �������� (4 �����)
    if(curPOSY == 0) {
      if(curPOSX == 0) { // ������ ����� ����� (����)
        int fdig = Clock.getDate();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 3) sdig = 0;
        if(sdig == 3 && fdig > 1) fdig = 1;
        Clock.setDate(sdig * 10 + fdig);
      }
      if(curPOSX == 1) { // ������ ����� ����� (����)
        int dig = Clock.getDate() + 1;        
        if(dig > 31) dig = 0;
        Clock.setDate(dig);
      }
      if(curPOSX == 3) { // ������ ����� ������ (����)
        int fdig = Clock.getMonth(Century);
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 1) sdig = 0;
        if(sdig == 1 && fdig > 2) fdig = 2;
        Clock.setMonth(sdig * 10 + fdig);
      }
      if(curPOSX == 4) { // ������ ����� ������ (����)
        int dig = Clock.getMonth(Century) + 1;        
        if(dig > 12) dig = 0;
        Clock.setMonth(dig);
      }
      if(curPOSX == 6) { // ������ ����� ���� (����)
        int fdig = Clock.getYear();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 9) sdig = 0;
        Clock.setYear(sdig * 10 + fdig);
      }
      if(curPOSX == 7) { // ������ ����� ���� (����)
        int dig = Clock.getYear() + 1;        
        if(dig > 99) dig = 0;
        Clock.setYear(dig);
      }
      if(curPOSX == 9) { // ������ ����� ���� (�����)
        int fdig = Clock.getHour(h24, PM);
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 2) sdig = 0;
        if(sdig == 2 && fdig > 4) fdig = 4;
        Clock.setHour(sdig * 10 + fdig);
      }
      if(curPOSX == 10) { // ������ ����� ���� (�����)
        int dig = Clock.getHour(h24, PM) + 1;        
        if(dig > 24) dig = 0;
        Clock.setHour(dig);
      }
      if(curPOSX == 12) { // ������ ����� ����� (�����)
        int fdig = Clock.getMinute();
        int sdig = fdig / 10 + 1;
        fdig = fdig % 10;
        if(sdig > 5) sdig = 0;        
        Clock.setMinute(sdig * 10 + fdig);
      }
      if(curPOSX == 13) { // ������ ����� ����� (�����)
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