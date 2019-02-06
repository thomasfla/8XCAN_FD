#include <ACAN2517.h>


static const byte CS_CAN_A = PA3; 
static const byte CS_CAN_B = PA4; 

ACAN2517 can_A(CS_CAN_A, SPI, 255); 
ACAN2517 can_B(CS_CAN_B, SPI, 255); 

void setup()
{
  //--- Switch on builtin led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //--- Start serial
  Serial.begin(38400);
  delay(100);
  SPI.begin();
  
  ACAN2517Settings settings_A(ACAN2517Settings::OSC_20MHz, 125 * 1000); // CAN bit rate 125 kb/s
  ACAN2517Settings settings_B(ACAN2517Settings::OSC_20MHz, 125 * 1000); // CAN bit rate 125 kb/s
  
  settings_A.mRequestedMode = ACAN2517Settings::InternalLoopBack;       // Select loopback mode
  settings_B.mRequestedMode = ACAN2517Settings::InternalLoopBack;       // Select loopback mode
  
  const uint32_t errorCode_A = can_A.begin(settings_A, NULL);               // No ISR
  Serial.print ("Configuration error code A 0x") ;
  Serial.println (errorCode_A, HEX) ;
  can_A.writeByteRegister (0xE00, 0x00);                                //to get 20Mhz as output clock TODO add it in the lib
  
  delay(10);
  const uint32_t errorCode_B = can_B.begin(settings_B, NULL);               // No ISR  
  Serial.print ("Configuration error code B 0x") ;
  Serial.println (errorCode_B, HEX) ; 
  can_B.writeByteRegister (0xE00, 0x00);                                //to get 20Mhz as output clock TODO add it in the lib
  
}

static unsigned gBlinkLedDate = 0;
static unsigned gReceivedFrameCount_A=0;
static unsigned gReceivedFrameCount_B=0;

static unsigned gSentFrameCount_A=0;
static unsigned gSentFrameCount_B=0;

void loop()
{
  can_A.poll(); // Call can.poll as often as possible
  //delay(1);
  can_B.poll(); // Call can.poll as often as possible
  CANMessage frame;
  if (gBlinkLedDate < millis())
  {
    gBlinkLedDate += 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    const bool ok_A = can_A.tryToSend(frame);
    const bool ok_B = can_B.tryToSend(frame);
    if (ok_A)
    {
      gSentFrameCount_A += 1;
      Serial.print("CAN A Sent: ");
      Serial.println(gSentFrameCount_A);
    }
    else
    {
      Serial.println("CAN A Send failure");
    }
    if (ok_B)
    {
      gSentFrameCount_B += 1;
      Serial.print("CAN B Sent: ");
      Serial.println(gSentFrameCount_B);
    }
    else
    {
      Serial.println("CAN B Send failure");
    }

    
  }
  if (can_A.available())
  {
    can_A.receive(frame);
    gReceivedFrameCount_A++;
    Serial.print("CAN_A Received: ");
    Serial.println(gReceivedFrameCount_A);
  }
  if (can_B.available())
  {
    can_B.receive(frame);
    gReceivedFrameCount_B++;
    Serial.print("CAN_B Received: ");
    Serial.println(gReceivedFrameCount_B);
  }

  
}
