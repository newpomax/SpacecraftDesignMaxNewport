#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <xOD01.h>

#define SDA 2  // data line for I2C
#define SCL 14 // clock line for I2C

#define TLEnum 2 // TLE to be written

xOD01 OD01; // instance of the OD01 object (zip library must be included in sketch)

const char* ssid = "ASUS";               // your network SSID (name)
const char* pass = "bravehill825";             // your network password
char servername[]="celestrak.com";           // Celestrak Server

WiFiClient client;

void setup() {
  /* Initialize serial, I2C, and OLED Display. */
  Serial.begin(115200);
  Wire.pins(SDA, SCL); // set up I2C
  Wire.begin(); // start I2C
  OD01.begin(); // start the OLED display
  OD01.clear(); // clear the OLED display of old content
  
  OD01.println("Attempting to connect to WiFi");
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED) {
    delay(1000);
    OD01.println("...");
  }

    OD01.println("Connected to wifi");
    OD01.println("\nStarting connection...");
    
    makeRequest();
    getTLE(TLEnum);
}

void makeRequest(){
    // if you get a connection, report back
    if (client.connect(servername, 80)) {
    OD01.println("connected to server");
    Serial.print("TLE for: ");
    // Make HTTP request:
    client.println("GET /NORAD/elements/iridium-33-debris.txt HTTP/1.0");     // rest of url for your chosen txt file, i.e extension following celestrak.com , Replace everything EXCEPT: GET HTTP/1.0
    client.println();                                                         
    }
    
   // if there are incoming bytes available
   // from the server, read them and print them:
  char c;
  int lineCounter=0; 
 while (!client.available()){
  // while loop runs while waiting for server availability
 }

// Skip HTTP headers
 char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }

 while (client.available()) {
    c = client.read();
    Serial.print(c);
    
    if (c == '\n'){
      lineCounter = lineCounter+1;
    }

    if (lineCounter==3){
      client.stop();
      break;
    }
  }

  // if the server becomes disconnected, stop the client:
  if (!client.connected()) {
    OD01.println("disconnecting...");
    client.stop();
  }
}

void getTLE(int whichTLE) {
  /* Asks for serial input to determine which TLE to collect. Loops until recieves a zero.
   * Each line includes the TLE name, then the two-line elements themselves. whichTLE = 1 prints
   * the first three lines to the serial output, whichTLE = 2 prints lines 4-6, and so on.
   */
  if (client.connect(servername, 80)) {
    Serial.print("TLE #");
    Serial.print(whichTLE);
    Serial.print(" from file, for: ");
    // Make HTTP request:
    client.println("GET /NORAD/elements/iridium-33-debris.txt HTTP/1.0");     // rest of url for your chosen txt file, i.e extension following celestrak.com , Replace everything EXCEPT: GET HTTP/1.0
    client.println();                                                         
  }
  else
    return;
    
  // Skip HTTP headers
 char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }
  while (!client.available()){
    // while loop runs while waiting for server availability
  }
  char c;
  int startingLine = 3 * (whichTLE - 1); // the line index on which the TLE sequence starts
  int lineCounter = 0;
  while(lineCounter <= startingLine + 2 && client.available()) { 
    c = client.read();
    if(lineCounter >= startingLine) {
      Serial.print(c);
    }
    if( c == '\n' ) {
      lineCounter += 1; 
    } 
  }
}

void loop() {
}
