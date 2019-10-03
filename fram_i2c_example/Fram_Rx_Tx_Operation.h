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

    UPDATED: Support 8-bit and 16-bit word addresses!

    NOTES: FRAM_Word_Adr(n) is needed to declare word-address bits.
             n = 0 -> 8-bit word address (Default)
             n = 1 -> 16-bit word address

    Date: 30 Sep 2019

    Written By
    Salai Aung Myint Myat
*/

#include "Master_TWI_Receive.h"

bool Word_Adr_Type = 0;                // '0', Default = 8-bit, '1' = 16-bit

void FRAM_Word_Adr(bool adr_type) {
  Word_Adr_Type = adr_type;
}

void FRAM_Write(uint16_t word_adr, uint8_t data) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // Byte adr shifting
  uint8_t H_adr = (uint8_t)(word_adr >> 8);
  uint8_t L_adr = (uint8_t)(word_adr & 0xFF);

  // FRAM Write Operation
  i2cMaster_Start();
  i2cMaster_Adr_Write(SLA_WR);
  if (Word_Adr_Type == 1) {
    i2cMaster_Data_Write(H_adr);
  }
  i2cMaster_Data_Write(L_adr);
  i2cMaster_Data_Write(data);
  i2cMaster_Stop();
}

void FRAM_Write_Array(uint16_t word_adr, char* temp) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // Byte adr shifting
  uint8_t H_adr = (uint8_t)(word_adr >> 8);
  uint8_t L_adr = (uint8_t)(word_adr & 0xFF);

  // FRAM Write Operation with array
  // Set array pointer
  uint8_t i = 0;

  // FRAM Write Operation
  i2cMaster_Start();
  i2cMaster_Adr_Write(SLA_WR);
  if (Word_Adr_Type == 1) {
    i2cMaster_Data_Write(H_adr);
  }
  i2cMaster_Data_Write(L_adr);
  // Start reading data
  while (temp[i] != '\0') {
    i2cMaster_Data_Write(temp[i]);
    i++;
  }
  i2cMaster_Stop();
}

char FRAM_Read(uint16_t word_adr) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // Byte adr shifting
  uint8_t H_adr = (uint8_t)(word_adr >> 8);
  uint8_t L_adr = (uint8_t)(word_adr & 0xFF);

  // FRAM Read Operation
  // Select word-address location
  i2cMaster_Start();
  i2cMaster_Adr_Write(SLA_WR);    // Write slave address
  if (Word_Adr_Type == 1) {
    i2cMaster_Data_Write(H_adr);
  }
  i2cMaster_Data_Write(L_adr);

  // Read data from current word-address
  i2cMaster_Repeat();
  i2cMaster_Adr_Read(SLA_RD);         // Read slave address
  char data = i2cMaster_Data_Read();  // Start reading data
  i2cMaster_Data_Read_N();            // Acknowledge that Master will stop read data
  i2cMaster_Stop();

  return data;
}


char FRAM_Read_Array(uint16_t word_adr, char* temp, uint8_t I2C_BUFFER_SIZE) {
  // Wait with time shifting method
  Current_Sec = Ref_Sec;
  while (!(Ref_Sec - Current_Sec > I2C_Shift_Sec)) {}

  // Byte adr shifting
  uint8_t H_adr = (uint8_t)(word_adr >> 8);
  uint8_t L_adr = (uint8_t)(word_adr & 0xFF);

  // FRAM Read Operation with array
  // Set array pointer
  uint8_t i = 0;

  // Select word-address location
  i2cMaster_Start();
  i2cMaster_Adr_Write(SLA_WR);    // Write slave address
  if (Word_Adr_Type == 1) {
    i2cMaster_Data_Write(H_adr);
  }
  i2cMaster_Data_Write(L_adr);

  // Read data from current word-address
  i2cMaster_Repeat();
  i2cMaster_Adr_Read(SLA_RD);     // Read slave address
  // Start reading data
  for (i = 0; i < I2C_BUFFER_SIZE; i++) {
    temp[i] = i2cMaster_Data_Read();
  }
  i2cMaster_Data_Read_N();        // Acknowledge that Master will stop read data
  i2cMaster_Stop();

  temp[i] = '\0';
  return temp;
}

