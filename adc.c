#include<LPC214x.H>
#include"adc.h"

unsigned int ADC(int adc_num,int channel,unsigned int mode)
{
   unsigned int val;
   if(adc_num==0)
   {
		 AD0CR&=~(0xFF);
     AD0CR  = AD0CR|(0x0000FF00&(CLKDIV<<8)); // Dividing PCLK by CLKDIV set to lowest value
     AD0CR  = AD0CR|PDN; // ADC is operational not power dwn
     AD0CR  = AD0CR|(1<<channel);//channel of AD0 SELECT
     if(mode==CONTROLLED_MODE)
     {
       AD0CR = (AD0CR & ~(7<<24))|(1<<24); // Start Conversion NOW bIt 24 is 1
     }
     
      // wait to finish conversion
		 
		 if(channel==1)
		 {
			 while(!(AD0GDR&(0x80000000)));
			val = AD0DR1;// bit 6-15 contains data 
		 }
		 else if(channel==2)
		 {
			 while(!(AD0GDR&(0x80000000)));
			 val = AD0DR2;
		 }
     val = val & DATA_MASK;   // data manipulation to get 10 bit value
     return(val>>6);    
   }
   return(0);
}
