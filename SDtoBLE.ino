#include <SD.h>
#include <SPI.h>
#include "ble.h"

#define SD_SS 4
#define MEGA_SS 53
#define BLE_REQN 9

File OBDLog;

void setup() {
  // Set serial rate
  Serial.begin(38400);
  
  pinMode(MEGA_SS, OUTPUT);
  
  // Setup SD card
  pinMode(SD_SS, OUTPUT);
  digitalWrite(SD_SS, HIGH);
  
  // Set up Serial Peripheral Interface for BLE
  /*SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.begin();
  
  // Start BLE
  ble_begin();
  
  
  // Set up Serial Peripheral Interface for BLE
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.begin();
  
  digitalWrite(SD_SS, LOW);
  
  SD.begin();
  
  OBDLog = SD.open("hi.txt", FILE_WRITE);
  if (OBDLog)
    Serial.println("YES");
    
  digitalWrite(SD_SS, HIGH);*/
  
  // Set up Serial Peripheral Interface for BLE
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.begin();
  
  ble_begin();
}

void loop() {
  ble_do_events();
}


