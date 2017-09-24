#include "MWtx.h"

// Attenuation settings
char att_val = 0;   //
SPISettings PE43711_SPISettings(1000000, LSBFIRST, SPI_MODE0);

// Switch
byte pin[4] = {v1, v2, v3, v4};  // array for pin selection


ADC *adc = new ADC(); // adc object;
int value = 0;   // adc0 value
int value2 = 0;  // adc1 value



void PE43711_SPI_tx(char att){
  /*
   * This function takes 8-bit data and sends it to PE43711 via SPI.
   * The shift register must be loaded while LE is held
     LOW to prevent the attenuator value from changing
     as data is entered. The LE input should then be
     toggled HIGH and brought LOW again, latching the
     new data into the DSA.
   */

    Serial.println("Sending data to PE43711: ");
    Serial.println(att, HEX);
    digitalWrite(PE43711_SS,LOW);
    SPI.beginTransaction(PE43711_SPISettings);
    SPI.transfer(att);
    digitalWrite(PE43711_SS,HIGH);
    delay(0.001);
    digitalWrite(PE43711_SS,LOW);
    //    delay(1);
    //    digitalWrite(PE43711_SS,HIGH);  // Disable further writing
    SPI.endTransaction();
    Serial.println("SPI done!");
    delay(20);
}

void SetSwitch(byte selection){
  /* based on 4 values sets the switch of the filter */
  for (int i = 0; i < 4; i++)
      digitalWrite(pin[i], (selection & (1 << i)));
}

void MWPins(void){

  // set the slave select pin as an output:
  pinMode (PE43711_SS, OUTPUT);
  digitalWrite (PE43711_SS, HIGH);

  // set switch selection pins
  pinMode (v1, OUTPUT);
  pinMode (v2, OUTPUT);
  pinMode (v3, OUTPUT);
  pinMode (v4, OUTPUT);

  //Set power amplifier PWDN low
  pinMode(PWDN, OUTPUT);
  digitalWrite(PWDN, LOW);

  // ADC pins as inputs
  pinMode(readPin, INPUT);
  pinMode(readPin2, INPUT);
}

void SelectFilter(char switchSelect){
  /* Select filter based on input char */
  switch (switchSelect){
    case '1':
      Serial.println("Selected filter 1.");
      SetSwitch(1);
      break;

    case '2':
      Serial.println("Selected filter 2.");
      SetSwitch(2);
      break;

    case '3':
      Serial.println("Selected filter 3.");
      SetSwitch(3);
      break;

    case '4':
      Serial.println("Selected filter 4.");
      SetSwitch(4);
      break;

    default:
      ;
  }
}

void MWInit(void){
  /* This function initializes settings for attenuator, filter and ADC.
   * Attenuator set  to zero value (0 dB) and filter 3 is selected (6 GHz cutoff).
   * Teensy 3.2 has two ADCs while LC only one.
   */

  PE43711_SPI_tx(att_val);
  Serial.print("Attentuation set: ");
  Serial.print((float)att_val/4);
  Serial.print(" dB\n");
  SetSwitch(0b0100);
  Serial.println("Selected filter 3");

  ///// ADC0 ////
	// reference can be ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
	//adc->setReference(ADC_REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

	adc->setAveraging(32); // set number of averages
	adc->setResolution(12); // set bits of resolution

	// it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
	// see the documentation for more information
	adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed
	// it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
	adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

	//adc->enableInterrupts(ADC_0);

	// always call the compare functions after changing the resolution!
	//adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
	//adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

	////// ADC1 /////
	#if ADC_NUM_ADCS>1
	adc->setAveraging(32, ADC_1); // set number of averages
	adc->setResolution(16, ADC_1); // set bits of resolution
	adc->setConversionSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the conversion speed
	adc->setSamplingSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the sampling speed

	// always call the compare functions after changing the resolution!
	//adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
	//adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V
	#endif
  	Serial.println("ADC initialized");
}

void RSSI_Read(void){
	  //RSSI readout
  value = adc->analogRead(readPin); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

  Serial.print("RSSI A: ");
  Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);

  #if ADC_NUM_ADCS>1
  value2 = adc->analogRead(readPin2, ADC_1);

  Serial.print("Pin: ");
  Serial.print(readPin2);
  Serial.print(", value ADC1: ");
  Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
  #endif


    /* fail_flag contains all possible errors,
        They are defined in  ADC_Module.h as

        ADC_ERROR_OTHER
        ADC_ERROR_CALIB
        ADC_ERROR_WRONG_PIN
        ADC_ERROR_ANALOG_READ
        ADC_ERROR_COMPARISON
        ADC_ERROR_ANALOG_DIFF_READ
        ADC_ERROR_CONT
        ADC_ERROR_CONT_DIFF
        ADC_ERROR_WRONG_ADC
        ADC_ERROR_SYNCH

        You can compare the value of the flag with those masks to know what's the error.
    */

    if(adc->adc0->fail_flag) {
        Serial.print("ADC0 error flags: 0x");
        Serial.println(adc->adc0->fail_flag, HEX);
        if(adc->adc0->fail_flag == ADC_ERROR_COMPARISON) {
            adc->adc0->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
            Serial.println("Comparison error in ADC0");
        }
    }
    #if ADC_NUM_ADCS>1
    if(adc->adc1->fail_flag) {
        Serial.print("ADC1 error flags: 0x");
        Serial.println(adc->adc1->fail_flag, HEX);
        if(adc->adc1->fail_flag == ADC_ERROR_COMPARISON) {
            adc->adc1->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
            Serial.println("Comparison error in ADC1");
        }
    }
    #endif

}
