/* Frequencies and Baudrate Definitions
   ------------------------------------
   MCU = 16 MHz
   Prescaler = 1
   SCL freq tested = 100 kHz, 400 kHz
   SCL freq seleced = 100 kHz

    Formula (ref. from ATmega328p datasheet)
    ***********************************************************
       TWBR Baudrate = [(F_CPU/SCL_Freq) - (16-Prescaler)]/2
    ***********************************************************

  Notes: I2C pinouts in ATmega2560(Arduino Mega Board),
          SDA = PD1 (digital pin 20)
          SCL = PD0 (digital pin 21)

  Date: 17 Sep 2019

  Written by
  Salai Aung Myint Myat
*/

#define F_CPU   16000000UL  // 16 MHz
#define SCL_FREQ  100000    // 100 kHz
#define TWPS_PRESCALER  1   // Set prescaler to 1, 4^TWPS = 4^0 = 1
#define TWBR_BAUD   ((F_CPU/SCL_FREQ)-(16-TWPS_PRESCALER))/2

//=====================================================//
//              MASTER CONTROLLER - WRITE              //
//=====================================================//

//**************** TWI Check Code ********************//
// General TWI Master Codes
#define TWI_START         0x08  // START condition is transmitted
#define TWI_REP_START     0x10  // Repeated START condition is transmitted

// TWI Master Transmitter Codes
#define TWI_MTX_ADR_ACK   0x18  // SLA+W (Slave with Write command) is transmitted, ACK received
#define TWI_MTX_DATA_ACK  0x28  // Data byte is transmitted, ACK received


//**************** Error Status Code ******************//
#define MTX_START_not_reach   0x01
#define MTX_ADR_not_reach     0x02
#define MTX_DATA_not_reach    0x03

#define MTX_START_dead_loop   0x04
#define MTX_ADR_dead_loop     0x05
#define MTX_DATA_dead_loop    0x06
#define MTX_STOP_dead_loop    0x07
volatile byte MasterTX_RX_Error = 0;
//volatile byte Error = 0;

//**************** Dead Loop Prevention ******************//
#define Ref_Sec           millis()
#define Wait_Sec          1         // 1 milli second
#define I2C_Shift_Sec     10        // 10 milli seconds for time shift waiting
unsigned long Current_Sec = 0;      // Manipulate current second with reference second

//**************** Slave Adr Convertion ******************//
#define RW_BIT            0         // Bit 0 at slave address for R/W operation
uint8_t SLA_WR = 0;                 // Write address
uint8_t SLA_RD = 0;                 // Read address

// Error detection functions
void MTX_RX_ERROR(void);


// Master device initialization
void i2cMaster_Init(uint8_t SLA)
{
  // Pull-up to SCL and SDA bus lines
  // Otherwise, use 1 kOhm resistor
  // Comment this if ATmega2560 (Mega board) is used
  DDRC &= ~((1 << DDC4) | (1 << DDC5));       // Set as input direction
  PORTC |= (1 << PC4) | (1 << PC5);           // Set pull-up resistor

  // Uncomment this if ATmega2560 (Mega board) is used
  //  DDRD &= ~((1 << DDD1) | (1 << DDD0));   // Set as input direction
  //  PORTD |= (1 << PD1) | (1 << PD0);       // Set pull-up resistor

  TWBR = TWBR_BAUD;   // Set baudrate by calculation from Datasheet
  TWCR = (1 << TWEN); // TWI enabled
  TWSR = 0;           // Set prescaler to 1

  // Slave address convertion
  uint8_t SlaveAdr = (SLA << 1);        // Convert slave address (7 to 8 bits)
  SlaveAdr &= ~(1 << RW_BIT);           // 0 - Write operation enabled
  SLA_WR = SlaveAdr;
  SlaveAdr |= (1 << RW_BIT);            // 1 - Read operation enabled
  SLA_RD = SlaveAdr;

  //  Serial.println(SLA, HEX);
  //  Serial.println(SLA_WR, HEX);
  //  Serial.println(SLA_RD, HEX);
}


// Master device disable
void i2cMaster_Disable(void)
{
  // Pull-down to SCL and SDA bus lines
  // Comment this if ATmega2560 (Mega board) is used
  DDRC |= (1 << DDC4) | (1 << DDC5);         // Set as output direction
  PORTC &= ~((1 << PC4) | (1 << PC5));       // Set low logic

  // Uncomment this if ATmega2560 (Mega board) is used
  //  DDRD |= (1 << DDD1) | (1 << DDD0);     // Set as input direction
  //  PORTD &= ~((1 << PD1) | (1 << PD0));   // Set low logic

  TWCR = 0;            // Clear all bits in control register
  TWBR = 0;            // Clear also buadrate
  
  SLA_WR = 0;          // Clear write address
  SLA_RD = 0;          // Clear read address
}

/*
    TWI Transmission
    ----------------
    1. Send START condition
    2. Send Slave Address
    3. Send Send Word Address
    4. Send Data
    5. Send STOP condition
*/

// 1. Send START condition
void i2cMaster_Start(void)
{
  TWCR = (1 << TWEN)  |    // TWI enabled
         (1 << TWINT) |    // Enable TWI interrupt
         (1 << TWSTA);     // Enable START bit to transmit

  // Check and wait START condition is transmitted
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      MasterTX_RX_Error = MTX_START_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for START condition
  if ((TWSR & 0xF8) != TWI_START) {
    MasterTX_RX_Error = MTX_START_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
}

// 2. Send Slave Address
void i2cMaster_Adr_Write(unsigned char Addr)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;

  TWDR = Addr;            // Load address into TWDR register
  TWCR = (1 << TWINT) |   // Clear TWINT to start transmission
         (1 << TWEN);

  // Check and wait SLA+W is transmitted and ACK is received
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      MasterTX_RX_Error = MTX_ADR_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for ADR_ACK
  if ((TWSR & 0xF8) != TWI_MTX_ADR_ACK) {
    MasterTX_RX_Error = MTX_ADR_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
}

// 3. Send data to slave
void i2cMaster_Data_Write(unsigned char Data)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) return 0;
  //  Serial.println("Next");

  TWDR = Data;            // Load data into TWDR register
  TWCR = (1 << TWINT) |   // Clear TWINT to start transmission
         (1 << TWEN);

  // Check and wait DATA is transmitted and ACK is received
  // Avoid while dead-loop by BREAKING after the specified time
  //---------------------------------------------------------------//
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWINT)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      MasterTX_RX_Error = MTX_DATA_dead_loop;
      MTX_RX_ERROR();
      return 0;
    }
  }
  //---------------------------------------------------------------//

  // Check code and error detection for ADR_ACK
  if ((TWSR & 0xF8) != TWI_MTX_DATA_ACK) {
    MasterTX_RX_Error = MTX_DATA_not_reach;
    MTX_RX_ERROR();
    return 0;
  }
  else {
    MasterTX_RX_Error = 0;     // No error
  }
}

// 4. Send STOP condition
void i2cMaster_Stop(void)
{
  /*** If there is error code, then out of the loop ***/
  if (MasterTX_RX_Error > 0) {
    MasterTX_RX_Error = 0;   // Clear error code for resending data
    // reset TWCR register
    TWCR = 0;
    TWCR = (1 << TWEN); // TWI enabled
    return 0;
  }

  TWCR = (1 << TWINT) | (1 << TWEN) |
         (1 << TWSTO);  // Enable STOP bit

  // Check and wait STOP bit is enable
  // Avoid while dead-loop by BREAKING after the specified time
  Current_Sec = Ref_Sec;
  while (!(TWCR & (1 << TWSTO)))
  {
    if (Ref_Sec - Current_Sec > Wait_Sec)
    { // If wait condition exceeded, then break this while loop
      MasterTX_RX_Error = MTX_STOP_dead_loop;
      MTX_RX_ERROR();
      MasterTX_RX_Error = 0;   // Clear error code for resending data
      // reset TWCR register
      TWCR = 0;
      TWCR = (1 << TWEN); // TWI enabled
      return 0;
    }
  }

  //  Error = 0;    // Clear out error
}


// Error detection function
void MTX_RX_ERROR(void)
{
  // Printout error bit and suggestion for troubleshooting
  //  Serial.println("------");
  //  Serial.print("Error bit: ");
  //  Serial.println(MasterTX_RX_Error, HEX);
  //  Serial.print("Status: ");
  //  Serial.println(TWSR & 0xF8, HEX);
  //  Serial.println();
  //  Error = MasterTX_RX_Error;
}

