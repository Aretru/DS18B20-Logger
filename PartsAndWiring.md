# Parts
* Arduino Nano
* LED Green
* LED Red
* DS1320 RTC
* 4x DS18B20
* SD Card Adapter
* 330 Ohm Resistor
* 4.7 kOhm Resistor  
  
The used LEDs have build-in pre-resistors. Therefore I am not using any additional resistors. If you don't have LEDs with build-in resistors chose them accordingly.

# Wiring
## RTC
* VCC: 3.3 V
* GND: GND
* CLK: D4
* DAT: D5
* RST: D6

## SD Card Adapter
* CS: D10
* SCK: D13
* MOSI: D11
* MISO: 330 Ohm Resistor - D12 
* VCC: 5V
* GND: GND

## DS18B20
All sensors are wired to the same connection. With only one wire the 4 sensors are read out. Therefore only one resistor is necessary between 5V and data. 
* Plus: 5V
* Data: D3
* 4.7 kOhm Resistor between data pin and 5V
* GND: GND

## LEDs
* Green: D7
* Red: D8
