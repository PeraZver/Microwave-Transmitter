#include <Arduino.h>

/*  Basic Microwave transmitter configuration and test
 *
 *  This example configures sinthesyzer MAX2871, attenuator PE43711 and switch SKY13322.
 *  It also reads ADC value from RSSI detector LMH2120.
 *  It is based on individual examples of each component (see the folder).
 *
 *
 *  CLK is pin 13
  * DATA IN (MISO) is pin 12
  * DATA OUT (MOSI) is pin 11
  * LE (SS) for MAX2871 is pin 5
  * CE for MAX2871 is pin 4
  * RF EN is pin 7
  * LE for PE43711 is pin 10
  * PWDN for PA is pin 9
  * Filter selection is pins from 0 to 3.
  *
  * To work with it, use following commands on serial terminal:
  * g : print current register status
  * rt: read register 6 with ADC configured to measure temperature
  * rv: read register 6 with ADC configured to measure VCO voltage
  * fx: select which filter is active where x is 1,2,3 or 4
  * px: select MAX2871 output power, where x is 1,2,3 or 4
  * e : enable RF output
  * d : disable power output
  * Nxa: set integer divider where x is between 16 and 65535. 'a' is obligatory
  * Fxa: set fractional divider where x is between 1 and 4096. 'a' is obligatory
  *
  *
Pero, September 2017

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


      case 'e':
         MAX2871_RFA_Enable();
         Serial.println("\nRFA out activated.\n");
         break;

      case 'd':
         MAX2871_RFA_Disable();
         Serial.println("\nRFA out deactivated.\n");
         break;

      case 'r':
         MAX2871_Read(Serial.read());   // type t for temperature or v for VCO tuning voltage
         RSSI_Read();
         break;

      case 'g':
         Serial.println("\nMAX2871 Registers printout:");
         MAX2871_Print_Registers();
         break;

      case 'p':
          MAX2871_RFA_SelectPower(Serial.read());
          break;

      case 'f':
          SelectFilter(Serial.read());
          break;

      case 'D':
          Serial.print("Set output divider value DIVA (0-7): ");
          MAX2871_SetDIVA(Serial.read());
          break;

      case 'N':
          Serial.print("Set integer division value N: ");
          MAX2871_SetN(String2Int());
          break;

      case 'F':
          Serial.println("Fractional mode selected!");
          MAX2871_SetFracMode();
          Serial.print("Set fractional division value F: ");
          MAX2871_SetF(String2Int());
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




uint16_t String2Int(){
  /* converts stream of serial input characters to integer*/
  uint16_t number = 0;
  char a = '0';
  while (a != 'a'){
    if(Serial.available()){
      a = Serial.read();
      // Serial.print("\nRead character: ");
    //  Serial.print(a, DEC);
      if (a != 'a')
          number = number*10 + (a - 48);  // in ascii '0' is 48 dec.
    }
  }
  return number;
}
