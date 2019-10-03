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
               - Change to 16-bit word address (MB85RC256V)
                 "FRAM_Word_Adr(n)"
                 n = 0 -> 8-bit word address (Default)
                 n = 1 -> 16-bit word address

    Date: 30 Sep 2019

    Written By
    Salai Aung Myint Myat

*/

#include "Fram_Rx_Tx_Operation.h"

#define FRAM_ADR_1            0x54
#define FRAM_ADR_2            0x52

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t num = 0x00;

void setup() {
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");

  Serial.begin(9600);
  Serial.println("+++Start+++");
  delay(3000);
  char data[10];
  char wr[] = "HAVE A GOOD Day!";
  char rd[20];
  Serial.print("T-shift = ");
  Serial.println(Ref_Sec);

  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);        // 1 for 16-bit word address
  Serial.println("init - done");
  Serial.println();

  //*******FRAM Write*******/
  Serial.println("---Test 1: Insert bytes---");

  FRAM_Write(0x00, 'F');
  FRAM_Write(0x01, 'A');
  FRAM_Write(0x02, 'M');
  FRAM_Write(0x03, 'E');
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test-1");

  //*******FRAM Read*******/
  char c = FRAM_Read(0x02);
  FRAM_Read_Array(0x00, data, 4);
  //  i2cMaster_Disable();      // Optional: I2C communication deactivate

  Serial.print("Test Char: ");
  Serial.println(c);
  Serial.print("Test Array: ");
  Serial.println(data);
  Serial.println();
  delay(1000);

  //*******READ/WRITE Simutaneously*******/
  Serial.println("---Test 2: Update bytes---");

  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);         // 1 for 16-bit word address
  FRAM_Write(0x00, 'G');
  FRAM_Write(0x04, 'R');

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test-2");
  char c0 = FRAM_Read(0x04);

  FRAM_Read_Array(0x00, data, 5);
  //  i2cMaster_Disable();

  Serial.print("Print Array: ");
  Serial.println(data);
  Serial.print("Char: ");
  Serial.println(c0);
  Serial.println();
  delay(1000);

  //*******Write Array and Read Array*******/
  Serial.println("---Test 3: Insert String---");

  i2cMaster_Init(FRAM_ADR_2);
  FRAM_Word_Adr(1);           // 1 for 16-bit word address
  FRAM_Write_Array(0x20, wr);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test-3");

  FRAM_Read_Array(0x20, rd, 16);
  char c1 = FRAM_Read(0x2D);
  delay(1000);
  //  i2cMaster_Disable();

  Serial.print("Print Str: ");
  Serial.println(rd);
  Serial.print("Char: ");
  Serial.println(c1);
  Serial.println();
  Serial.println("+++End Test+++");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("End");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FRAM1 = ");
  lcd.setCursor(0, 1);
  lcd.print("FRAM2 = ");

}

void loop() {
  i2cMaster_Init(FRAM_ADR_1);
  FRAM_Word_Adr(1);
  FRAM_Write(num, num);
  unsigned char c2 = FRAM_Read(num);
  //  i2cMaster_Disable();

  i2cMaster_Init(FRAM_ADR_2);
  FRAM_Word_Adr(1);
  FRAM_Write(num, num);
  unsigned char c3 = FRAM_Read(num);
  //  i2cMaster_Disable();

  Serial.print(c2);
  Serial.print("      ");
  Serial.println(c3);

  lcd.setCursor(10, 0);
  lcd.print(num);
  lcd.setCursor(10, 1);
  lcd.print(num);

  num++;
  delay(1000);


}


