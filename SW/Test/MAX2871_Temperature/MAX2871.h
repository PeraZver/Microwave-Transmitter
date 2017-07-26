#ifndef MAX2871_H
#define MAX2871_H

// this parameters should be changed to suit the application
#define N 100  // (16 bit feedback divider factor in integer mode
#define F 0  // 12 bit  fractional divider
#define M 0  // 12 bit modulus value in fractional mode
#define R 1  // 10 bit ref. frequency divider value
#define CDIV_VAL 192 // 12 bit clock divide value (since fPFD = 19.2 MHz, 19.2MHz/100kHz = 192
#define B_POWER 0b00 // Sets RFOUTB single-ended output power: 00 = -4dBm, 01 = -1dBm, 10 = +2dBm, 11 = +5dBm
#define A_POWER 0b00 // Sets RFOUTA single-ended output power: 00 = -4dBm, 01 = -1dBm, 10 = +2dBm, 11 = +5dBm
// less likely to get changed
#define MUX 0b1100
#define MUX_MSB (MUX >> 3) // MSB of MUX
#define MUX_LSB (MUX & ~(1 << 3)) // lower 3 bits of MUX
#define CPL_MODE 0b00  //CPL linearity mode: 0b00in integer mode, 0b01 10% in frac mode, 0b10 for 20%, and 0b11 for 30%
#define CPT_MODE 0b00  // 00 normal mode, 01 long reset, 10 force into source, 11 force into sink
#define CP_CURRENT 0x0 // 4 bit CP current in mA
#define P 0x0     // 12-bit phase value for adjustment
#define SD  0b00 //sigma delta noise mode: 0b00 low noise mode, 0b01 res, 0b10 low spur 1 mode, 0b11 low spur 2 mode
#define VCO 0x0 // 6 bit VCO selection
#define CDIV_MODE 0b00 // clock divide mode: 0b00 mute until lock delay, 01 fast lock enable, 10 phase adjustment, 11 reserved
#define DIVA_VAL 0b000 //Sets RFOUT_ output divider mode
#define BS 384
#define BS_MSB_VAL (BS >> 8) // 2 MSBs of Band select clock divider  
#define BS_LSB_VAL (BS & ~(11 << 8))  // 8 LSBs of band select clock divider
#define VAS_DLY_VAL 0b00 // VCO Autoselect Delay:  11 when VAS_TEMP=1, 00 when VAS_TEMP=0
#define LD_VAL 0b00 //  lock-detect pin function: 00 = Low, 01 = Digital lock detect, 10 = Analog lock detect, 11 = High
#define ADC_MODE 0b001 // ADC mode: 001 temperature, 100 tune pin, 

// register 0 masks
#define EN_INT 1 << 31    // enables integer mode
#define N_DIV N << 15     // puts value N on its place
#define F_DIV F << 3
#define REG_0 0b000

// register 1 masks 
#define CPL CPL_MODE << 29  // Sets CP linearity mode
#define CPT CPT_MODE << 27 // Sets CP test mode
#define PHASE P << 15 // Sets phase adjustment
#define MODULUS M << 3  // sets modulus value
#define REG_1 0b001

// register 2 masks
#define LDS 1 << 31 //Lock detect speed adjustment: 0 fPFD < 32 MHz, 1 pPFD > 32 MHz
#define SDN SD << 29 //sets sigma-delta noise
#define MUX_2 MUX_LSB << 26  //sets MUX bits
#define DBR 1 << 25 //sets reference doubler mode, 0 disable, 1 enable
#define RDIV2 1 << 24 //enable reference divide-by-2
#define R_DIV R << 14 // set reference divider value
#define REG4DB 1 << 13 // sets double buffer mode
#define CP_SET  CP_CURRENT << 9  // sets CP current 
#define LDF 1 << 8 // sets lock detecet in integer mode
#define LDP 1 << 7 //sets lock detect precision
#define PDP 1 << 6 // phase detect polarity
#define SHDN 1 << 5 // shutdown mode
#define CP_HZ 1 << 4 // sets CP to high Z mode
#define RST 1 << 3 // R and N counters reset
#define REG_2 0b010

//register 3 masks
#define VCO_SET VCO << 26 // Manual selection of VCO and VCO sub-band when VAS is disabled.
#define VAS_SHDN 1 << 25  // VAS shutdown mode
#define VAS_TEMP 1 << 24 // sets VAS temperature compensation
#define CSM 1 << 18 // enable cycle slip mode
#define MUTEDEL 1 << 17 // Delay LD to MTLD function to prevent ﬂickering 	
#define CDM CDIV_MODE << 15  // sets clock divider mode
#define CDIV CDIV_VAL << 3 // sets clock divider value
#define REG_3 0b011

// register 4 masks
#define REG4HEAD 3 << 29 // Always program to 0b011
#define SDLDO 1 << 28 // Shutdown VCO LDO
#define SDDIV 1 << 27 // shutdown VCO divider
#define SDREF 1 << 26 // shutdown reference input mode
#define BS_MSB BS_MSB_VAL << 24 // Sets band select 2 MSBs
#define FB 1 << 23  //Sets VCO to N counter feedback mode
#define DIVA DIVA_VAL << 20 // Sets RFOUT_ output divider mode. Double buffered by register 0 when REG4DB = 1.
#define BS_LSB BS_LSB_VAL << 12 // Sets band select 8 LSBs
#define SDVCO 1 << 11 // sets VCO shutdown mode
#define MTLD 1 << 10 // Sets RFOUT Mute until Lock Detect Mode
#define BDIV 1 << 9 // Sets RFOUTB output path select. 0 = VCO divided output, 1 = VCO fundamental frequency
#define RFB_EN 1 << 8 // Enable RFOUTB output 
#define BPWR B_POWER << 6 //RFOUTB Power
#define RFA_EN 1 << 8 // Enable RFOUTA output 
#define APWR A_POWER << 6 //RFOUTA Power
#define REG_4 0b100

// register 5 masks
#define VAS_DLY VAS_DLY_VAL << 29 // VCO Autoselect Delay
#define SDPLL 1 << 25 // Shutdown PLL
#define F01 1 << 24 // sets integer mode when F = 0 
#define LD LD_VAL << 22 // sets lock detection pin function
#define MUX_5 MUX_MSB << 18 // sets MSB of MUX bits
#define ADCS 1 << 6 // Starts ADC mode
//#define ADCM ADC_MODE << 3 // ADC Mode  THIS PART IS DEFINED IN USER INTERFACE
#define REG_5 0b101

// register 6 masks (read only values)
#define ADC_mask 0x7F << 16 // mask ADC value in register 6
#define ADCV 1 << 15 // validity of ADC read
#define VASA 1 << 9 // determines if VAS is active
#define V 0x3F << 3  // Current VCO
#define REG_6 0b110



#endif
