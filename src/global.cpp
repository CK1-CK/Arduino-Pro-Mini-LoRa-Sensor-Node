#include "global.h"
#include "io_pins.h"

volatile uint8_t door_state = 0;                // DOOR-Status open/closed
volatile uint8_t watchdog = 1;                  // Watchdog, Differentiation between watchdog and real alarm.   1=WatchDogSignal 0=Real DoorAlarm
volatile uint8_t AlarmMode_Enabled = 0;         // Alarmmode 1=Enabled
volatile uint8_t FirstAlarmAfterJoiningTTN = 0; // After Restart of Arduino it takes minSendIntervall Sekonds till the first alarm will send. With this Option you can enable Alarm after TTN Join immediately.
volatile unsigned long oldTime = 0;
volatile unsigned long minSendIntervall = 600000; // Intervall to Send Alarm Pakages in ms  600000=10min