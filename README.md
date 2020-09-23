# rioc-arduino

RIOC is an acronym for Remote IO Control. rioc-arduino is an Arduino sketch program that enables Arduino IO pins to be reconfigurable and controllable by remote apps like h5control or DigiShow LINK. 

rioc-arduino is able to configure and drive your Arduino units to be parts in a simple controller network using USB for controlling digital/analog IO, motors, servos, LED lights, sensors etc.


## How to make a remote-controlled Arduino with RIOC ?

1. Need an Arduino MEGA or UNO board.

2. Open sketch RiocArduino.ino in Arduino IDE and upload the program to the Arduino. 

3. In order to be able to remote reset Arduino, also must use a wire to connect pin 12 to the RESET pin on the Arduino board.

