# SNK_LS-30_Rotary_Joystick_SNAC_adapter
## Introduction

This is a proof of concept of how to build a SNAC adapter for the MiSTer platform to natively use SNK's LS-30 arcade rotary controls, used in T.A.N.K, Ikari Warriors or Victory Road games among others. 
This adapter allows you to connect the entire wiring harness to a STM32F103CT8 microcontroller breadboard known as Blue Pill. It is an inexpensive development board and very similar to an Arduino nano, the difference is that this time it mounts an ST micro, specifically the STM32F103C8T6, this micro is much more powerful than the Atmega328 of the Arduino Nano, we can also program it from the Arduino IDE, with which the ease of programming is just as simple, once we have loaded its bootloader.
I have chosen this board for its power, low cost, and ease of use (if you are used to moving around in the DIY world of electronics). It also allows you to connect all your wiring directly to the board if you already have your I/O port pinouts soldered using DuPont cables. 
Of course this is a totally experimental Do It Yourself project and I do not guarantee that it will be free of bugs nor am I responsible for any possible damage that may arise from its use. It is your responsibility to use it. 

## Bill of Material
* STM32F103C8T6 "Blue Pill".
* One or Two LS-30 Rotary joystick with cable harness.
* Typical arcade cable kit for microswitches with female pin header in the other end.
* Dupont cables male-female.
* USB 3 pcb adapter.
* Generic maker tools: cable pliers, electric tape, tin welder, tin, ...

If you build the LS-30 13pin cable conector harness adapter to 4 cable:
* PCB double side for protoboard.
* 10K 1/4W 9pin resistor array.
* 10nF ceramic capacitors.
* 1K 1/4W carbon film resistors.
* Wirewrap cable


## Schematics
![adapter schematic](/img/SNK_LS30_SNAC_ADAPTER_esquematico.png)

## Circuit

![adapter circuit](/img/SNK_LS30_SNAC_ADAPTER_bb.png)
![my setup2](/img/my_setup2.jpg)
![LS-30 cable harness](/img/LS30_CABLE_HARNESS_CONNECTION_TO_PCB.png)
Already mounted in a test box:
![my setup](/img/my_setup.jpg)
