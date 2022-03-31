#include "global.h"
#include "io_pins.h"

volatile uint8_t door_state = 0; // DOOR-Status open/closed
volatile uint8_t watchdog = 1; // Watchdog, Differentiation between watchdog and real alarm.   1=WatchDogSignal 0=Real DoorAlarm
volatile unsigned long alteZeit=0, entprellZeit=20; //Debouncing/Entprellung Switch