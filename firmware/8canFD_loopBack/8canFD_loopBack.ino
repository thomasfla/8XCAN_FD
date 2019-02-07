#include <ACAN2517.h>

#define CS_CAN_A  PB11
#define CS_CAN_B  PB10 
#define CS_CAN_C  PB1 
#define CS_CAN_D  PB0 
#define CS_CAN_E  PA4 
#define CS_CAN_F  PA3 
#define CS_CAN_G  PA2 
#define CS_CAN_H  PA1
                   
ACAN2517 can_A(CS_CAN_A, SPI, 255); 
ACAN2517 can_B(CS_CAN_B, SPI, 255); 
ACAN2517 can_C(CS_CAN_C, SPI, 255);
ACAN2517 can_D(CS_CAN_D, SPI, 255);
ACAN2517 can_E(CS_CAN_E, SPI, 255);
ACAN2517 can_F(CS_CAN_F, SPI, 255);
ACAN2517 can_G(CS_CAN_G, SPI, 255);
ACAN2517 can_H(CS_CAN_H, SPI, 255);

#define NUMBER_OF_CAN 8

ACAN2517 *can[NUMBER_OF_CAN];

int i;
void setup()
{
  can[0] = &can_A;
  can[1] = &can_B;
  can[2] = &can_C;
  can[3] = &can_D;
  can[4] = &can_E;
  can[5] = &can_F;
  can[6] = &can_G;
  can[7] = &can_H;
  
    //--- Switch on builtin led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //--- Start serial
  Serial.begin(38400);
  delay(100);
  SPI.begin();
  
  ACAN2517Settings settings(ACAN2517Settings::OSC_20MHz, 125 * 1000); // CAN bit rate 125 kb/s
  settings.mRequestedMode = ACAN2517Settings::InternalLoopBack;       // Select loopback mode
  uint32_t errorCode[NUMBER_OF_CAN]={0};
  for (i = 0;i<NUMBER_OF_CAN;i++)
  {
      errorCode[i] = can[i]->begin(settings, NULL);               // No ISR
      Serial.print ("Configuration error code 0x") ;
      Serial.println (errorCode[i], HEX) ;
      can[i]->writeByteRegister (0xE00, 0x00);  //to get 20Mhz as output clock TODO add it in the lib
  }
}

static unsigned gBlinkLedDate = 0;
static unsigned gReceivedFrameCount[NUMBER_OF_CAN]={0};
static unsigned gSentFrameCount[NUMBER_OF_CAN]={0};

void loop()
{
  for (i = 0;i<NUMBER_OF_CAN;i++)
  {
    can[i]->poll(); // Call can.poll as often as possible
  }
  CANMessage frame;
  if (gBlinkLedDate < millis())
  {
    gBlinkLedDate += 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    for (i = 0;i<NUMBER_OF_CAN;i++)
    {
      if(can[i]->tryToSend(frame))
      {
      gSentFrameCount[i]++;
      Serial.print("CAN Sent: ");
      Serial.println(gSentFrameCount[i]);
      }
      else 
      {
        Serial.println("CAN A Send failure");
      }
    }
  }
  for (i = 0;i<NUMBER_OF_CAN;i++)
  {
    if (can[i]->available())
    {
      can[i]->receive(frame);
      gReceivedFrameCount[i]++;
      Serial.print("CAN Received: ");
      Serial.println(gReceivedFrameCount[i]);
    }
  }
 

  
}
