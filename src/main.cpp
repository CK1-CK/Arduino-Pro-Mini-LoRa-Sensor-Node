#include <Arduino.h>
#include "io_pins.h"
#include "functions.h"
#include "lorawan.h"
#include "global.h"
#include "powerdown.h"
#include "version.h"
#include "version_build.h"
#include "settings.h"

long doorCounter = -1;
int counterAlarmPackages = 0;

// Resets all Alarm values to default
void resetToDefaultValues()
{
  watchdog = 1;     // Reset Watchdog
  doorState = 1;    // Reset Doorstate
  doorCounter = -1; // Reset DoorCounter
}

// Set Alarm State Values
void setAlarmState()
{
  watchdog = 0;  // Real Alarm
  doorState = 0; // Door open
}

// Check runtimes and sends Lora Packages
void SendAlarmLoraPackage() // Sends a Lora Package
{
  if ((millis() - oldTime > minSendIntervall) && AlarmModeEnabled) // Limit for sending Lora Packages - The millis() function will overflow (go back to zero), after approximately 50 days. (Max value = 4.294.967.295)
  {
    if (!os_queryTimeCriticalJobs(ms2osticks(300))) // Check for Time critical Job
    {
      os_clearCallback(&sendjob);               // Clear the SendQueue
      os_setCallback(&sendjob, LoRaWANDo_send); // Queue Lora Package - Send Alarm Message
      LoRaWANDo();                              // Run OS to process jobs

      Serial.println("Alarm Package queued!!"); // Debug

      oldTime = millis();     // Remember last run time.
      counterAlarmPackages++; // Counter for sent Alarm Packages

      if (counterAlarmPackages < 2) // Send n AlarmPackages to TTN with fast Intervall
      {
        minSendIntervall = 30000; // Fast Intervall 30 Sek
      }
      else
      {
        minSendIntervall = 600000; // If the door remains permanently open: Intervall to Send Alarm Pakages in ms  600000=10min - From here on, an alarm message is only sent every x ms.
      }
    }
    // Serial.println("os_queryTimeCriticalJobs - EXIT"); // Debug
  }
  // Serial.println("SendAlarmLoraPackage - EXIT"); // Debug
}

// Door Check Function. If door is open (Denbouncing) send Lora Alarm Package
void CheckDoorStateForAlarm()
{
  if (digitalRead(PIN_DOOR_SWITCH) == 0 && AlarmModeEnabled)
  {
    if (doorCounter < DOOR_COUNTER) // Limits the DoorCounter and prevent a variable overflow
    {
      doorCounter++; // Doorswitch is zero --> Debouncing
    }

    if (doorCounter >= DOOR_COUNTER) // Door must opened for some time --> Debouncing
    {
      setAlarmState();
      Serial.println("Door open!");

      SendAlarmLoraPackage();
      resetToDefaultValues();
    }
  }
  else // Door is closed
  {

    if (doorState == 0) // Runs once if the door is closed. Creates a new WatchDog Package
    {
      os_clearCallback(&sendjob);                                                             // Clear the SendQueue
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL/2), LoRaWANDo_send); // Schedule next transmission
      Serial.println("Next WatchDog Package is scheduled.");                                  // Serial Print - Debug
    }

    resetToDefaultValues();
    minSendIntervall = 180000; // Intervall to Send Alarm Pakages in ms 180000=3min --> If door is closed reset to normal SendIntervall --> Alarm system reactivated after 3 minutes.
    counterAlarmPackages = 0;  // Rest Counter sent Alarm Packages
  }
}

//Setup Function (Serial Interface, Lora Interface)
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  Serial.println("Sketch: " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "." BUILD_COMMIT "-" BUILD_BRANCH);
  Serial.println("Builddate: " BUILD_DATE " " BUILD_TIME);

  Setup_Pins();
  doorState = digitalRead(PIN_DOOR_SWITCH);
  Blink_Info_LED(50, 15); // LED blink (The LED can only be used once at the beginning due to SPI PIN/collision)
  delay(3000);

  PrintResetReason();
  LoRaWANVersion();
  PowerDownSetupWatchdog();
  LoRaWANSetup();
  disableDeepSleep(); // DeepSleep Disable
}

// Infinite Loop
void loop() // Infinity Loop
{
  CheckDoorStateForAlarm();
  LoRaWANDo();
}
