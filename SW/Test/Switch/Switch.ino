/*
 * 4-Pole Switch SKY13322-375LF control example
 * 
 * Pero
 * July, 2017
 * 
 *  SKY13322-375LF Truth Table
 * V1 V2 V3 V4  | RFC to J1      RFC to J2       RFC to J3       RFC to J4
 * ------------------------------------------------------------------------
    1  0  0  0  | Pass           Isolation       Isolation       Isolation
    0  1  0  0  | Isolation      Pass            Isolation       Isolation
    0  0  1  0  | Isolation      Isolation       Pass            Isolation
    0  0  0  1  | Isolation      Isolation       Isolation       Pass 
 * 
 */




// Pins that control the filter
#define v1 2
#define v2 1
#define v3 3
#define v4 0

byte pin[4] = {v1, v2, v3, v4};  // array for pin selection
char incomingChar = 0;  //Serial input

void setup() {  
  
  delay(2000);
  Serial.begin(9600);
  Serial.println("SKY13322-375LF Control");  
  Serial.println("v0.0");
  Serial.println("Pero, July 2017");
  Serial.println(" ");
  Serial.println(" ");
    
  pinMode (v1, OUTPUT);
  pinMode (v2, OUTPUT);
  pinMode (v3, OUTPUT);
  pinMode (v4, OUTPUT);

}

void loop() {
  if(Serial.available() > 0) {
    incomingChar = Serial.read();    
    
    switch (incomingChar){  
      case '1':    
          Serial.println("Selected filter 1");
          SetSwitch(0b0001);
          break;
      
      case '2':  
          Serial.println("Selected filter 2");
          SetSwitch(0b0010);
          break;
  
      case '3':  
          Serial.println("Selected filter 3");
          SetSwitch(0b0100);
          break;
  
      case '4':  
          Serial.println("Selected filter 4");
          SetSwitch(0b1000);
          break;
  
      case 'r':
          Serial.println("Filters deselected!");
          SetSwitch(0b0000);
          break;
          
      default:  
          ; 
    }
  }
}

void SetSwitch(byte selection){
/* based on 4 values sets the switch of the filter */
  for (int i = 0; i < 4; i++)
      digitalWrite(pin[i], (selection & (1 << i)));
  
}

