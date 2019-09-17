/*
    FRAM Read/Write Operation - Driver File
    ---------------------------------------
    Header file name - "Fram_Rx_Tx_Operation.h"
    Must include: "Master_TWI_Receive.h"
                  (already included "Master_TWI.h" header)

    Description:
    This header file contains I2C read and write operation functions
    To avoid duplicated operations, time shifting method applied to while loop
    instead of using delay.

    Date: 17 Sep 2019

    Written By
    Salai Aung Myint Myat
*/

#include "Master_TWI_Receive.h"

void FRAM_Write(uint8_t word_adr, uint8_t data) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // FRAM Write Operation
  i2cMaster_Start();
  i2cMaster_Adr_Write(0xA0);
  i2cMaster_Data_Write(word_adr);
  i2cMaster_Data_Write(data);
  i2cMaster_Stop();
}

char FRAM_Read(uint8_t word_adr) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // FRAM Read Operation
  // Select word-address location
  i2cMaster_Start();
  i2cMaster_Adr_Write(0xA0);    // Write slave address
  i2cMaster_Data_Write(word_adr);

  // Read data from current word-address
  i2cMaster_Repeat();
  i2cMaster_Adr_Read(0xA1);           // Read slave address
  char data = i2cMaster_Data_Read();  // Start reading data
  i2cMaster_Data_Read_N();            // Acknowledge that Master will stop read data
  i2cMaster_Stop();

  return data;
}


char FRAM_Read_Array(uint8_t word_adr, uint8_t I2C_BUFFER_SIZE, char* temp) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // FRAM Read Operation with array
  // Set array pointer
  uint8_t i;

  // Select word-address location
  i2cMaster_Start();
  i2cMaster_Adr_Write(0xA0);    // Write slave address
  i2cMaster_Data_Write(word_adr);

  // Read data from current word-address
  i2cMaster_Repeat();
  i2cMaster_Adr_Read(0xA1);     // Read slave address
  // Start reading data
  for (i = 0; i < I2C_BUFFER_SIZE; i++) {
    temp[i] = i2cMaster_Data_Read();
  }
  i2cMaster_Data_Read_N();      // Acknowledge that Master will stop read data
  i2cMaster_Stop();

  temp[i] = '\0';
  return temp;
}

