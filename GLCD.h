#ifndef _GLCD_H
#define _GLCD_H
#include<LPC214x.h>
#include "glcd_config.h"
#include "utilities.h"

#define glcdSelectPage0() IO0SET|=1<<GLCD_CS1; IO0CLR|=1<<GLCD_CS2;
#define glcdSelectPage1() IO0CLR|=1<<GLCD_CS1; IO0SET|=1<<GLCD_CS2;

//delay
void delay_us(U32 count);

//display part
void glcdInit(void);
void glcdSendCommand(U8 command);
void glcdSendData(U8 data);
void glcdBusy(void);
void glcdDisplay(void);

//buffer operations
void modifyBuffer(U8 command);
void addBufferUnit(U8 command,S16 line,S16 cursor);
void addBufferChar(U16 letter,S16 line,S16 cursor);
void addBufferString(U8 word[],U8 length,S16 line,S16 cursor);
void addBufferSmallChar(U8 letter,S16 line,S16 cursor);
void addBufferSmallString(U8 word[],U8 length,S16 line,S16 cursor);
void addBufferBoldChar(U8 letter,S16 line,S16 cursor);
void addBufferBoldString(U8 word[],U8 length,S16 line,S16 cursor);
void addBufferSelection(S16 line,S16 cursor);
void addBufferNumber(U16 number,S16 line,S16 cursor);
void addBufferEnemyHPBar(S16 line,S16 cursor,S16 hp,S16 maxhp);
void addBufferDestruction(S16 line,S16 cursor);

#endif
