/*  MAX2871 SPI Interface
  
  This example controls an MAX2871 synthesizer.
  
  MAX2871 serial interface contains six write-only and
  one read-only 32-bit registers. 
     
   The circuit:
  * CLK is pin 13
  * DATA OUT is pin 11
  * LE is pin 5
  * DATA IN is pin 12
  * CE is pin 6
  * RF EN is pin 7

 
Pero, June 2017
 
*/


// include the SPI library:
#include <SPI.h>

#define slaveSelectPin 5   //LE or slave select
#define chipEnablePin  4   // (CE)
#define clkPin         13  // clk pin
SPISettings MAX2871_SPISettings(1000000, MSBFIRST, SPI_MODE0);

char incomingChar = 0;  //Serial input

void setup() {
  delay(3000);
  Serial.begin(9600);
  Serial.println("MAX2871 SPI Interface");  
  Serial.println("v0.0");
  Serial.println("Pero, June 2017");
  Serial.println(" ");
  Serial.println(" ");
  // Enable chip
  pinMode(chipEnablePin, OUTPUT);
  digitalWrite(chipEnablePin, HIGH);
    
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  digitalWrite (slaveSelectPin, HIGH);
  
  // initialize SPI:
  SPI.begin(); 
  Serial.println("SPI Initialized");
  
  MAX2871_Init ();
  Serial.println("MAX2871 Initialized");
  
}

void loop() {
  if(Serial.available() > 0) 
    incomingChar = Serial.read();
  
  //incomingChar = 'r';
  switch (incomingChar){
    
    case 'r':    
      MAX2871_Set_MUX(3);  // Set read function on MUX pin
      while(1){
        MAX2871_Read();
        delay(1000);
      }
      break;
      
    default:
      MAX2871_Set_MUX(1);
      delay(2000);
      MAX2871_Set_MUX(2);
      delay(2000);
  }
}

void MAX2871_Init (){
//Upon power-up, the registers should be programmed
//twice with at least a 20ms pause between writes. The first
//write ensures that the device is enabled, and the second
//write starts the VCO selection process. Recommended to
//turn-off the outputs during this sequence and then turn-on
//the outputs using RFA_EN, RFB_EN. 

//Register programming order should be address 0x05,
//0x04, 0x03, 0x02, 0x01, and 0x00. 

 uint32_t regInitValues[6] = {0x00000005, 
                              0x60000004,
                              0x00000003,
                              0x00000002,
                              0x00000001,
                              0x0};

 // Writing 2 times
 for (int j = 0; j < 2; j++){
  for (int i = 0; i < 6; i++)   // 6 write registers
      MAX2871_SPI_tx (regInitValues[i]);  //Everything to 0.
  delay(20);
  }
      
}

void MAX2871_Read(){
  /*
    Register 0x06 can be read back through the MUX pin.
    The user must set MUX (register 5, bit 18 and register
    2, bits 28:26) = 1100. To begin the read sequence, set
    LE to logic-low, send 32 periods of CLK, and set LE to
    logic-high. While the CLK is running, the DATA pin can
    be held at logic-high or logic-low for 29 clocks, but the
    last 3 bits must be 110 to indicate register 6, then set LE
    back to logic-high after the 32nd clock. Finally, send 1
    period of the clock. The MSB of register 0x06 appears
    after the rising edge of the next clock and continues to
    shift out for the next 29 clock cycles (Figure 2). After the
    LSB of register 0x06 has been read, the user can reset
    MUX register = 0000.
   */
   uint32_t spi_data = 0;
   uint8_t spi_packet[4] = {0}; 
   signed char i;
   Serial.println("Reading data from MAX2871.");
   
   MAX2871_SPI_tx(spi_data | 6);   // "... last 3 bits must be 110 to indicate register 6." 

   SPI.beginTransaction(MAX2871_SPISettings);  // start new clock sequence. No need to change SS to low.
   for ( i = 0; i < 4; i++)   {
       spi_packet[i] = SPI.transfer(0x06);   // read from SPI shift register 4 times
   }
   SPI.endTransaction();    
  
  Serial.println("Data read:");
  for ( i = 0; i < 4; i++){
     Serial.println(spi_packet[i], HEX);
     spi_data |= spi_packet[i];  
     if ( i != 3)
         spi_data = spi_data << 8;
  }
  Serial.println(spi_data << 2, HEX);
  Serial.println("Reading done!");   
  //MAX2871_Set_MUX(0);  // Reset MUX pin
  
  
}

void MAX2871_Set_MUX(byte MUXSet){
  
/* Bits MUX[3:0] set the MUX pin.
 *  0001: VDD
 *  0010: GND
 *  1100: SPI read operation
 *  
 *  MUX[3] is bit 18 in reg5, and MUX[2:0] are bits 28:26 in reg2.
 */

 uint32_t spi_data = 0; // 32-bits for SPI consist of 29 data bits and 3 address bits.
 
 switch( MUXSet ){

    case 0:  //Set MUX to High Z
         Serial.println("setting MUX to High Z ...");
         // Set all to zero
         MAX2871_SPI_tx( spi_data | 0x02);
         MAX2871_SPI_tx( spi_data | 0x05);
         break;
         
    case 1:   // Set MUX to VDD
         Serial.println("setting MUX to VDD ...");
         spi_data |= (1 << 26);
         MAX2871_SPI_tx(spi_data | 0x02);
         break;
         
    case 2:  // Set MUX to GND
         Serial.println("setting MUX to GND ...");
         spi_data |= (1<<27);
         MAX2871_SPI_tx(spi_data | 0x02);
         break;
         
    default:  //Set MUX to "read"
         Serial.println("setting MUX to SPI read ...");
         spi_data |= (1<<18);
         MAX2871_SPI_tx(spi_data | 0x05);         
         spi_data = 0;    
         spi_data |= (1<<28);
         MAX2871_SPI_tx(spi_data | 0x02);
         
 }
}

void MAX2871_SPI_tx(uint32_t spi_data){
  /*
  This function takes 32-bit data and parses it on four 8-bit packets that Teensy transmitts. The 29 most-significant
  bits (MSBs) are data, and the three least-significant bits (LSBs) are the register address. 
   */

    signed char i = 0;
    uint8_t spi_package[4] = {0};
    // store 32-bit integer into 4 bytes
    for ( i = 0; i < 4; i++) {
       spi_package[i] = (spi_data & (0xFF << 8*i)) >> ( 8*i ) ;
       //Serial.println(spi_package[i], HEX);
    }
    
    Serial.println("Sending data: "); 
    Serial.println(spi_data, HEX);
    digitalWrite(slaveSelectPin,LOW);   
    SPI.beginTransaction(MAX2871_SPISettings);
     
    for ( i = 3; i >= 0; i--)
         SPI.transfer(spi_package[i]); 
          
    digitalWrite(slaveSelectPin,HIGH);
    SPI.endTransaction();  
    Serial.println("SPI transmission done!");
    delay(20);
}
  



