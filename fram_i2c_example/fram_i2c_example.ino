/*
    FRAM I2C Example
    ----------------
    Description:
    This is FRAM Memory Read/Write operation in ease of use.

    Tested Boards: Arduino UNO, Nano (Atmega328p MCU)
    MCU Clock: 16 MHz
    Tested I2C Device: FM24CL16B FRAM (Waveshare),
                       MB85RC256V FRAM

    *****************************************************************************
    **Notes - Don't forget to check MCU clock if another Arduino board is used.**
    **        (e.g. Pro Mini 3.3v, 8MHz)                                       **
    *****************************************************************************
    **Caution - If Mega board is used, don't forget to uncomment and comment   **
    **          ports at i2cMaster_Init() in "Master_TWI.h" header.            **
    *****************************************************************************
    !!UPDATED: - Additional function that terminate I2C communication protocol.
                 "i2cMaster_Disable()"
               - Change to 16-bit word address type (MB85RC256V)
                 "FRAM_Word_Adr(n)"
                 n = 0 -> 8-bit word address (Default)
                 n = 1 -> 16-bit word address

    ##WARNING##
    Two different FRAM types (FM24CL16B and MB85RC256V) cannot use at the same time.
    Because it will make faulty memory saving and reading.
    --> Therefore, only use 1 type of FRAM, same FRAMs can use in parallel.

    Date: 3 Oct 2019

    Written By
    Salai Aung Myint Myat

*/

#include "Fram_Rx_Tx_Operation.h"

#define FRAM_ADR_1            0x50
//#define FRAM_ADR_2            0x51

void setup() {
  Serial.begin(9600);
  Serial.println("+++Start+++");
  delay(3000);
  
  char data[10];
  char wr[] = "HAVE A GOOD Day!";
  char rd[20];
  Serial.print("T-shift = ");
  Serial.println(Ref_Sec);




  //--------------------------------//
  //-------------TEST 1-------------//
  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);        // 1 for 16-bit word address type
  Serial.println("init - done");
  Serial.println();
  
  //*******FRAM Write*******/
  Serial.println("---Test 1: Insert bytes---");
  
  FRAM_Write(0x01, 'F');
  FRAM_Write(0x02, 'A');
  FRAM_Write(0x03, 'M');
  FRAM_Write(0x04, 'E');

  //*******FRAM Read*******/
  char c = FRAM_Read(0x03);
  FRAM_Read_Array(0x01, data, 4);
  i2cMaster_Disable();      // Optional: I2C communication deactivate

  Serial.print("Test Char: ");
  Serial.println(c);
  Serial.print("Test Array: ");
  Serial.println(data);
  Serial.println();


  //-------------TEST 2-------------//
  //*******READ/WRITE Simutaneously*******/
  Serial.println("---Test 2: Update bytes---");
  
  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);         // 1 for 16-bit word address type
  FRAM_Write(0x01, 'G');
  FRAM_Write(0x05, 'R');
  FRAM_Read_Array(0x01, data, 5);
  char c1 = FRAM_Read(0x05);
  i2cMaster_Disable();

  Serial.print("Test Char: ");
  Serial.println(c1);
  Serial.print("Print Array: ");
  Serial.println(data);
  Serial.println();
  

  //-------------TEST 3-------------//
  //*******Write Array and Read Array*******/
  Serial.println("---Test 3: Insert String---");
  
  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);           // 1 for 16-bit word address type
  FRAM_Write_Array(0x10, wr);
  FRAM_Read_Array(0x10, rd, 16);
  char c2 = FRAM_Read(0x1D);
  i2cMaster_Disable();
  
  Serial.print("Print Str: ");
  Serial.println(rd);
  Serial.print("Char: ");
  Serial.println(c2);
  Serial.println();
  Serial.println("+++End Test+++");
}

void loop() {

}


