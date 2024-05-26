#include <SPI.h>
#include <RH_RF95.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NOSERIAL true

// Singleton instance of the radio driver
RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

int led = 9;

void setup() {
  if (!NOSERIAL) Serial.println("Starting LoRa messaging service");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    if (!NOSERIAL) Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // Display static text
  display.println("LoRa Messaging!");
  display.setCursor(0, 10);
  display.println("no messages recieved yet");
  display.display(); 

  pinMode(led, OUTPUT);
  if (!NOSERIAL) Serial.begin(9600);

  if (!NOSERIAL) while (!Serial) ; // Wait for serial port to be available

  if (!rf95.init())
    if (!NOSERIAL) Serial.println("init failed");
    delay(100); // do i need this?
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("LoRa radio not working :(");
    display.display(); 

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // You can change the modulation parameters with eg
  // rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);

// The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 2 to 20 dBm:
//  driver.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);
}

void loop() {
  if (rf95.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      digitalWrite(led, HIGH);
//      RH_RF95::printBuffer("request: ", buf, len);
      if (!NOSERIAL) Serial.print("got request: ");
      // Serial.println((char*)buf);
      // I want to flush out buf I have no idea why this is tweaking
      
      // Send a reply
      uint8_t data[] = "recieved";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      digitalWrite(led, LOW);

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      // Display static text
      display.println("LoRa Messaging!");
      display.setCursor(0, 10);
      display.println((char*)buf);

      if (!NOSERIAL) Serial.print("RSSI (server): ");
      if (!NOSERIAL) Serial.println(rf95.lastRssi(), DEC);

      display.setCursor(60, 22);
      display.printf("RSSI: %d", rf95.lastRssi());
      display.display();

    } else {
      if (!NOSERIAL) Serial.println("recv failed");
          delay(100); // do i need this?
      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 10);
      // Display static text
      display.println("recv failed");
      display.display();
    }
  }
}


