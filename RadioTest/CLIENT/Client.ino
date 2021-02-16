/*************************************************************
  This is an examples for the RL01-02-03 Radio Range

  You can buy one on our store!
  -----> https://xinabox.cc/products/RL01/
  -----> https://xinabox.cc/products/RL02/
  -----> https://xinabox.cc/products/RL03/

  This example requests the Alcohol sensor to measure
  the Breath Alcohol Level

  Currently Supported on the following ☒CHIPs:
  - CW01
  - CR01/02/03

  The sensor communicates over the I2C Bus.

*************************************************************/

#include <xCore.h>
#include <xRL0x.h>
#include <SPI.h>
#include <SD.h>

#define RL03_FREQ 915.0
#define Serial SerialUSB
#define CHIP_SELECT_PIN 3

#define num_cycles 10
static char ModemConfigs[RL0X.Bw125Cr48Sf4096+1][20] = {"Bw125Cr45Sf128","Bw500Cr45Sf128","Bw31_25Cr48Sf512","Bw125Cr48Sf4096"};

File myFile;

void setup() {
  #ifdef ESP8266
  Wire.pins(2,14);
  Wire.setClockStretchLimit(15000);
  #endif
  // Start the Serial Monitor
  Serial.begin(9600);

 // Set the RGB Pin directions
 // pinMode(LED_RED, OUTPUT);
 // pinMode(LED_GREEN, OUTPUT);
 // pinMode(LED_BUILTIN, OUTPUT);

 Wire.begin(); // start I2C

 while(!Serial); // wait for serial to connect
 Serial.println("Starting setup...");


 while(!SD.begin(CHIP_SELECT_PIN)){
   Serial.println("Failed to initialize SD card...");
   delay(100);
 }
 myFile = SD.open("RSSIvals.txt", FILE_WRITE);

 if(!myFile)
    Serial.println("File failed to open.");

  if (!RL0X.begin()) {
    Serial.println("Check the connector to CR01");
    while (1) {
      // Flash RED to indicate failure
      digitalWrite(LED_RED, HIGH);
      delay(100);
      digitalWrite(LED_RED, LOW);
      delay(100);
    }
  } else {
    // RL0X Initialized correctly
    RL0X.setFrequency(RL03_FREQ);
    RL0X.setTxPower(23, false);
  }

  Serial.println("Setup done.");
}

void loop() {
  for( int i = RL0X.Bw125Cr45Sf128 ; i <= RL0X.Bw125Cr48Sf4096 ; i++ ) {
    switch( i ){
      case (int)RL0X.Bw125Cr45Sf128:
        RL0X.setModemConfig(RL0X.Bw125Cr45Sf128); //
        break;
      case (int)RL0X.Bw500Cr45Sf128:
        RL0X.setModemConfig(RL0X.Bw500Cr45Sf128); //
        break;
      case (int)RL0X.Bw31_25Cr48Sf512:
        RL0X.setModemConfig(RL0X.Bw31_25Cr48Sf512); //
        break;
      case (int)RL0X.Bw125Cr48Sf4096:
        RL0X.setModemConfig(RL0X.Bw125Cr48Sf4096); //
        break;
    }
    Serial.print("Modem Config - ");
    Serial.print(ModemConfigs[i]);
    Serial.println(" || Sending to RL0X Server");
    myFile.print("Modem Config - ");
    myFile.println(ModemConfigs[i]);
    myFile.flush();
    int c = 0;
    while( c < num_cycles ) {

      digitalWrite(LED_GREEN, HIGH);

      uint8_t data[] = "Hello World!";
      delay(100);
      RL0X.send(data, sizeof(data));

      uint8_t buf[195];
      uint8_t len = sizeof(buf);

      if (RL0X.waitAvailableTimeout(3000)) {
        if (RL0X.recv(buf, &len)) {
          Serial.print("got reply: ");
          Serial.println((char*)buf);
          Serial.print("RSSI: ");
          Serial.println(RL0X.lastRssi(), DEC);
          myFile.print("RSSI: ");
          myFile.println(RL0X.lastRssi(), DEC);
          myFile.flush();
          c++; //increment on success
        } else {
          Serial.println("recv failed");
        }
      } else {
        digitalWrite(LED_GREEN, LOW);
        Serial.println("No reply, is the RL01 server running ?");
      }
    }
  }
  Serial.println("Finished with all modem configurations.");
  myFile.close();
  while(1) {}; //stall after completion
}
