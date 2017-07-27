/*  MAX2871 Temperature and VCO tuning voltage
  
  This example reads out the temperature or VCO tuning voltage from MAX2871 via SPI.
  
  MAX2871 serial interface contains six write-only and
  one read-only 32-bit registers. 
     
   The circuit with Teensy 3.2 (or LC):
  * CLK is pin 13
  * DATA OUT is pin 11
  * LE is pin 5
  * DATA IN is pin 12
  * CE is pin 6
  * RF EN is pin 7

 
Pero, July 2017
 
*/
#include <SPI.h>
#include "MAX2871.h"

char incomingChar = 0;  //Serial input

void setup() {
  delay(3000);
  Serial.begin(9600);
  Serial.println("MAX2871 Temperature readout");  
  Serial.println("v1.0");
  Serial.println("Pero, July 2017");
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
    
  switch (incomingChar){  
    case 'v':    
        adc_mode = 0b100;
        break;
    
    case 't':  
        adc_mode = 0b001;
        break;
        
    default:   
        ;
  }
  MAX2871_Read();
  delay(1000);
}

