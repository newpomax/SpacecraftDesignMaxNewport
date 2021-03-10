#include "logXK07.h"
#include <xCS11_SDU.h>
#include <SPI.h>
#include <SD.h>
#include <xOD01.h>
//#include "XinaBox_XK07.h"

//xOD01 OD01;  // OLED object
logXK07 logger; // logger object

// Writes data of choice to CSV file on SD card

void setup(){
  // Start the I2C communication
  #ifdef ESP8266
    Wire.pins(2, 14);
  #endif
  Wire.begin();

  logger.enableExcelLogOutput(false);
  // Start the Logger
  logger.begin();
}

void loop(){
  logger.LogSI01();
  delay(10);
}
