# Beat707 NXT
### Open Source Arduino Compatible MIDI Sequencer

created by William (Guilherme) Kalfelz (c) 2017

www.Beat707.com - www.Wusik.com - www.Kalfelz.com

Support us on Patreon: https://patreon.com/williamk

--------------------------------------------------------------------------------

MIDI Groove Sequencer
---------------------

- 15 Drum Tracks (note on/off, 3 velocity values).
- 1 Accent Track. But each step can be set: Accent Track, Min or Max.
- 8 Note Tracks (note number, 3 velocity values). Steps have the option of Slide, Double and Note Off.
- 30 Songs.
- 64 Patterns per Song.
- 16 Steps on screen, plus ABCD variations for a total of 64 steps.
- Double Steps (in between steps option when editing the steps).
- MIDI Processor Per Track. EG: Fade In, Fade Out, Fade InOut, Fade OutInt, Random Velocity and more.
- Simple Hardware design, based on the easy to find TM1638 IC based boards. Just search on eBay and you should be able to find several options. We used 3 boards that has 8 x buttons, 8 x LEDs and 8 x LED_Segments. The board uses a single ATmega328 running at 3.3V, one FLASH chip for storage and connections for the TM1638 boards.
- MIDI Input and Output, with Clock Send and Receive (Slave mode too).

--------------------------------------------------------------------------------

How The Software Works
----------------------

- The first board is the main interface. Where it shows the Tempo in BPM and the current Pattern. The top leds, first 4 leds show the beat position and the last 4 leds shows the ABCD pattern variation. Also, beats will blink to bottom dot of the 4th digit from left to right.
- Still in the first board, the first button is Play, second button is Stop. 3rd is - and 4th is +. 5th, when hold, let you force a step to Max, Min or Accent. 6th button, if you press and release, will alternate mirror mode, where all ABCD variations will be edited at once, or not, when disabled. You can also hold it and select the first 4 buttons on the second board to select ABCD variations directly. The 7th button, if you press and release, will go to the next pattern. If you hold, it will show the current selected pattern on the two other boards LEDs. The final button, the 8th one, is for selecting the track been edited. If you select a track twice, it will go to the Note Track, instead of Drum Track.
- Holding button #5 from the first board and selecting the button #7, will open the pattern menu: copy, paste, init. Or selecting button #8 will open the global settings menu.
- Adding steps is easy, just click to turn On or Off. By default when you turn a step On it will use the Accent track (last track). Or you can hold button #5 from the left board and turn a step On: Max, Min and Accent. Holding a step will turn the Double-Step On/Off. The Double-Step uses the same velocity as the regular step it is on. Double-Steps are 1/32 steps.
- The Slide option for the notes work like this: regular notes will turn the previous note Off before turning the new step note On. When Slide is activated for the step, the new Step note On will come Before the previous Step Note Off. So if the target MIDI device supports Glide/Slide it will do just that.

--------------------------------------------------------------------------------

Global Options Menu
-------------------
- Button #7 is Previous Item and Button #8 is Next Item. 
- Buttons #3 and #4 are the usual - and +.
- Step buttons will select which track you are editing parameters for (when applicable).
- To exit click button #5 again.

First Board (from the left).

```
  1 2 3 4 5 6 7 8 
  ---------------
      - + E   < >
          x
          i
          t
```

--------------------------------------------------------------------------------

Pattern Menu
------------
- Just click the button next to the option you want: Copy, Paste and Init. Or hit the button #5 again to exit.

--------------------------------------------------------------------------------

Interface Overview
------------------

```
  1 2 3 4 5 6 7 8    1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8
  ---------------    ---------------  ---------------
  P S - + M A P T    S S S S S S S S  S S S S S S S S
  l t     e B a r    t t t t t t t t  t t t t t t t t
  a o     n C t a    e e e e e e e e  e e e e e e e e
  y p     u D t c    p p p p p p p p  p p p p p p p p
              e k  
          A V r      0 0 0 0 0 0 0 0  0 1 1 1 1 1 1 1
          c a n      1 2 3 4 5 6 7 8  9 0 1 2 3 4 5 6
          c r
          e i
          n a
          t t
            i
            o
            n
            s
```

--------------------------------------------------------------------------------

Hold Button #5 and Press Button #8, from the first board on the left.

```
  1 2 3 4 5 6 7 8 
  ---------------
          +_____+
             Global Options Menu.
```

--------------------------------------------------------------------------------

Hold Button #5 and Press Button #7, from the first board on the left.

```
  1 2 3 4 5 6 7 8
  ---------------  
          +___+
            Pattern Menu: Copy, Paste and Init.
```

--------------------------------------------------------------------------------

List of required components in order to build your unit
-------------------------------------------------------

- The Main PCB produced by us. You can order on the www.Beat707.com site.
- USB Power for the Main PCB.
- The components required by the board above. Specially the ATmega328 IC and the Flash Memory IC. The ATmega328 runs with the internal Oscillator at 8Mhz and 3.3 VCC operation.
- 3 x TM1638 boards such as this one: https://github.com/Beat707/Beat707-NXT/blob/master/Hardware/TM1638.jpg or from eBay: http://www.ebay.com/itm/400985462447
- A MIDI Cable and a MIDI compatible unit to produce some Audio. EG: a synth keyboard or a sound module. As the Beat707 NXT unit itself only produces MIDI, NOT Audio.
- To program the ATmega328 you will also need some sort of ISP programmer. Ordering an UNO based ATmega328 won't do, as it requires 5V and 16Mhz external oscillator to run. The easy way is to order an Arduino UNO and the USB Tiny ISP programmer to program the ATmega328 on the UNO, changing it to run on 3.3V and internal 8Mhz Oscillator.
- The Flash memory is the Winbond W25Q64 BVAIG - DIP

--------------------------------------------------------------------------------

Connecting the boards
---------------------

- From the Main PCB you can see the Main Header, which starts from the top: 5V+, Ground, STB (Strobe), CLK (Clock), Data 1, data 2 and Data 3.
- Now, the first 4 pins are shared on all the 3 x TM1638 boards. While Data 1 is for the first board (on the left) DIO pin, data 2 is for the middle board DIO pin and finally Data 3 for the last (on the right) board DIO pin.
- The best thing is to remove the J1 headers from the TM1638 boards and just pass wires from behind the board.

Main Header
-----------

- 5V+
- GND (Ground)
- STB (Strobe)
- CLK (Clock)
- Data 1 (board on the left)
- Data 2 (middle board)
- Data 3 (board on the right)

--------------------------------------------------------------------------------

BOM (Build Of Materials) List for the Main PCB
----------------------------------------------
- 1x ATmega328P-PU DIP - 8-Bit Microcontroller
- 1x W25Q64 DIP - 8Mbit Flash Memory Chip
- 1x 4N35 DIP - Octopler
- 1x 1k Ohm Resistor
- 1x 10k Ohm Resistor
- 1x 200 Ohm Resistor
- 1x 10 Ohm Resistor
- 1x 33 Ohm Resistor
- 1x 1N4748 Diode
- 2x Female Din-5 - MIDI Connector
- 1x 47uf Electrolitic Capacitor
- 2x 100nh Ceramic Capacitor
- 1x LD33V - 3.3V Voltage Regulator
- 1x Female USB-B Connector
- 3x Jumpers
- 6x Male Headers for the FTDI Connection