#ifndef _GLCD_CONFIG_H
#define _GLCD_CONFIG_H

#include<LPC214x.h>

#define BUSYBIT 	7
#define GLCD_RS 	8
#define GLCD_RW 	9
#define GLCD_EN 	10
#define GLCD_CS1 	11
#define GLCD_CS2 	12
#define GLCD_RST 	13

#define SWSELECT 21 //select switch
#define SWBACK 22 //back switch
#define ADC01 28 //x-axis
#define ADC02 29 //y-axis

#define DATASHIFT 0
#define DATAMASK 	0xFFFFFF00

/*LCD cofiguration:
19 D0
21 D1
22 D2
26 D3
27 D4
29 D5
30 D6
31 D7
33 RS
34 RW
35 EN
37 CS1
38 CS2
39 RST
Vss/BLK Gnd
Vdd/BLA 5V
Vout/Gnd- Pot terminals
Vo-Pot jockey
*/
#endif
