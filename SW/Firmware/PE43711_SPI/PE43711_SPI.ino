/*  PE43711 SPI Interface
  
  This example controls an PE43711 attentuator over SPI.  
     
   The circuit:
  * CLK is pin 13
  * DATA OUT is pin 11
  * LE is pin 10
 
Pero, August 2017
 
*/


// include the SPI library:
#include <SPI.h>

#define slaveSelectPin 10  //LE or slave select
#define clkPin         13  // clk pin
SPISettings PE43711_SPISettings(1000000, LSBFIRST, SPI_MODE0);

char incomingChar[2] = {0};  //Serial input
char att_val = 0;   //
char i = 0;

void setup() {
  delay(3000);
  Serial.begin(9600);
  Serial.println("PE43711 SPI Interface");  
  Serial.println("v0.0");
  Serial.println("Pero, August 2017");
  Serial.println(" ");
  Serial.println(" ");
    
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  digitalWrite (slaveSelectPin, HIGH);
  
  // initialize SPI:
  SPI.begin(); 
  Serial.println("SPI Initialized");  
}

void loop() {

    for (i = 0; i < 2; i++){
      while(Serial.available() == 0) ;
      incomingChar[i] = Serial.read() - 48;  // serial reads chars.
    }

    if (i){    
        att_val = incomingChar[0]*10 + incomingChar[1];    
        PE43711_SPI_tx(att_val);     
        Serial.print("Attentuation set: ");
        Serial.print((float)att_val/4);
        Serial.print(" dB\n");
        i = 0;
    }
}

void PE43711_SPI_tx(char att){
  /*
   * This function takes 8-bit data and sends it to PE43711 via SPI. 
   * The shift register must be loaded while LE is held
     LOW to prevent the attenuator value from changing
     as data is entered. The LE input should then be
     toggled HIGH and brought LOW again, latching the
     new data into the DSA. 
   */
    
    Serial.println("Sending data: "); 
    Serial.println(att, HEX);
    digitalWrite(slaveSelectPin,LOW);   
    SPI.beginTransaction(PE43711_SPISettings);
    SPI.transfer(att);           
    digitalWrite(slaveSelectPin,HIGH);
    delay(0.001);
    digitalWrite(slaveSelectPin,LOW);
    SPI.endTransaction();  
    Serial.println("SPI transmission done!");
    delay(20);
}
  



