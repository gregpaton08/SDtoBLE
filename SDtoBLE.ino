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
  digitalWrite(SD_SS, LOW);
  SD.begin();
  
  // Setup BLE
  setBLEActive();
  ble_begin();
}


void loop() {
  const uint8_t len = 16;
  char buf[len];
  uint8_t cnt = 0;
  while (ble_available() > 0 && cnt < len) {
    buf[cnt] = ble_read();
    if (buf[cnt] == 'l' && cnt == 0) {
      listFiles();
      setBLEActive();
      break;
    }
    else if (buf[cnt] == 'o' && cnt == 0) {
      BLEopenFile(OBDLog);
      break;
    }
    else if (buf[cnt] == 'r' && cnt == 0) {
      BLEreadLine(OBDLog);
      break;
    }
    else if (buf[cnt] == 'd' && cnt == 0) {
    }
    else
      ++cnt;
  }
  ble_do_events();
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

void listFiles() {
  setSDActive();
  File dir = SD.open("/");
  dir.rewindDirectory();
  while(true) {
    File entry =  dir.openNextFile();
    if (false == entry) {
      break;
    }
    if (false == entry.isDirectory() 
        && entry.name()[0] != '~') {
      writeToBLE(entry.name());
      setSDActive();
    }
  }
}

boolean BLEopenFile(File &file) {
  boolean ret = false;
  const uint8_t len = 16;
  char name[len];
  uint8_t cnt = 0;
  while (ble_available() == 0);
  while (ble_available() > 0 && cnt < len) {
    name[cnt] = ble_read();
    if (name[cnt] < 46) {
      name[cnt] = '\0';
      break;
    } 
    ++cnt;
  }
  setSDActive();
  if (strcmp("open", name)) {
    if (SD.exists(name)) {
      file = SD.open(name, FILE_READ);
      if (file) 
        ret = true;
    }
  }
  setBLEActive();
  return ret;
}

boolean BLEreadLine(File &file) {
  const uint8_t len = 32;
  char line[len];
  uint8_t cnt = 0;
  char c = -1;
  
  setSDActive();
  
  if (false == file) 
    return false;
    
  Serial.println("READ");
  
  while (true) {
    line[cnt] = file.read();
    if (line[cnt] == '\n') {
      line[cnt] = '\0';
      break;
    }
    ++cnt;
  }
  Serial.print("line: ");
  Serial.print(line);
  Serial.println(";");
  writeToBLE(line);
  
  return true;
}
