// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

void setup() {

  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  Serial.println("LoRa Messaging client is running...!");

  // You can change the modulation parameters with eg
  // rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 2 to 20 dBm:
//  rf95.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  rf95.setTxPower(14, true);
}

/*
I want to add some error message checking
A.) if what we read from the serial monitor is too long (> RH_MAX) then inform the user that only ___ will
be sent
B.) if no "recieved" is found (or is broken) we need to inform the user
*/
void loop() {
  Serial.printf("Sending to rf95_server... max size is %d\n", RH_RF95_MAX_MESSAGE_LEN);
  char buffer[RH_RF95_MAX_MESSAGE_LEN];
  // buffer[0] = '!'; // gets around "is alive" issues

  int count = 0;
  bool overflow = false;
  while (Serial.available() > 0) {
    if (count < RH_RF95_MAX_MESSAGE_LEN - 1) { // '<' makes it so null char can be included?
      buffer[count] = Serial.read();
      Serial.println(buffer[count]);
    } else {
      if (!overflow) {
        Serial.println("Your message was too long, omitting:");
        overflow = true;
      }
      Serial.print(Serial.read());
    }
    count++;
  }
  // Serial.printf("\nwrote '%s'\n", buffer);

  rf95.send((uint8_t*) buffer, sizeof(buffer));

  rf95.waitPacketSent();

  /*
    // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  bool first_doa = true;
  while (!rf95.waitAvailableTimeout(3000)) { // changing from if so we stop having this endless loop of stuff
    if (first_doa) {
      Serial.println("no response from listener, is rf95_server running?");
      first_doa = false;
    }
  }
  // Should be a reply message for us now   
  if (rf95.recv(buf, &len)) {
    Serial.println((char*)buf);
    Serial.print("RSSI (client): ");
    Serial.println(rf95.lastRssi(), DEC);    
  }
  else {
    Serial.println("recv failed");
  }
  */
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(3000)) { // changing from if so we stop having
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len)) {
      Serial.println((char*)buf);
      Serial.print("RSSI (client): ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else {
      Serial.println("recv failed");
    }
  } else {
    Serial.println("No reply, is rf95_server running?");
    // Serial.printf("Your message '%s' was not transmitted\n", buffer);
  }
  delay(400);
}


