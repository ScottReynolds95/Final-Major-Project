// rf69 demo tx rx oled.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0


#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
  #define LED           8
#endif

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() 
{
  delay(500);
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  // Initialize OLED display
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  delay(500);
  oled.clearDisplay();
  oled.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX/TX Test!");

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(14, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  // OLED text display tests
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("RFM69 @ ");
  oled.print((int)RF69_FREQ);
  oled.println(" MHz");
  oled.display();

  delay(500);
}


void loop()
{  if (rf69.waitAvailableTimeout(100)) {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (! rf69.recv(buf, &len)) {
      Serial.println("Receive failed");
      return;
    }
    digitalWrite(LED, HIGH);
    rf69.printBuffer("Received: ", buf, len);
    buf[len] = 0;
    
    Serial.print("Got: "); Serial.println((char*)buf);
    Serial.print("RSSI: "); Serial.println(rf69.lastRssi(), DEC);

    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.println((char*)buf);
    oled.print("RSSI: "); oled.print(rf69.lastRssi());
    oled.display(); 
    digitalWrite(LED, LOW);
  }

  if (!digitalRead(BUTTON_A) || !digitalRead(BUTTON_B) || !digitalRead(BUTTON_C))
  {
    Serial.println("Button pressed!");
    
    char radiopacket[20] = "Button #";
    if (!digitalRead(BUTTON_A)) radiopacket[8] = 'A';
    if (!digitalRead(BUTTON_B)) radiopacket[8] = 'B';
    if (!digitalRead(BUTTON_C)) radiopacket[8] = 'C';
    radiopacket[9] = 0;

    Serial.print("Sending "); Serial.println(radiopacket);
    rf69.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf69.waitPacketSent();
  }
}

