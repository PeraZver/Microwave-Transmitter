/* Register viewer 

This small program will display how registers are set.
Compile with simple GCC. 

Pero, Aug 2017

*/


#include <stdio.h>
#include <stdint.h>
#include "MAX2871.h"

void reg0();
void reg1();
void reg2();
void reg3();
void reg4();
void reg5();

void main(){
	 
	 reg0();
	 reg1();
	 reg2();
	 reg3();
	 reg4();
	 reg5();

	//getc(0);

}

void reg0(void){
	uint32_t reg0 = 0;
	printf("\ninput reg0: ");
	if(scanf("%x", &reg0)){
		printf("INT enable: %u \n", (reg0 & EN_INT) != 0 );
		printf("N: %u \n", (reg0 & (0xFFFF << 15)) >> 15);
		printf("F: %u \n", (reg0 & (0xFFF << 3)) >> 3 );	
	}
}

void reg1(void){
	uint32_t reg1 = 0;
	printf("\ninput reg1: ");	
	if(scanf("%x", &reg1)){
		printf("CPL: %u \n",(reg1 & (0b11 << 29)) >> 29);
		printf("CPT: %u \n",(reg1 & (0b11 << 27)) >> 27);
		printf("P: %u \n", (reg1 & (0xFFF << 15)) >> 15);
		printf("M: %u \n", (reg1 & (0xFFF << 3)) >> 3 );	
	}
}

void reg2(void){
	uint32_t reg2 = 0;
	printf("\ninput reg2: ");	
	if(scanf("%x", &reg2)){
		printf("LDS: %u \n", (reg2 & LDS) != 0 );
		printf("DBR: %u \n", (reg2 & DBR) != 0);
		printf("RDIV2: %u \n", (reg2 & RDIV2) != 0);
		printf("R: %u \n", (reg2 & (0x3FF << 14)) >> 14);
		printf("LDF: %u \n", (reg2 & LDF) != 0);
	}
}

void reg3(void){
	uint32_t reg3 = 0;
	printf("\ninput reg3: ");	
	if(scanf("%x", &reg3)){
		printf("VCO: %u \n", (reg3 & (0x3F << 26)) >> 26 );
		printf("CDIV: %u \n", (reg3 & (0xFFF << 3)) >> 3);
	}
}

void reg4(void){
	uint32_t reg4 = 0;
	printf("\ninput reg4: ");	
	if(scanf("%x", &reg4)){
		printf("DIVA: %u \n", (reg4 & (0b111 << 20)) >> 20 );
		printf("FB: %u \n", (reg4 & FB) != 0 );
		printf("BS: %u \n", ((reg4 & (0xFF << 12)) >> 12) + ((reg4 & (0b11 << 24)) >> 16)); // MSBs are on bits 25:24 while lower 8 bits are on 19:12
		printf("BPWR: %u \n", (reg4 & (0b11 << 6)) >> 6);
		printf("APWR: %u \n", (reg4 & (0b11 << 3)) >> 3);
		printf("RFA_EN: %u \n", (reg4 & RFA_EN) != 0 );
		printf("RFB_EN: %u \n", (reg4 & RFB_EN) != 0 );
	}
}

void reg5(void){
	uint32_t reg5 = 0;
	printf("\ninput reg5: ");	
	if(scanf("%x", &reg5)){
		printf("LD: %u \n", (reg5 & (0b11 << 22)) >> 22 );
	}
}