#include "glcd.h"
#include "spritesheet.h"
#include<math.h>

U8 displayBuffer[DISPLAY_WIDTH*DISPLAY_HEIGHT];
void delay_us(U32 count)
{
  U32 i,j;

  for(j=0;j<count;j++)
  {
    /* At 60Mhz, the below loop introduces
    delay of APPROX 1 us */
    for(i=0;i<4;i++);
  }
}
void glcdInit(void)
{
		IO0DIR|=(1<<GLCD_RS|1<<GLCD_RW|1<<GLCD_EN|1<<GLCD_RST|1<<GLCD_CS1|1<<GLCD_CS2);
		IO0DIR|=0xFF<<DATASHIFT;
		
		IO0SET|=1<<GLCD_RST;
		IO0CLR|=1<<GLCD_EN;
		 
    glcdSelectPage0();
    glcdSendCommand(0x3F);//LCD ON
		glcdSendCommand(0xC0);//START FROM LEFTMOST
		glcdSendCommand(0xB8);//LINE 0
		glcdSendCommand(0x40);//CURSOR 0
	
    glcdSelectPage1();
    glcdSendCommand(0x3F);//LCD ON
		glcdSendCommand(0xC0);//START FROM LEFTMOST
		glcdSendCommand(0xB8);//LINE 0
		glcdSendCommand(0x40);//CURSOR 0
}
void glcdBusy(void)
{
	IO0DIR&=~(1<<BUSYBIT);//SET BUSY_BIT AS INPUT
	delay_us(1);
	IO0SET|=1<<GLCD_RW;//SET RW FOR READ MODE
	IO0CLR|=1<<GLCD_RS;//CLR RS FOR COMMAND MODE
	delay_us(1);
	IO0SET|=1<<GLCD_EN;
	delay_us(1);
	while(IO0PIN&(1<<BUSYBIT));//WAIT FOR INTERNAL OPERATION
	IO0CLR|=1<<GLCD_EN;
	IO0DIR|=1<<BUSYBIT;
}

void glcdSendCommand(U8 command)
{		
	 glcdBusy();//WAIT
	IO0CLR|=1<<GLCD_RS;
	IO0CLR|=1<<GLCD_RW;
	IO0CLR|=0xFF<<DATASHIFT;
	IO0SET|=command<<DATASHIFT;
	 delay_us(5);
	IO0SET|=1<<GLCD_EN;
	 delay_us(5);
	IO0CLR|=1<<GLCD_EN;
}

void glcdSendData(U8 data)
{	  
	 glcdBusy();//WAIT
	IO0SET|=1<<GLCD_RS;
	IO0CLR|=1<<GLCD_RW;
	IO0CLR|=0xFF<<DATASHIFT;
	IO0SET|=data<<DATASHIFT;
	 delay_us(5);
	IO0SET|=1<<GLCD_EN;
	 delay_us(5);
	IO0CLR|=1<<GLCD_EN;
}

void glcdDisplay()
{
	S16 line,cursor;
	for(line=0;line<8;++line)
	{
		glcdSelectPage0();
    glcdSendCommand(0x40);
		glcdSendCommand(0xB8+line);
		for(cursor=0;cursor<128;++cursor)
		{
			glcdSendData(displayBuffer[128*line+cursor]);
			if(cursor==63)
			{
				glcdSelectPage1();
				glcdSendCommand(0x40);
				glcdSendCommand(0xB8+line);
			}
		}
	}
}

void modifyBuffer(U8 command)
{
	U16 index;
	switch(command)
	{
		case CLEAR:for(index=0;index<128*64;++index)
		displayBuffer[index]=0x00;
		break;
		
		case HOME:for(index=0;index<128*64;++index)
		displayBuffer[index]=home1971[index];
		break;
		
		case VICTORY:for(index=0;index<128*64;++index)
		displayBuffer[index]=victory[index];
		break;
		
		case DEFEAT:for(index=0;index<128*64;++index)
		displayBuffer[index]=defeat[index];
		break;
		default:break;
	}
}

void addBufferUnit(U8 command,S16 line,S16 cursor)
{
	U16 index;
	switch(command)
	{
		case RM_IND_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=rm00[index];
		break;
		
		case RM_IND_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=rm01[index];
		break;
		
		case RM_PAK_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=rm10[index];
		break;
		
		case RM_PAK_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=rm11[index];
		break;
		
		case TK_IND_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk00[index];
		break;
		
		case TK_IND_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk01[index];
		break;
		
		case TK_IND_U:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk02[index];
		break;
		
		case TK_IND_D:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk03[index];
		break;
		
		case TK_PAK_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk10[index];
		break;
		
		case TK_PAK_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk11[index];
		break;
		
		case TK_PAK_U:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk12[index];
		break;
		
		case TK_PAK_D:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=tk13[index];
		break;
		
		case HZ_IND_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz00[index];
		break;
		
		case HZ_IND_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz01[index];
		break;
		
		case HZ_IND_U:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz02[index];
		break;
		
		case HZ_IND_D:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz03[index];
		break;
		
		case HZ_PAK_R:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz10[index];
		break;
		
		case HZ_PAK_L:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz11[index];
		break;
		
		case HZ_PAK_U:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz12[index];
		break;
		
		case HZ_PAK_D:for(index=0;index<8;++index)
		displayBuffer[line*128+cursor+index]=hz13[index];
		break;
		
		default:break;
	}
	
}

void addBufferChar(U16 letter,S16 line,S16 cursor)
{
	U16 index,index2;
	for(index=0;index<8*letter;index+=8);
	for(index2=index;index2<8+index;++index2)
	displayBuffer[line*128+cursor+index2-index]=cga8[index2];
}

void addBufferString(U8 word[],U8 length,S16 line,S16 cursor)
{
	U16 index;
	for(index=0;index<length;++index)
	addBufferChar(word[index],line,cursor+8*index);
}
void addBufferSmallChar(U8 letter,S16 line,S16 cursor)
{
	U16 index,index2;
	if(letter==' ')
		letter=26;
	else if(letter=='.')
		letter=27;
	else
		letter-=65;
	for(index=0;index<5*letter;index+=5);
	for(index2=index;index2<5+index;++index2)
	displayBuffer[line*128+cursor+index2-index]=cgaSmall[index2];
}
void addBufferSmallString(U8 word[],U8 length,S16 line,S16 cursor)
{
	U16 index;
	for(index=0;index<length;++index)
	addBufferSmallChar(word[index],line,cursor+6*index);
}
void addBufferBoldChar(U8 letter,S16 line,S16 cursor)
{
	addBufferChar(letter+32*8,line,cursor);
}
void addBufferBoldString(U8 word[],U8 length,S16 line,S16 cursor)
{
	U16 index;
	for(index=0;index<length;++index)
	addBufferBoldChar(word[index],line,cursor+8*index);
}
void addBufferNumber(U16 number,S16 line,S16 cursor)
{
	U16 index;
	cursor+=16;
	if(number==0)
		addBufferChar(number+48,line,cursor);
	else
	{
	for(index=0;index<3;number/=10,cursor-=8,++index)
	addBufferChar(number%10+48,line,cursor);//+256
	}
}
void addBufferSelection(S16 line,S16 cursor)
{
	displayBuffer[128*line+cursor]|=129;
	displayBuffer[128*line+cursor+7]|=129;
}
void addBufferEnemyHPBar(S16 line,S16 cursor,S16 hp,S16 maxhp)
{
	S16 life=ceil(hp*8.0/maxhp);
	switch(life)
	{
		case 8:displayBuffer[128*line+cursor+7]|=1;
		case 7:displayBuffer[128*line+cursor+6]|=1;
		case 6:displayBuffer[128*line+cursor+5]|=1;
		case 5:displayBuffer[128*line+cursor+4]|=1;
		case 4:displayBuffer[128*line+cursor+3]|=1;
		case 3:displayBuffer[128*line+cursor+2]|=1;
		case 2:displayBuffer[128*line+cursor+1]|=1;
		case 1:displayBuffer[128*line+cursor]|=1;break;
		default:break;
	}
}
void addBufferDestruction(S16 line,S16 cursor)
{
	U16 index;
	for(index=0;index<8;++index)
		displayBuffer[128*line+cursor+index]=0;
}
