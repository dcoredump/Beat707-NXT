# Beat707 NXT
### Open Source Arduino Compatible MIDI Sequencer

created by William (Guilherme) Kalfelz (c) 2017

www.Beat707.com - www.Wusik.com - www.Kalfelz.com

--------------------------------------------------------------------------------

MIDI Groove Sequencer
---------------------

- 16 Drum Tracks (note on/off, 3 velocity values).
- 1 Accent Track. But each step can be set: Accent Track, Min or Max.
- 8 Note Tracks (note number, 3 velocity values). Steps have the option of Slide and Note Off.
- 30 Songs.
- 64 Patterns per Song.
- 16 Steps on screen, plus ABCD variations for a total of 64 steps.
- Double Steps (in between steps option when editing the steps).
- MIDI Processor Per Track. EG: Fade In, Fade Out, Fade InOut, Fade OutInt, Random Velocity and more.
- Simple Hardware design, based on the easy to find TM1638 IC based boards. Just search on eBay and you should be able to find several options. We used 3 boards that has 8 x buttons, 8 x LEDs and 8 x LED_Segments. The board uses a single ATmega328 running at 3.3V, one FLASH chip for storage and connections for the TM1638 boards.

--------------------------------------------------------------------------------

List of required components in order to build your unit
-------------------------------------------------------

- The Main PCB produced by us. You can order on the www.Beat707.com site.
- USB Power for the Main PCB.
- The components required by the board above. Specially the ATmega328 IC and the Flash Memory IC. The ATmega328 runs with the internal Oscillator at 8Mhz and 3.3 VCC operation.
- 3 x TM1638 boards such as this one: https://github.com/Beat707/Beat707-NXT/blob/master/Hardware/TM1638.jpg or from eBay: http://www.ebay.com/itm/400985462447
- A MIDI Cable and a MIDI compatible unit to produce some Audio. EG: a synth keyboard or a sound module. As the Beat707 NXT unit itself only produces MIDI, NOT Audio.
- To program the ATmega328 you will also need some sort of ISP programmer. Ordering an UNO based ATmega328 won't do, as it requires 5V and 16Mhz external oscillator to run. The easy way is to order an Arduino UNO and the USB Tiny ISP programmer to program the ATmega328 on the UNO, changing it to run on 3.3V and internal 8Mhz Oscillator.