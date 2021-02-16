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

#define RL03_FREQ 915.0
#define CW01_RED 12
#define CW01_GREEN 13
#define CW01_BLUE 5

#define num_cycles 10
static char ModemConfigs[RL0X.Bw125Cr48Sf4096+1][20] = {"Bw125Cr45Sf128","Bw500Cr45Sf128","Bw31_25Cr48Sf512","Bw125Cr48Sf4096"};

void setup() {
  Wire.pins(2,14);
  Wire.setClockStretchLimit(15000);
  Wire.begin();

  // Start the Serial Monitor
  Serial.begin(9600);
  delay(1000);

  // Set the RGB Pin directions
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!RL0X.begin()) { // <-- enter radio name here
    Serial.println("Check the connector to RL03");
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
    int c = 0;
    while( c < num_cycles ) {
      digitalWrite(LED_BUILTIN,HIGH);
      Serial.println("Waiting");
      if (RL0X.waitAvailableTimeout(3000)) {
        uint8_t buf[195];
        uint8_t len = sizeof(buf);
        if (RL0X.recv(buf, &len)) {
          digitalWrite(LED_RED, HIGH);
          Serial.println("got message: ");
          Serial.println((char*)buf);
          Serial.print("RSSI: ");
          Serial.println(RL0X.lastRssi(), DEC);

          // Send a reply
          uint8_t data[] = "And hello back to you";
          delay(100);
          RL0X.send(data, sizeof(data));
          Serial.println("Sent a reply");
          digitalWrite(LED_RED, LOW);
          c++; // if sent a reply, increment
        } else {
          Serial.println("recv failed");
        }
      }
      digitalWrite(LED_BUILTIN,LOW);
    }
  }
  Serial.println("Finished with all modem configurations.");
  while(1) {}; //stall after completion
}
