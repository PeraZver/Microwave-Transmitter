#include "MAX2871.h"

//Setting temperature read and MUX readout
uint32_t regInitValues[6] = { EN_INT | N_DIV | REG_0,
                              REG_1,   
                              R_DIV | REG_2,   // MUX[2:0] = 100, R = 1
                              REG_3,   // Clock DIV = 192 
                              REG4HEAD | REG_4,   // bits 31:29 reserved. Program to 011
                              REG_5 }; // MUX[3] = 1, ADCM = 001, ADCS = 1

uint32_t MAX2871_Registers[6] = {0};
// ADC selection varible
byte adc_mode = 0b001;  // 001 for temperature, 100 for V tune.

SPISettings MAX2871_SPISettings(1000000, MSBFIRST, SPI_MODE0);



void MAX2871_Init (){
//Upon power-up, the registers should be programmed
//twice with at least a 20ms pause between writes. 
//Register programming order should be address 0x05,
//0x04, 0x03, 0x02, 0x01, and 0x00. 

// Writing 2 times
 for (int j = 0; j < 2; j++){
  for (int i = 5; i >= 0; i--){   // 6 write registers
      MAX2871_SPI_tx (regInitValues[i]);  
      MAX2871_Registers[i] = regInitValues[i];    // write init values into working registers
  }
  delay(20);
  }
 
  MAX2871_SPI_Init();   
}

void MAX2871_Read(){
  /*
   * Reading register 6 over SPI. Register contains data from ADC and VCO selection.
   * ADC can read either temperature or VCO tuning voltage. 
   */

   uint32_t spi_data = 0;
   uint8_t spi_packet[4] = {0};   
   uint8_t ADC_data = 0;
   float data = 0;
   signed char i;
 
   MAX2871_ADC_Init();   
   
   Serial.println("");   
   Serial.println("Reading data from MAX2871 ...");   
   MAX2871_SPI_tx(spi_data | 6);   // "... last 3 bits must be 110 to indicate register 6." 

   SPI.beginTransaction(MAX2871_SPISettings);  // start new clock sequence. No need to change SS to low.
   for ( i = 0; i < 4; i++)   {
       spi_packet[i] = SPI.transfer(0x00);   // read from SPI shift register 4 times
   }
   SPI.endTransaction();  
   Serial.println("Data read:");
   for ( i = 0; i < 4; i++){
     //Serial.println(spi_packet[i], HEX);
     spi_data |= spi_packet[i];  
     if ( i != 3)
         spi_data = spi_data << 8;
   }  
   spi_data = spi_data << 2; //one clk period delay  
   Serial.println(spi_data, HEX);
   Serial.println("Reading done!"); 
   Serial.println("");
   
   if (spi_data & ADCV)
     Serial.println("ADC data valid!"); 
  else
    Serial.println("ADC data invalid!");    
  ADC_data = ( spi_data & ADC_mask ) >> 16;      
  Serial.print("ADC data: ");
  Serial.print(ADC_data);
  
  if (adc_mode == 1){
      data = 95 - 1.14*ADC_data;
      Serial.print("\nTemperature: ");
  }
  else{
      data = 0.315 + 0.0165*ADC_data;
      Serial.print("\nVCO voltage: ");
  }

  Serial.print(data);
  Serial.println("");

  if (spi_data & VASA)
    Serial.println("VCO Autoselect searching"); 
  else
    Serial.println("VAS Completed"); 
  
  Serial.print("Current VCO: ");
  Serial.print((spi_data & V) >> 3);
  Serial.println("");

  MAX2871_ADC_Reset();
}

void MAX2871_SPI_tx(uint32_t spi_data){
  /*
   * This function takes 32-bit data and parses it on four 8-bit packets that Teensy transmitts. The 29 most-significant
   * bits (MSBs) are data, and the three least-significant bits (LSBs) are the register address. 
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
  
void MAX2871_ADC_Init(void){
/*
 * Initialize ADC in MAX2871 for temperature or tune voltage readout. Must be
 * sequentially written, otherwise it won't work. 
 */
   byte ADCM;
   ADCM = adc_mode << 3;
 
   MAX2871_Registers[3] |= CDIV;
   MAX2871_SPI_tx(MAX2871_Registers[3]);
   MAX2871_Registers[5] |= ADCM;
   MAX2871_SPI_tx(MAX2871_Registers[5]);
   MAX2871_Registers[5] |= ADCS;
   MAX2871_SPI_tx(MAX2871_Registers[5]);
   delay(0.1);
}

void MAX2871_ADC_Reset(void){
/*
 * Apparently ADC won't work if ADCS is constantly on. For that reason, I'll need to reset it after every read.
 */
 MAX2871_Registers[5] &= ~(111 << 3);
 MAX2871_Registers[5] &= ~(ADCS);
 MAX2871_SPI_tx(MAX2871_Registers[5]);
 
}

void MAX2871_SPI_Init(){
  /* Set MUX bits to enable reg6 readout over SPI.
   *  
   */
   MAX2871_Registers[5] |= MUX_5;
   MAX2871_SPI_tx(MAX2871_Registers[5]);
   MAX2871_Registers[2] |= MUX_2;
   MAX2871_SPI_tx(MAX2871_Registers[2]);
   
}
