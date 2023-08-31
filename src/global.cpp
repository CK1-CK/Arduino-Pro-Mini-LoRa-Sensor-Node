#include "global.h"
#include "io_pins.h"

volatile uint8_t doorState = 0;                // DOOR-Status open/closed
volatile uint8_t watchdog = 1;                  // Watchdog, Differentiation between watchdog and real alarm.   1=WatchDogSignal 0=Real DoorAlarm
volatile uint8_t AlarmModeEnabled = 0;         // Alarmmode 1=Enabled
volatile unsigned long oldTime = 0;
volatile unsigned long minSendIntervall = 20*60*1000; // Intervall to Send Alarm Pakages in ms  1200000 = 20min | 60000=1min