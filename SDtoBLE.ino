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
  
  //pinMode(MEGA_SS, OUTPUT);
  pinMode(BLE_REQN, OUTPUT);
  
  // Setup SD card
  pinMode(SD_SS, OUTPUT);
  digitalWrite(SD_SS, HIGH);
  
  //SPI.begin();
  
  digitalWrite(SD_SS, LOW);
  SD.begin();
  
  listSDFiles();
  OBDLog = SD.open("hello.txt", FILE_WRITE);
  if (OBDLog)
    Serial.println("YES");
    
  digitalWrite(SD_SS, HIGH);
  
  setBLEActive();
  
  ble_begin();
}

const uint8_t len = 16;
char buf[len];

void loop() {
  uint8_t cnt = 0;
  while (ble_available() > 0 && cnt < len) {
    buf[cnt] = ble_read();
    if (buf[cnt] == 'l') {
      listSDFiles();
      setBLEActive();
    }
    else if (buf[cnt] == 'a') {
      writeToBLE("hello");
    }
    ++cnt;
  }
  if (cnt > 0) {
    Serial.print(buf);
    saveToSD(buf);
    setBLEActive();
  }
  ble_do_events();
  delay(100);
}

void saveToSD(char *buf) {
  setSDActive();
  OBDLog.print(buf);
  OBDLog.flush();
}

void writeToBLE(char *buf) {
  setBLEActive();
  uint8_t cnt = 0;
  while (buf[cnt] != '\0') {
    ble_write(buf[cnt]); 
    ++cnt; 
  }
  ble_do_events();
}

void setSDActive() {
  digitalWrite(BLE_REQN, HIGH);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  digitalWrite(SD_SS, LOW);
}

void setBLEActive() {
  digitalWrite(SD_SS, HIGH);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  digitalWrite(BLE_REQN, LOW);
}

void listSDFiles() {
  setSDActive();
  File dir = SD.open("/");
  File file;
  while (file = dir.openNextFile()) {
    // if file is not a directory or hidden file
    // then list file 
    if (false == file.isDirectory() 
        && file.name()[0] != '.'
        && file.name()[0] != '~') {
      Serial.println(file.name());
      setSDActive();
    }
  }
}
