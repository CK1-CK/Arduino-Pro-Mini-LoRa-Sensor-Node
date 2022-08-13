#include <Arduino.h>
#include "io_pins.h"
#include "functions.h"
#include "lorawan.h"
#include "global.h"
#include "powerdown.h"
#include "version.h"
#include "version_build.h"
#include "settings.h"

unsigned int test = 0;
long door_counter = -1;

void CheckDoorStateAndSendLoraPackage()
{
  Serial.println("CheckDoorStateAndSendLoraPackage");
  if (millis() - oldTime > minSendIntervall) // Debouncing/Entprellung Switch - The millis() function will overflow (go back to zero), after approximately 50 days. (Max value = 4.294.967.295)
  {
    if (watchdog == 0) // Real Alarm?
    {
      // Queue Lora Package
      LoRaWANDo_send(&sendjob);

      // Debug
      // Serial.print("DoorState: ");
      // Serial.println(door_state, DEC);

      watchdog = 1; // Reset Watchdog

      // Debug
      Serial.println("Scheisse!!");
      Serial.println(test++, DEC);
    }
    oldTime = millis(); // Remember last run time.
  }
}

void CheckDoorStateForAlarm()
{
  if (digitalRead(PIN_DOOR_SWITCH) == 0)
  {
    door_counter++;
  }
  else
  {
    door_counter = -1;
  }

  if (door_counter >= 200) // Door must opened for some time
  {
    door_state = 0;
    watchdog = 0;
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
}

void loop()
{
  CheckDoorStateForAlarm();

  if (!os_queryTimeCriticalJobs(5000))
  {
    CheckDoorStateAndSendLoraPackage();
  }

  LoRaWANDo();
}
