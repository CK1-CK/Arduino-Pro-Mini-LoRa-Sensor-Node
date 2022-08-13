#include <Arduino.h>
#include "io_pins.h"
#include "functions.h"
#include "lorawan.h"
#include "global.h"
#include "powerdown.h"
#include "version.h"
#include "version_build.h"
#include "settings.h"

long door_counter = -1;

void resetToDefaultValues()
{
  watchdog = 1;                              // Reset Watchdog
  door_state = digitalRead(PIN_DOOR_SWITCH); // Reset Doorstate

  // Serial.println("resetToDefaultValues");
}

void CheckAlarm_SendAlarmLoraPackage()
{
  if ((millis() - oldTime > minSendIntervall) && AlarmMode_Enabled) // Debouncing/Entprellung Switch - The millis() function will overflow (go back to zero), after approximately 50 days. (Max value = 4.294.967.295)
  {
    if (watchdog == 0) // Real Alarm?
    {
      // Queue Lora Package
      LoRaWANDo_send(&sendjob); // Send Alarm Message

      // Debug
      Serial.println("Alarm Package queued!!");
    }
    oldTime = millis(); // Remember last run time.
  }
}

void CheckDoorStateForAlarm()
{
  if (digitalRead(PIN_DOOR_SWITCH) == 0)
  {
    door_counter++; // Doorswitch is zero --> Debouncing
  }
  else // Door is closed
  {
    door_counter = -1;
    door_state = 1;
  }

  if (door_counter >= 40000) // Door must opened for some time --> Debouncing
  {
    door_state = 0;    // Door open
    watchdog = 0;      // Real Alarm
    door_counter = -1; // Door Counter Reset

    Serial.println("Door open!");

    CheckAlarm_SendAlarmLoraPackage();
    resetToDefaultValues();
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  Serial.println("Sketch: " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "." BUILD_COMMIT "-" BUILD_BRANCH);
  Serial.println("Builddate: " BUILD_DATE " " BUILD_TIME);

  Setup_Pins();
  door_state = digitalRead(PIN_DOOR_SWITCH);
  Blink_Info_LED(50, 15); // LED blink (The LED can only be used once at the beginning due to SPI PIN/collision)

  PrintResetReason();
  LoRaWANVersion();
  delay(3000);
  PowerDownSetupWatchdog();
  LoRaWANSetup();
  disableDeepSleep(); // DeepSleep Disable
}

void loop() // Infinity Loop
{
  CheckDoorStateForAlarm();
  LoRaWANDo();
}
