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

//Setting temperature read and MUX readout
uint32_t regInitValues[6] = { 0x0,
                              0x00000001,   //
                              0x10004002,   // MUX[2:0] = 100, R = 1
                              0x00000603,   // Clock DIV = 192 
                              0x60000004,   // bits 31:29 reserved. Program to 011
                              0x0004004D }; // MUX[3] = 1, ADCM = 001, ADCS = 1

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
      while(1){
        MAX2871_Read();
        delay(1000);
      }
      break;
      
    default:
        MAX2871_Temp();
        MAX2871_Read();
        delay(1000);
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

 // Writing 2 times
 for (int j = 0; j < 2; j++){
  for (int i = 5; i >= 0; i--)   // 6 write registers
      MAX2871_SPI_tx (regInitValues[i]);  
  delay(20);
  }
      
}

void MAX2871_Read(){

   uint32_t spi_data = 0;
   uint32_t ADC_Mask = 0x007F0000;
   uint8_t spi_packet[4] = {0}; 
   uint8_t ADC_temperature = 0;
   float temperature = 0;
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

  ADC_temperature = ( spi_data & ADC_Mask ) >> 16;
  temperature = 95 - 1.14*ADC_temperature;

  Serial.print("\nTemp data: ");
  Serial.print(ADC_temperature);
  Serial.print("\nTemperature: ");
  Serial.print(temperature);
  Serial.println("");
  
  
  
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
  

void MAX2871_Temp(void)
{
   MAX2871_SPI_tx(0x0004004D);
   MAX2871_SPI_tx(0x00000603);
   delay(1);
}

