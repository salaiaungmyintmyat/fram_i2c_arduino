# FRAM I2C Driver for Arduino
This is a Arduino library/driver for FRAM Read/Write operations. I've tested on FM24CL16B FRAM (Waveshare module) and it's perfectly work!

As MCU, I've tested on most ATmega328p MCU based boards (5V, 16 MHz) like Arduino UNO and Nano. If another board like Pro Mini (3.3V, 8 MHz) is used, then it is needed to change MCU clock frequency in "Master_TWI.h" header file. If another MCU is used, it is needed to check its datasheet for I2C pins configuration (SDA and SCL pins).

## I2C Pinout Hookup
Two wires is needed to use I2C protocol: SDA(Serial Data), SCL(Serial Clock).

On ATmega328p MCU, I2C pinouts are as followed.
- SDA: PC4 (A4 - Analog input 4)
- SCL: PC5 (A5 - Analog input 5)

On ATmega2560 MCU, I2C pinouts are as followed.
- SDA: PD1 (D20 - Digital I/O 20)
- SCL: PD0 (D21 - Digital I/O 21)

These two pins are needed pull-up resistor with Vcc (5V). Simply used 1 kOhm resistor, or calculate minimum or maximum resistance based on Vcc and bus frequency.

In this sketch, it doesn't need to connect with pull-up resistor because these two pins are internally pull-up with source code. Simply hookup with SCL and SDA pins directly to the slave device.

## FRAM Slave Address
There are two different slave addresses intended for read/write operation. 
- 0xA0: Write Operation
- 0xA1: Read Operation
