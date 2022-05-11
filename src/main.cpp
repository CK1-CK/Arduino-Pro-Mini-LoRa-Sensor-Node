#include <Arduino.h>
#include "io_pins.h"
#include "functions.h"
#include "lorawan.h"
#include "global.h"
#include "powerdown.h"
#include "version.h"
#include "version_build.h"
#include "settings.h"

void interruptDoorFunction() // Interrupt Function for DoorSwitch
{
  if ((millis() - oldTime) > debounceTime) // Debouncing/Entprellung Switch
  {
    door_state = digitalRead(PIN_DOOR_SWITCH);
    watchdog = 0;       // Real Alarm  // Watchdog/Heartbeat, Differentiation between watchdog and real alarm.   1=WatchDogSignal 0=Real DoorAlarm
    oldTime = millis(); // Remember last run time.

    // Debug
    Serial.print("Interrupt Routine: DoorState: ");
    Serial.println(door_state, DEC);
  }
}

void CheckDoorStateAndSendLoraPackage()
{
  if (watchdog == 0 && door_state == 0)
  {
    detachInterrupt(digitalPinToInterrupt(PIN_DOOR_SWITCH)); // Disable Interrupt Function for DoorSwitch
    Serial.println("Interrupt Routine disabled.");

    disableDeepSleep();
    //setTX_Interval(10); // Try to send Lora Package during the next n seconds

    // Queue Lora Package
    LoRaWANDo_send(&sendjob);

    // Debug
    Serial.print("DoorState: ");
    Serial.println(door_state, DEC);
  }

  watchdog = 1; // Reset Watchdog
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  Serial.println("Sketch: " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "." BUILD_COMMIT "-" BUILD_BRANCH);
  Serial.println("Builddate: " BUILD_DATE " " BUILD_TIME);
  PrintResetReason();
  LoRaWANVersion();
  delay(3000);
  PowerDownSetupWatchdog();
  Setup_Pins();
  Blink_Info_LED(50, 15); // LED blink (The LED can only be used once at the beginning due to SPI PIN/collision)
  LoRaWANSetup();

  attachInterrupt(digitalPinToInterrupt(PIN_DOOR_SWITCH), interruptDoorFunction, CHANGE); // Interrupt Function for DoorSwitch
}

void loop()
{
  CheckDoorStateAndSendLoraPackage();
  LoRaWANDo();
}
