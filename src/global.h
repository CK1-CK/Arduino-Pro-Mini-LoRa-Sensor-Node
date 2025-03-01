#pragma once
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <Arduino.h>

extern volatile int POWER_DOWN_SLEEP_COUNTER;
extern volatile uint8_t doorState; // DOOR-Status open/closed
extern volatile uint8_t watchdog; // Watchdog, Differentiation between watchdog and real alarm.
extern volatile uint8_t AlarmModeEnabled; // Alarmmode 1=Enabled
extern volatile unsigned long oldTime, minSendIntervall; //Debouncing/Entprellung Switch

#endif