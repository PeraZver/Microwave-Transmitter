#ifndef MWTX_H
#define MWTX_H

#include <SPI.h>
#include <ADC.h>
// Attenuator settings
#define PE43711_SS 10  //LE or slave select
// Pins that control the filter
#define v1 2
#define v2 1
#define v3 3
#define v4 0


extern SPISettings PE43711_SPISettings;
extern char att_val;   //


// switch settings
extern byte pin[4];  // array for pin selection


// ADC settings
const int readPin = A0; // ADC0
const int readPin2 = A1; // ADC1

extern ADC *adc; // adc object;
extern int value;
extern int value2;

void PE43711_SPI_tx(char att);
void SetSwitch(byte selection);
void MWPins();
void MWInit();
void RSSI_Read();
#endif
