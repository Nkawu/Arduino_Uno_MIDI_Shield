# Arduino Uno MIDI Shield

<img src="https://raw.githubusercontent.com/Nkawu/Arduino_Uno_MIDI_Shield/master/images/shield.png" width="300">

This project originated during the conversion of an old AGO spec pipe organ pedalboard to MIDI in order to be used with
[Hauptwerk](https://www.hauptwerk.com) virtual organ software. 

The shield uses 4 daisy chained CD4021 shift registers to read 32 reed switches installed on the pedalboard. The Arduino Uno reads the shift registers and then sends MIDI notes via [Hiduino](https://github.com/ddiakopoulos/hiduino) firmware flashed to the Uno's Atmel ATmega16U2. The Hiduino firmware makes the Arduino Uno appear as a class-compliant USB-MIDI device.

The project contains: 
* Eagle CAD files for an Arduino Uno R3 shield
* Arduino sketch

**NOTE:** This is still a work in progress. I've breadboarded the circuit with a single CD4021 which works perfectly, but I have yet to daisy chain multiple CD4021s on the breadboard. When that has been done I'll have PCBs made at [OSHPark](https://oshpark.com)

![Schematic](https://raw.githubusercontent.com/Nkawu/Arduino_Uno_MIDI_Shield/master/images/shield_schematic.png)