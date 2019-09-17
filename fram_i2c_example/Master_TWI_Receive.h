/* 
    Master TWI Receive
    ------------------
    Header file name - "Master_TWI_Receive.h"
    Must include: "Master_TWI.h"
    
    Description:
    This header file contains I2C read operation steps and TWSR status flags responses.
    In the previous header, "Master_TWI.h", contains about I2C write operation steps
    and status flags.

    Date: 17 Sep 2019

    Written By
    Salai Aung Myint Myat

*/

#include "Master_TWI.h"

//=====================================================//
//              MASTER CONTROLLER - READ               //
//=====================================================//

//**************** TWI Check Code ********************//
// TWI Master Transmitter Codes
#define TWI_MRX_ADR_ACK     0x40  // SLA+R (Slave with Read command) is transmitted, ACK received
#define TWI_MRX_DATA_ACK    0x50  // Data byte is transmitted, ACK received
#define TWI_MRX_DATA_NACK   0x58  // SLA+R (Slave with Read command) is transmitted, NACK received


//**************** Error Status Code ******************//
#define MRX_REPEAT_not_reach     0x11
#define MRX_ADR_not_reach        0x12
#define MRX_DATA_not_reach       0x13
#define MRX_DATA_N_not_reach     0x14

#define MRX_REPEAT_dead_loop     0x15
#define MRX_ADR_dead_loop        0x16
#define MRX_DATA_dead_loop       0x17
#define MRX_DATA_N_dead_loop     0x18


/*
    TWI Transmission (Write)
    ------------------------
    1. Send START condition
    2. Send Slave Address (Write adr)
    3. Send Word Address
    4. Send REPEAT condition
    5. Send Slave Address again to read (Read adr)
    6. Receive Data (can be repeated)
    7. Receive Data NACK - end of received data
    8. Send STOP condition
*/


// 3. Send REPEAT condition
void i2cMaster_Repeat(void)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;

  TWCR = (1 << TWEN)  |    // TWI enabled
         (1 << TWINT) |    // Enable TWI interrupt flag
         (1 << TWSTA);     // Enable START bit to transmit

  // Check and wait REPEAT condition is transmitted
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      // Serial.println("Break");
      MasterTX_RX_Error = MRX_REPEAT_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for START condition
  if ((TWSR & 0xF8) != TWI_REP_START) {
    MasterTX_RX_Error = MRX_REPEAT_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
  //  Serial.println("REPEAT");
  //  Serial.println(TWSR & 0xF8, HEX);
  //  Serial.println("=====");
}



// 4. Send Slave Address
void i2cMaster_Adr_Read(unsigned char Addr)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;

  TWDR = Addr;            // Load address into TWDR register
  TWCR = (1 << TWINT) |   // Clear TWINT to start transmission
         (1 << TWEN);

  // Check and wait SLA+R is transmitted and ACK is received
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      //      Serial.println("Break");
      MasterTX_RX_Error = MRX_ADR_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for ADR_ACK
  if ((TWSR & 0xF8) != TWI_MRX_ADR_ACK) {
    MasterTX_RX_Error = MRX_ADR_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
  //  Serial.println("read adr");
}

//5. Receive Data (can be repeated)
char i2cMaster_Data_Read(void)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;

  TWCR = (1 << TWINT) |   // Clear TWINT to start transmission
         (1 << TWEN)  |
         (1 << TWEA);     // Read ACK return

  // Check and wait DATA is received and ACK is return
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      // Serial.println("Break");
      MasterTX_RX_Error = MRX_DATA_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for ADR_ACK
  if ((TWSR & 0xF8) != TWI_MRX_DATA_ACK) {
    MasterTX_RX_Error = MRX_DATA_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }

  char data = TWDR;
  //  Serial.println("Received");
  return data;
}


//6. Receive Data NACK - end of received data
void i2cMaster_Data_Read_N(void)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;

  TWCR = (1 << TWINT) |   // Clear TWINT to start transmission
         (1 << TWEN);

  // Check and wait DATA is received and ACK is return
  // Avoid while dead-loop by BREAKING after the specified time
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      MasterTX_RX_Error = MRX_DATA_N_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for ADR_ACK
  if ((TWSR & 0xF8) != TWI_MRX_DATA_NACK) {
    MasterTX_RX_Error = MRX_DATA_N_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
}

