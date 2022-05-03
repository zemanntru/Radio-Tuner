## Overview
Code for a hardware design project: controlling the local oscillator frequency for a radio submodule. The microcontroller used is ATMega324. The mcu folder contains code that allows the user to control the Adafruit Si5351 clock generator by adjusting the Sparkfun Qwiic Twist I2C rotary encoder, as well as toggling between transmission and reception mode for the radio via the RX/TX switch. The gui folder provides a virtual numberpad app that allows the user to control the Si5351 through a breakout board.


| Serial Terminal Settings |      |
| ------------------------ | ---- |
| number of bits           | 8    |
| stop bits                | 2    |
| baud rate                | 2400 |
| parity                   | none |

<center><img src="./app-screenshot.png" alt="Diagram 1" ></img></center>
<center><img src="./radio_tuner.jpg" alt="Diagram 1" ></img></center>
