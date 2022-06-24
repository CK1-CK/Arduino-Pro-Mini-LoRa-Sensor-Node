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
  long diff = millis() - oldTime;
  if (diff < 0) //The millis() function will overflow (go back to zero), after approximately 50 days. (Max value = 4.294.967.295)
  {
    diff=debounceTime;
  }

  if (millis() - oldTime >= debounceTime) // Debouncing/Entprellung Switch
  {
    door_state = 0;
    watchdog = 0; // Real Alarm  // Watchdog/Heartbeat, Differentiation between watchdog and real alarm.   1=WatchDogSignal 0=Real DoorAlarm

    // Debug
    // Serial.print("Interrupt Routine: DoorState: ");
    // Serial.println(door_state, DEC);

    // Serial.println("Disabled: Interrupt Routine (DoorSwitch).");
    detachInterrupt(digitalPinToInterrupt(PIN_DOOR_SWITCH)); // Disable Interrupt Function for DoorSwitch

    oldTime = millis(); // Remember last run time.
  }
}

void CheckDoorStateAndSendLoraPackage()
{
  if (watchdog == 0) // Real Alarm?
  {
    // Queue Lora Package
    LoRaWANDo_send(&sendjob);

    // Debug
    Serial.print("DoorState: ");
    Serial.println(door_state, DEC);

    watchdog = 1; // Reset Watchdog
  }
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
  disableDeepSleep(); // DeepSleep Disable

  attachInterrupt(digitalPinToInterrupt(PIN_DOOR_SWITCH), interruptDoorFunction, FALLING); // Enable Interrupt Function for DoorSwitch
}

void loop()
{
  CheckDoorStateAndSendLoraPackage();
  LoRaWANDo();
}
