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
#include <Wire.h>
#include <LiquidCrystal.h>
#include "DS3231.h"
#include "settings.h"
#include "myFunction.h"

void setup() {
  // put your setup code here, to run once:
  lcd.begin(lcdCOL, lcdROW);
  lcd.print("Hello world!");
  delay(2000);

  // Start the I2C interface
	Wire.begin();
  /*
  Clock.setSecond(00);//Set the second 
  Clock.setMinute(00);//Set the minute 
  Clock.setHour(10);  //Set the hour 
  Clock.setDoW(5);    //Set the day of the week
  Clock.setDate(4);  //Set the date of the month
  Clock.setMonth(2);  //Set the month of the year
  Clock.setYear(22);  //Set the year (Last two digits of the year)
  */
  // ��������� ������� ��� ������ � ����
  pinMode(butPUMP, INPUT);
  pinMode(butLEFT, INPUT);
  pinMode(butRIGHT, INPUT);
  pinMode(butSET, INPUT);
  pinMode(pinRELAY1, OUTPUT);
  pinMode(pinRELAY2, OUTPUT);
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
      
      countSETMODE--;
      if(!countSETMODE) setMODE = false;
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