#ifndef _ADC_H
#define _ADC_H


#define BURST_MODE 1
#define CONTROLLED_MODE 0
#define CLKDIV 6
#define PDN 1<<21
#define DATA_MASK 0x0000FFC0; //111111111100

unsigned int ADC(int,int,unsigned int);

#endif
