/*
    FRAM I2C Example
    ----------------
    Description:
    This is FRAM Memory Read/Write operation in ease of use.

    Tested Boards: Arduino UNO, Nano (Atmega328p MCU)
    MCU Clock: 16 MHz
    Tested I2C Device: FM24CL16B FRAM (Waveshare)

    *****************************************************************************
    **Notes - Don't forget to check MCU clock if another Arduino board is used.**
    **        (e.g. Pro Mini 3.3v, 8MHz)                                       **
    *****************************************************************************
    **Caution - If Mega board is used, don't forget to uncomment and comment   **
    **          ports at i2cMaster_Init() in "Master_TWI.h" header.            **
    *****************************************************************************

    Date: 17 Sep 2019

    Written By
    Salai Aung Myint Myat

*/

#include "Fram_Rx_Tx_Operation.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  delay(3000);
  char data[10];
  Serial.println('s');
  Serial.println(Ref_Sec);

  i2cMaster_Init();
  Serial.println("init");
  
  //*******FRAM Write*******/
  FRAM_Write(0x05, 'F');

  //*******FRAM Read*******/
  char c = FRAM_Read(0x08);
  FRAM_Read_Array(0x05, 6, data);
  Serial.println(c);
  Serial.println(data);
  Serial.println('e');
  //  delay(1000);

  //*******READ/WRITE Simutaneously*******/
  FRAM_Write(0x04, 'R');
  FRAM_Read_Array(0x04, 7, data);
  Serial.println(data);
  Serial.println("end");
}

void loop() {

}


