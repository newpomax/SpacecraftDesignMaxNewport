#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <xSN01.h>
#include <xSI01.h>
#include <xSL01.h>
#include <xSW01.h>

#define SDA 2  // data line for I2C
#define SCL 14 // clock line for I2C
#define DELAY_TIME 3000 // delay time between chip readouts in millis

xSN01 SN01; // instance of SN01 (GNSS)
xSI01 SI01; // instance of SI01 (Inertial)
xSL01 SL01; // instance of SL01 (light sensor)
xSW01 SW01; // instance of SW01 (weather sensor)

void read_SN01(void);
void read_SI01(void);
void read_SL01(void);
void read_SW01(void);

void setup() {
  /* Initialize serial, I2C, and various components. */
  Serial.begin(9600);
  Wire.pins(SDA, SCL); // set up I2C
  Wire.setClockStretchLimit(15000);
  Wire.begin(); // start I2C

  if( !SN01.begin() ) { Serial.println("FAILED TO START SN01"); } // start SN01 (GNSS)
  if( !SI01.begin() ) { Serial.println("FAILED TO START SI01"); } // start SI01 (intertial)
  if( !SL01.begin() ) { Serial.println("FAILED TO START SL01"); } // start SL01 (light sensor)
  if( !SW01.begin() ) { Serial.println("FAILED TO START SW01"); } // start SW01 (weather sensor

  delay(DELAY_TIME);

  read_SI01(); // read from inertial unit and print to OLED
  delay(DELAY_TIME);

  read_SL01(); // read from light sensor and print to OLED
  delay(DELAY_TIME);

  read_SW01(); // read from weather sensor and print to OLED
  delay(DELAY_TIME);

  // int i = 0;
  // int max_iters = 10;
  // SN01.poll();
  // if(SN01.getSatelitesConnected() <= 0) {
  //   Serial.print("Awaiting satellites on SN01...");
  //   delay(DELAY_TIME);
  //   SN01.poll();
  //   while(SN01.getSatelitesConnected() <= 0 && i < max_iters) {
  //     i++;
  //     Serial.print(".");
  //     delay(DELAY_TIME);
  //     SN01.poll();
  //   }
  //   Serial.print("\n");
  // }
  // if( i == max_iters )
  //   Serial.println("Failed to connect to GNSS satellites.");
  // else {
    read_SN01(); // read from GNSS and print to OLED
    delay(DELAY_TIME);
  // }

  Serial.println("FINISHED CHIP TEST.");
}

void loop() {
}

void read_SN01()
{
  Serial.println("SN01 Chip Test >>> "); // write header to screen
  SN01.poll(); // update sensors by polling for data
  Serial.printf("GPS Date: %s | GPS Time: %s | ", SN01.getDate().c_str(), SN01.getTime().c_str());
  Serial.printf("No. Sats Cnctd: %d | ", SN01.getSatelitesConnected());
  Serial.printf("Lat: %f N | Long: %f E | ", SN01.getLatitude(), SN01.getLongitude());
  Serial.printf("Alt: %f MSL \n", SN01.getAltitude() );
}

void read_SI01(){
  Serial.println("SI01 Chip Test >>> ");
  SI01.poll();
  Serial.printf("AX: %f | AY: %f | AZ: %f | ", SI01.getAX(), SI01.getAY(), SI01.getAZ());
  Serial.printf("MX: %f | MY: %f | MZ: %f | ", SI01.getMX(), SI01.getMY(), SI01.getMZ());
  Serial.printf("GX: %f | GY: %f | GZ: %f | ", SI01.getGX(), SI01.getGY(), SI01.getGZ());
  Serial.printf("Pitch: %f | Roll: %f | Yaw: %f | ", SI01.getPitch(), SI01.getRoll(), SI01.getYaw());
  Serial.printf("GForce: %f \n", SI01.getGForce());
}

void read_SL01(){
  Serial.println("SL01 Chip Test >>> ");
  SL01.poll();
  SL01.getLUX();
  SL01.getUVIndex();
  Serial.printf("UVA: %f uW/m2 | UVB: %f uW/m2 | LUX: %f | UV Index: %f \n", SL01.getUVA(), SL01.getUVB(), SL01.getLUX(), SL01.getUVIndex() ) ;
}

void read_SW01(){
  Serial.println("SW01 Chip Test >>> ");
  SW01.poll();
  Serial.printf("Temp: %f degC | Humidity: %f | Pressure: %e Pa | ", SW01.getTempC(), SW01.getHumidity(), SW01.getPressure()/1000000);
  Serial.printf("Altitude: %f m | Dew Point: %f degC \n", SW01.getQNE(), SW01.getDewPoint());
}
