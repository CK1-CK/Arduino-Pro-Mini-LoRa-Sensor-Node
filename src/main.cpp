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
int sended_AlarmPackages = 0;

void resetToDefaultValues()
{
  watchdog = 1;      // Reset Watchdog
  door_state = 1;    // Reset Doorstate
  door_counter = -1; // Reset DoorCounter
}

void CheckAlarm_SendAlarmLoraPackage()
{
  if ((millis() - oldTime > minSendIntervall) && AlarmMode_Enabled) // Limit for sending Lora Packages - The millis() function will overflow (go back to zero), after approximately 50 days. (Max value = 4.294.967.295)
  {
    if (watchdog == 0) // Real Alarm?
    {
      if (!os_queryTimeCriticalJobs(ms2osticks(300)))
      {
        
        os_setCallback(&sendjob, LoRaWANDo_send); // Queue Lora Package - Send Alarm Message
        LoRaWANDo(); //Run OS to process jobs

        Serial.println("Alarm Package queued!!"); // Debug

        oldTime = millis();     // Remember last run time.
        sended_AlarmPackages++; // Counter for sended Alarm Packages

        if (sended_AlarmPackages < 2) // Send n AlarmPackages to TTN with fast Intervall
        {
          minSendIntervall = 30000; // Fast Intervall 30 Sek
        }
        else
        {
          minSendIntervall = 600000; // If the door remains permanently open: Intervall to Send Alarm Pakages in ms  600000=10min - From here on, an alarm message is only sent every x ms.
        }
      }
    }
  }
}

void CheckDoorStateForAlarm()
{
  if (digitalRead(PIN_DOOR_SWITCH) == 0 && AlarmMode_Enabled)
  {
    door_counter++; // Doorswitch is zero --> Debouncing
  }
  else // Door is closed
  {
    resetToDefaultValues();
    minSendIntervall = 180000; // Intervall to Send Alarm Pakages in ms  180000=3min -> If door is closed reset to normal SendIntervall
    sended_AlarmPackages = 0;  // Rest Counter sended Alarm Packages
  }

  if (door_counter >= 50000) // Door must opened for some time --> Debouncing
  {
    watchdog = 0;   // Real Alarm
    door_state = 0; // Door open

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
  delay(3000);

  PrintResetReason();
  LoRaWANVersion();
  PowerDownSetupWatchdog();
  LoRaWANSetup();
  disableDeepSleep(); // DeepSleep Disable
}

void loop() // Infinity Loop
{
  if (!os_queryTimeCriticalJobs(ms2osticks(500)))
  {
    CheckDoorStateForAlarm();
  }
  LoRaWANDo();
}
