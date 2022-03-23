#include "functions.h"
#include "io_pins.h"
#include "global.h"

void Setup_Pins()
{
  pinMode(PIN_DOOR_SWITCH, INPUT_PULLUP); // Doorswitch Pin (Interrupt)
}

void Blink_Info_LED(unsigned int time = 50, unsigned int count = 1)
{
  for (size_t i = 0; i < count; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(time);                     // wait
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(time);                     // wait
  }
}

// https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
long ReadVcc()
{
// Read 1.1V reference against AVcc
// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2);            // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bitRead(ADCSRA, ADSC))
    ;

  uint8_t adc_low = ADCL;  // must read ADCL first - it then locks ADCH
  uint8_t adc_high = ADCH; // unlocks both
  long adc_result = (adc_high << 8) | adc_low;
  adc_result = 1125300L / adc_result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return adc_result;                  // Vcc in millivolts
}

void PrintResetReason()
{
  uint8_t mcusr_copy = MCUSR;
  MCUSR = 0;
  Serial.print("MCUSR:");
  if (mcusr_copy & (1 << WDRF))
    Serial.print(" WDRF");
  if (mcusr_copy & (1 << BORF))
    Serial.print(" BORF");
  if (mcusr_copy & (1 << EXTRF))
    Serial.print(" EXTRF");
  if (mcusr_copy & (1 << PORF))
    Serial.print(" PORF");
  Serial.println();
}