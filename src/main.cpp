#include <Arduino.h>
#include "io_pins.h"
#include "functions.h"
#include "lorawan.h"
#include "global.h"
#include "powerdown.h"
#include "version.h"
#include "version_build.h"



void interruptDoorFunction() //Interrupt Function for DoorSwitch
{
  door_state = digitalRead(PIN_DOOR_SWITCH);
  watchdog=0; //Real Alarm

  //Send Lora Package
  LoRaWANDo_send(&sendjob);

  watchdog=1; //Restet Watchdog
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting DHT ..."));
  Serial.println("Sketch: " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "." BUILD_COMMIT "-" BUILD_BRANCH);
  Serial.println("Builddate: " BUILD_DATE " " BUILD_TIME);
  PrintResetReason();
  LoRaWANVersion();
  delay(3000);
  PowerDownSetupWatchdog();
  Setup_Pins();
  Blink_Info_LED(50, 15); // LED blink (The LED can only be used once at the beginning due to SPI PIN/collision)
  LoRaWANSetup();

  attachInterrupt(digitalPinToInterrupt(PIN_DOOR_SWITCH), interruptDoorFunction, CHANGE); //Interrupt Function for DoorSwitch
}

void loop()
{
  LoRaWANDo();
}
