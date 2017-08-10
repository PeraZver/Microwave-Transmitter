/*  Basic Microwave transmitter configuration
 *   
 *  This example configures sinthesyzer MAX2871, attenuator PE43711 and switch SKY13322. 
 *  It also reads ADC value from RSSI detector LMH2120. 
 *  It is based on individual examples of each component (see the folder).
 *  
 *  
 *  CLK is pin 13
  * DATA IN is pin 12
  * DATA OUT is pin 11
  * LE for MAX2871 is pin 5
  * CE for MAX2871 is pin 6
  * RF EN is pin 7
  * LE for PE43711 is pin 10
  * 
Pero, August 2017
 
*/
#include <SPI.h>
#include <ADC.h>
#include "MAX2871.h"
#include "MWtx.h"

// user input
char incomingChar = 0;  //Serial input

void setup() {  
  Serial.begin(9600);
  delay(3000);
  Serial.println("Microwave transmitter");
  Serial.println("Basic configuration example");  
  Serial.println("v1.0");
  Serial.println("Pero, July 2017");
  Serial.println(" ");
  Serial.println(" ");
  
  MWPins(); // Set pins for all of the components

  // initialize SPI:
  SPI.begin(); 
  Serial.println("SPI Initialized");
  
  MAX2871_Init ();
  Serial.println("MAX2871 Initialized");

  MWInit();
  Serial.println("Attentuation and filter selected.");
  Serial.println("End setup"); 
}

void loop() {
  if(Serial.available() > 0){
    incomingChar = Serial.read();
      
    switch (incomingChar){  
      case 'v':    
          adc_mode = 0b100;
          break;
      
      case 't':  
          adc_mode = 0b001;
          break;
          
      case 'e':
        MAX2871_RFA_Enable();      
        Serial.println("RFA out activated.");
        break;
  
     case 'd':
        MAX2871_RFA_Disable();
        Serial.println("RFA out deactivated.");
        break;  
          
      default:   
          ;
    }
  }
  MAX2871_Read();
  RSSI_Read();  

  delay(1000);
}
  

// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}

