# Arduino Pro Mini TTN LoRaWAN Door Switch node (Alarm System)

`Arduino Pro Mini` TTN LoRaWAN Node with `DoorSwitch` and `SX1276` Lora module.

The Arduino can be converted to [LowPower](https://jackgruber.github.io/2019-12-27-Low-power-Arduino-Pro-Mini/) by desoldering the power LED and the voltage converter.


## TTN payload decoder (v3)

see payload_formatter folder


## LMIC MCCI Lib - EU Configuration

.pio\libdeps\pro8MHzatmega328\MCCI LoRaWAN LMIC library\project_config\lmic_project_config.h
    #define CFG_eu868 1
    #define CFG_sx1276_radio 1
