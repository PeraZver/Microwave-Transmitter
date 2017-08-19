#include <Arduino.h>

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
  while (!Serial.available()) ;
  Serial.println("Microwave transmitter");
  Serial.println("Basic configuration example");
  Serial.println("v1.1");
  Serial.println("Pero, July 2017");
  Serial.println(" ");
  Serial.println(" ");

  MWPins(); // Set pins for all components

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
         Serial.println("\nRFA out activated.\n");
         break;

      case 'd':
         MAX2871_RFA_Disable();
         Serial.println("\nRFA out deactivated.\n");
         break;

      case 'r':
         MAX2871_Read();
         RSSI_Read();
         break;

      case 'g':
         Serial.println("\nMAX2871 Registers printout:");
         MAX2871_Print_Registers();
         break;

     case 'q':
         Serial.println("\nSetting power to -4 dBm");
         MAX2871_RFA_Power(0);
         break;

     case 'w':
         Serial.println("\nSetting power to -1 dBm");
         MAX2871_RFA_Power(1);
         break;

     case 'a':
         Serial.println("\nSetting power to 2 dBm");
         MAX2871_RFA_Power(2);
         break;

     case 's':
         Serial.println("\nSetting power to 5 dBm");
         MAX2871_RFA_Power(3);
         break;

      case '1':
        Serial.println("\nSelected filter 1.\n");
        SetSwitch(1);
        break;

      case '2':
        Serial.println("\nSelected filter 2.\n");
        SetSwitch(2);
        break;

      case '3':
        Serial.println("\nSelected filter 3.\n");
        SetSwitch(3);
        break;

      case '4':
        Serial.println("\nSelected filter 4.\n");
        SetSwitch(4);
        break;

      default:
          ;
    }
  }

  delay(1000);
}


// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}
