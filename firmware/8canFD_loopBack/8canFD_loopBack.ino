#include <ACAN2517.h>
static const byte MCP2517_CS = PA4; // CS input of MCP2517

ACAN2517 can(MCP2517_CS, SPI, 255); // Last argument is 255 -> no interrupt pin

void setup()
{
  //--- Switch on builtin led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //--- Start serial
  Serial.begin(38400);
  //--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial)
  {
    delay(50);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  SPI.begin();
  //--- Configure ACAN2517
  Serial.println("Configure ACAN2517");
  ACAN2517Settings settings(ACAN2517Settings::OSC_20MHz, 125 * 1000); // CAN bit rate 125 kb/s
  settings.mRequestedMode = ACAN2517Settings::InternalLoopBack;       // Select loopback mode
  const uint32_t errorCode = can.begin(settings, NULL);               // No ISR

  //     uint32_t reg = can.readRegisterSPI(0xE00);
  //     Serial.println(reg);
  //     reg&=~(0x0000|(1<<5)|(1<<6));
  //     can.writeRegisterSPI(0xE00,reg);
  //Serial.println(can.readRegisterSPI(0xE00));
}

static unsigned gBlinkLedDate = 0;
static unsigned gReceivedFrameCount = 0;
static unsigned gSentFrameCount = 0;

void loop()
{
  can.poll(); // Call can.poll as often as possible
  CANMessage frame;
  if (gBlinkLedDate < millis())
  {
    gBlinkLedDate += 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    const bool ok = can.tryToSend(frame);
    if (ok)
    {
      gSentFrameCount += 1;
      Serial.print("Sent: ");
      Serial.println(gSentFrameCount);
    }
    else
    {
      Serial.println("Send failure");
    }
  }
  if (can.available())
  {
    can.receive(frame);
    gReceivedFrameCount++;
    Serial.print("Received: ");
    Serial.println(gReceivedFrameCount);
  }
}
