#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;

int greenLed = 3;
int redLed = 4;
int button = 5;

void setup() 
{
  Serial.begin(9600);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(button, INPUT);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
}

void loop()
{
  if (digitalRead(button)) {
    //button is pressed, message should be sent, turn the green LED on
    digitalWrite(greenLed, HIGH);    

   // Send a message
    uint8_t data[] = "Aloha";
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();
    digitalWrite(greenLed, LOW);
  } else {
    // Wait for a message
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    while (nrf24.waitAvailableTimeout(200) && nrf24.recv(buf, &len))
    {   
        //something was received, turn the right LED on
        digitalWrite(redLed, HIGH);      
    }      
    digitalWrite(redLed, LOW);    
  }
}
