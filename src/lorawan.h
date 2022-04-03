#pragma once
#ifndef _LORAWAN_H
#define _LORAWAN_H

#include <Arduino.h>
#include <lmic.h>
#include "lora_credentials.h"

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = TTN_APPEUI;

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = TTN_DEVEUI;

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = TTN_APPKEY;

static osjob_t sendjob;

void os_getArtEui(u1_t *buf);
void os_getDevEui(u1_t *buf);
void os_getDevKey(u1_t *buf);

void LoRaWANSetup(void);
void LoRaWANDo_send(osjob_t *j);
void LoRaWANDo(void);
void LoRaWANGetData(void);
void LoRaWANVersion(void);
void disableDeepSleep(void);

#endif