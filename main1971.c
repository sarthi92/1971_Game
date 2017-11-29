#include "glcd.h"
#include "adc.h"
#include<stdlib.h>
#include<stdio.h> 

UNIT activeUnitsList[maxUnitsCount];//Stores indian as well as enemy units, each with Population Limit=maxUnitsCount/2
U8 selectedUnit,indianUnitsCount,enemyUnitsCount;//selectedUnit=index of selected unit
U8 currentLine,currentScreen,currentLevel;//present parameters
U8 gameCounter;//game Action counter for units to fire periodically

S16 maxHP[20]={50,50,50,50,450,450,450,450,500,500,500,500,250,250,250,250,250,250,250,250};//defines maximum HP of units
U8 range[20]={8,8,8,8,8,8,8,8,8,8,8,8,16,16,16,16,16,16,16,16};//defines attack range of units
S16 attackMatrix[20]={20,20,20,20,150,150,150,150,150,150,150,150,200,200,200,200,200,200,200,200};//defines attack strength of units
S16 defenceMatrix[20][20]={//each entry defines defence of a target from attacking unit
{10,10,10,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{10,10,10,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{10,10,10,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{10,10,10,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{18,18,18,18,50,50,50,50,50,50,50,50,40,40,40,40,40,40,40,40},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50},
{10,10,10,10,80,80,80,80,80,80,80,80,50,50,50,50,50,50,50,50}
};

void homeScreen(void);//screen where game starts & game logo is displayed
void mainMenu(void);//menu to select CAMPAIGN, DEATHMATCH, HELP, EXIT
void campaignMenu(void);//menu to select different levels
void helpScreen(void);//game instructions window
void messageScreen(void);//displays level description before starting gameplay
void gameScreen(void);//gameplay where all action takes place
void exitGame(void);//asks user wether to exit
void resultScreen(void);//displays Victory/Defeat condition of a battle

void getSwitchResponse(void);//polls Select Switch & Back Switch to get response
void getJoystickResponse(void);//reads ADC values related to Joystick axis to get response
	
void createUnit(U8 type,S16 posX,S16 posY,S16 destX,S16 destY,U8 team);//creates unit
void generateLevelUnits(void);//generates initial units for each level
void addActiveUnits(void);//adds unit to buffer for printing to screen
void updateActiveUnits(void);//updates parameters of each unit

void destroyUnit(U8 targetIndex);//destroys targeted unit
U8 checkUnitInRange(U8 unitIndex,U8 targetIndex);//checks if target unit is in range-USER AI
U8 checkUnitNearRange(U8 unitIndex,U8 targetIndex);//checks if target unit is nearby but outside attack range
void attackUnit(U8 unitIndex,U8 targetIndex);//attacks target unit
void followUnit(U8 unitIndex,U8 targetIndex);//follows target unit-ENEMY AI
U8 getWeakestUnit(U8 unitIndex);//returns index of weakest unit in range-ENEMY AI
void faceTarget(U8 unitIndex,U8 targetIndex);//faces target to attack/follow
void indianAction(void);//fire at enemy unit periodically
void enemyAction(void);//fire at indian unit periodically

void getSwitchResponse()
{
		U32 select,back;
		select=IO0PIN&(1<<SWSELECT);
		back=IO0PIN&(1<<SWBACK);
	
		if(!select || !back)
		switch(currentScreen)
		{
			case HOMELOGO:
			currentScreen=MAINMENU;
			break;
				
			case MAINMENU:
			if(!back)currentScreen=HOMELOGO;
			else
			switch(currentLine)
			{
				case 2:currentScreen=CAMPAIGN;break;
				case 3:currentScreen=DMATCH;currentLevel=DMATCH;break;
				case 4:currentScreen=HELP;break;
				default:break;
			}
			break;
			
			case CAMPAIGN:
			if(!back)currentScreen=MAINMENU;
			else
			switch(currentLine)
			{
				case 2:currentScreen=LV1MSG;currentLevel=LV1MSG;break;
				case 3:currentScreen=LV2MSG;currentLevel=LV2MSG;break;
				case 4:currentScreen=LV3MSG;currentLevel=LV3MSG;break;
				case 5:currentScreen=LV4MSG;currentLevel=LV4MSG;break;
				default:break;
			}
			break;
			
			case HELP:
			currentScreen=MAINMENU;
			break;
			
			case LV1MSG:
			if(!select)
			{
				currentScreen=GAMEPLAY;
				generateLevelUnits(); 
			}
			else 
			currentScreen=CAMPAIGN;
			break;
			
			case LV2MSG:
			if(!select)
			{
				currentScreen=GAMEPLAY;
				generateLevelUnits(); 
			}
			else 
			currentScreen=CAMPAIGN;
			break;
			
			case LV3MSG:
			if(!select)
			{
				currentScreen=GAMEPLAY;
				generateLevelUnits(); 
			}
			else 
			currentScreen=CAMPAIGN;
			break;
			
			case LV4MSG:
			if(!select)
			{
				currentScreen=GAMEPLAY;
				generateLevelUnits(); 
			}
			else 
			currentScreen=CAMPAIGN;
			break;
			
			case DMATCH:
			if(!select)
			{				
				currentScreen=GAMEPLAY;
				generateLevelUnits();
			}
			else
			currentScreen=MAINMENU;
			break;
				
			case GAMEPLAY: 
			if(!select) selectedUnit=(selectedUnit+1)%indianUnitsCount;
			else currentScreen=EXITGAME;
			break;
				
			case EXITGAME:
			if(!select) currentScreen=MAINMENU;
			else currentScreen=GAMEPLAY;
			break;
			
			case RESULT:
			currentScreen=CAMPAIGN;
			break;
			
			default:break;
	}
	delay_us(40000);
}
void getJoystickResponse()
{
	U16 responseX,responseY;
	responseX=ADC(0,2,CONTROLLED_MODE);
	responseY=ADC(0,1,CONTROLLED_MODE);
	if(currentScreen!=GAMEPLAY)
	{
		if(responseY>750)
			--currentLine;
		if(responseY<10)
			++currentLine;
	}
	else
	{
		if(responseX>750)
		{
			activeUnitsList[selectedUnit].destination.posX=activeUnitsList[selectedUnit].source.posX;
			activeUnitsList[selectedUnit].destination.posX++;
		}
		if(responseX<10)
		{
			activeUnitsList[selectedUnit].destination.posX=activeUnitsList[selectedUnit].source.posX;
			activeUnitsList[selectedUnit].destination.posX--;
		}
		if(responseY>750)
		{
			activeUnitsList[selectedUnit].destination.posY=activeUnitsList[selectedUnit].source.posY;
			activeUnitsList[selectedUnit].destination.posY--;
		}
		if(responseY<10)
		{
			activeUnitsList[selectedUnit].destination.posY=activeUnitsList[selectedUnit].source.posY;
			activeUnitsList[selectedUnit].destination.posY++;
		}
	}
	delay_us(40000);
}
void homeScreen()
{
	U8 data[]="<Press any key> ";
	modifyBuffer(HOME);
	glcdDisplay();
	delay_us(1000000);
	addBufferBoldString(data,16,7,0);
	glcdDisplay();
	while(1)
	{
		getSwitchResponse();
		if(currentScreen!=HOMELOGO)
			break;
	}
}
void mainMenu()
{
	U8 head[]="   Main Menu    ";
		U8 data0[]="       CAMPAIGN      ";
		U8 data1[]="      DEATHMATCH     ";
		U8 data2[]="         HELP        ";
		U8 foot[]="X:Select  Y:Exit";
	currentLine=2;
	while(1)
	{
		modifyBuffer(CLEAR);
		addBufferBoldString(head,16,0,0);
		addBufferSmallString(data0,21,2,0);
		addBufferSmallString(data1,21,3,0);
		addBufferSmallString(data2,21,4,0);
		addBufferBoldString(foot,16,7,0);
		if(currentLine<2)currentLine=4;
		else if(currentLine>4)currentLine=2;
		addBufferChar(16,currentLine,24);
		addBufferChar(17,currentLine,102);
		glcdDisplay();
		getJoystickResponse();
		getSwitchResponse();
		if(currentScreen!=MAINMENU)break;
	}
}
 void campaignMenu()
{
	U8 head[]="    Campaign    ";
		U8 data0[]="   BREAKING DAWN     ";
		U8 data1[]="   OPPOSING FORCE    ";
		U8 data2[]="   SIALKOT STRIKE    ";
		U8 data3[]="     DHAKA OPS       ";
		U8 foot[]="X:Select  Y:Back";
	currentLine=2;
	while(1)
	{
		modifyBuffer(CLEAR);
		addBufferBoldString(head,16,0,0);
		addBufferSmallString(data0,21,2,0);
		addBufferSmallString(data1,21,3,0);
		addBufferSmallString(data2,21,4,0);
		addBufferSmallString(data3,21,5,0);
		addBufferBoldString(foot,16,7,0);
		if(currentLine<2)currentLine=5;
		else if(currentLine>5)currentLine=2;
		addBufferChar(16,currentLine,6);
		addBufferChar(17,currentLine,108);
		glcdDisplay();
		getJoystickResponse();
		getSwitchResponse();
		if(currentScreen!=CAMPAIGN)break;
	}
}
void helpScreen()
{
		U8 head[]="      Help      ";
		U8 data0[]="USE JOYSTICK AND X   ";
		U8 data1[]="KEY TO SELECT AND Y  ";
		U8 data2[]="KEY TO GO BACK.      ";
		U8 data3[]="SELECT AND MOVE UNITS";
		U8 data4[]="TO ATTACK.           ";
		U8 foot[]="X:Select  Y:Back";
		modifyBuffer(CLEAR);
		addBufferBoldString(head,16,0,0);
		addBufferSmallString(data0,21,2,0);
		addBufferSmallString(data1,21,3,0);
		addBufferSmallString(data2,21,4,0);
		addBufferSmallString(data3,21,5,0);
		addBufferSmallString(data4,21,6,0);
		addBufferBoldString(foot,16,7,0);
		glcdDisplay();
	while(1)
	{
		getSwitchResponse();
		if(currentScreen!=HELP)break;
	}
}
void messageScreen()
{
	U8 head[16];
	U8 data0[21],data1[21],data2[21],data3[21];
	U8 foot[]="X:Start   Y:Back";
	U8 presentScreen=currentScreen;
	switch(currentLevel)
	{
		case LV1MSG:
			sprintf((char*)head," BREAKING DAWN  ");
			sprintf((char*)data0,"TANKS SPOTTED AT THE ");
			sprintf((char*)data1,"BORDER. STAND GUARD  ");
			sprintf((char*)data2,"AND DEFEND LONGEWALA.");
			sprintf((char*)data3,"DEATH BEFORE RETREAT.");
			break;
		case LV2MSG:
			sprintf((char*)head," OPPOSING FORCE ");
			sprintf((char*)data0,"REINFORCEMENTS HAVE  ");
			sprintf((char*)data1,"ARRIVED. DESTROY ALL ");
			sprintf((char*)data2,"ENEMY TANKS WITH YOUR");
			sprintf((char*)data3,"ARTILLERY.           ");
			break;
		case LV3MSG:
			sprintf((char*)head," SIALKOT STRIKE ");
			sprintf((char*)data0,"COMMANDEER TANKS AND ");
			sprintf((char*)data1,"AMBUSH ENEMY UNITS.  ");
			sprintf((char*)data2,"CAPTURE ALL ENEMY    ");
			sprintf((char*)data3,"TERRITORY.           ");
			break;
		case LV4MSG:
			sprintf((char*)head,"   DHAKA OPS    ");
			sprintf((char*)data0,"FIGHT USING HOWITZERS");
			sprintf((char*)data1,"AND PREVENT ACCESS TO");
			sprintf((char*)data2,"THE ONLY BRIDGE THAT ");
			sprintf((char*)data3,"LEADS TO DHAKA.      ");
			break;
		case DMATCH:
			sprintf((char*)head,"   DEATHMATCH   ");
			sprintf((char*)data0,"RELIVE HISTORY AND   ");
			sprintf((char*)data1,"FIGHT WITH VALOUR.   ");
			sprintf((char*)data2,"LEAD INDIAN ARMY TO  ");
			sprintf((char*)data3,"VICTORY.             ");
			break;
		default:break;
	}
	modifyBuffer(CLEAR);
	addBufferBoldString(head,16,0,0);
	addBufferSmallString(data0,21,2,0);
	addBufferSmallString(data1,21,3,0);
	addBufferSmallString(data2,21,4,0);
	addBufferSmallString(data3,21,5,0);
	addBufferBoldString(foot,16,7,0);
	glcdDisplay();
	while(1)
	{
		getSwitchResponse();
		if(currentScreen!=presentScreen)break;
	}
}
void createUnit(U8 type,S16 posX,S16 posY,S16 destX,S16 destY,U8 team)
{
	U8 selectedCount;
	if(team==IND)
		selectedCount=indianUnitsCount;
	else
		selectedCount=enemyUnitsCount+10;
	activeUnitsList[selectedCount].type=type;
	activeUnitsList[selectedCount].source.posX=posX;
	activeUnitsList[selectedCount].source.posY=posY;
	activeUnitsList[selectedCount].destination.posX=destX;
	activeUnitsList[selectedCount].destination.posY=destY;
	switch(type)
	{
		case RM_IND_R:
		case RM_IND_L:activeUnitsList[indianUnitsCount++].hp=50;break;
		case RM_PAK_R:
		case RM_PAK_L:activeUnitsList[10+enemyUnitsCount++].hp=50;break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[indianUnitsCount++].hp=450;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[10+enemyUnitsCount++].hp=500;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[indianUnitsCount++].hp=250;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[10+enemyUnitsCount++].hp=250;break;
		default:break;
	}
	
}
void generateLevelUnits()
{
	indianUnitsCount=0;
	enemyUnitsCount=0;
	selectedUnit=0;
	switch(currentLevel)
	{
		case LV1MSG:
		createUnit(HZ_IND_R,10,1,10,1,IND);
		createUnit(RM_IND_R,15,2,15,2,IND);
		createUnit(RM_IND_R,15,3,15,3,IND);
		createUnit(RM_IND_R,15,4,15,4,IND);
		createUnit(HZ_IND_R,10,5,10,5,IND);
		createUnit(RM_PAK_L,110,1,45,1,PAK);
		createUnit(TK_PAK_L,105,2,55,2,PAK);
		createUnit(TK_PAK_L,105,3,55,3,PAK);
		createUnit(TK_PAK_L,105,4,55,4,PAK);
		createUnit(RM_PAK_L,110,5,45,5,PAK);
		break;
		
		case LV2MSG:
		createUnit(HZ_IND_R,0,1,20,1,IND);
		createUnit(HZ_IND_R,0,2,15,2,IND);
		createUnit(HZ_IND_R,0,3,15,3,IND);
		createUnit(HZ_IND_R,0,4,15,4,IND);
		createUnit(HZ_IND_R,0,5,20,5,IND);
		createUnit(TK_PAK_L,110,0,45,0,PAK);
		createUnit(RM_PAK_L,110,1,45,1,PAK);
		createUnit(TK_PAK_L,105,2,55,2,PAK);
		createUnit(TK_PAK_L,105,3,55,3,PAK);
		createUnit(TK_PAK_L,105,4,55,4,PAK);
		createUnit(RM_PAK_L,110,5,45,5,PAK);
		createUnit(TK_PAK_L,110,6,45,6,PAK);
		break;
		
		case LV3MSG:
		createUnit(TK_IND_R,0,1,10,1,IND);
		createUnit(TK_IND_R,0,2,15,2,IND);
		createUnit(RM_IND_R,0,3,15,3,IND);
		createUnit(TK_IND_R,0,4,15,4,IND);
		createUnit(TK_IND_R,0,5,10,5,IND);
		createUnit(TK_IND_R,0,1,20,1,IND);
		createUnit(TK_IND_R,0,2,25,2,IND);
		createUnit(RM_IND_R,0,3,25,3,IND);
		createUnit(TK_IND_R,0,4,25,4,IND);
		createUnit(TK_IND_R,0,5,20,5,IND);
		createUnit(RM_PAK_L,100,1,100,1,PAK);
		createUnit(RM_PAK_R,105,2,105,2,PAK);
		createUnit(RM_PAK_R,105,3,105,3,PAK);
		createUnit(RM_PAK_R,105,4,105,4,PAK);
		createUnit(TK_PAK_R,90,5,90,5,PAK);
		createUnit(HZ_PAK_L,115,1,115,1,PAK);
		createUnit(RM_PAK_L,80,2,75,2,PAK);
		createUnit(RM_PAK_R,80,3,90,3,PAK);
		createUnit(RM_PAK_L,80,4,75,4,PAK);
		createUnit(HZ_PAK_L,110,5,110,5,PAK);
		break;
		
		case LV4MSG:
		createUnit(HZ_IND_R,10,1,10,1,IND);
		createUnit(HZ_IND_R,15,2,15,2,IND);
		createUnit(HZ_IND_R,15,3,15,3,IND);
		createUnit(HZ_IND_R,15,4,15,4,IND);
		createUnit(HZ_IND_R,10,5,10,5,IND);
		createUnit(HZ_IND_R,0,1,20,1,IND);
		createUnit(RM_IND_R,0,2,25,2,IND);
		createUnit(RM_IND_R,0,3,25,3,IND);
		createUnit(RM_IND_R,0,4,25,4,IND);
		createUnit(HZ_IND_R,0,5,20,5,IND);
		createUnit(HZ_PAK_L,110,1,110,1,PAK);
		createUnit(HZ_PAK_L,105,2,105,2,PAK);
		createUnit(HZ_PAK_L,105,3,105,3,PAK);
		createUnit(HZ_PAK_L,105,4,105,4,PAK);
		createUnit(HZ_PAK_L,110,5,110,5,PAK);
		createUnit(RM_PAK_L,110,1,100,1,PAK);
		createUnit(RM_PAK_L,105,2,95,2,PAK);
		createUnit(RM_PAK_L,105,3,95,3,PAK);
		createUnit(RM_PAK_L,105,4,95,4,PAK);
		createUnit(RM_PAK_L,110,5,100,5,PAK);
		break;
		
		case DMATCH:
		createUnit(RM_IND_R,0,1,10,1,IND);
		createUnit(RM_IND_R,0,2,15,2,IND);
		createUnit(RM_IND_R,0,3,15,3,IND);
		createUnit(RM_IND_R,0,4,15,4,IND);
		createUnit(RM_IND_R,0,5,10,5,IND);
		createUnit(RM_IND_R,0,1,20,1,IND);
		createUnit(RM_IND_R,0,2,25,2,IND);
		createUnit(RM_IND_R,0,3,25,3,IND);
		createUnit(RM_IND_R,0,4,25,4,IND);
		createUnit(RM_IND_R,0,5,20,5,IND);
		createUnit(RM_PAK_L,120,1,100,1,PAK);
		createUnit(RM_PAK_L,115,2,90,2,PAK);
		createUnit(RM_PAK_L,115,3,90,3,PAK);
		createUnit(RM_PAK_L,115,4,90,4,PAK);
		createUnit(RM_PAK_L,120,5,100,5,PAK);
		createUnit(RM_PAK_L,120,1,90,1,PAK);
		createUnit(RM_PAK_L,115,2,80,2,PAK);
		createUnit(RM_PAK_L,115,3,80,3,PAK);
		createUnit(RM_PAK_L,115,4,80,4,PAK);
		createUnit(RM_PAK_L,120,5,90,5,PAK);
		break;
		
		default:break;
	}
}
void addActiveUnits()
{
	U8 index;
	for(index=0;index<indianUnitsCount;++index)
	addBufferUnit(activeUnitsList[index].type,activeUnitsList[index].source.posY,activeUnitsList[index].source.posX);
	for(index=10;index<enemyUnitsCount+10;++index)
	{
		addBufferUnit(activeUnitsList[index].type,activeUnitsList[index].source.posY,activeUnitsList[index].source.posX);
		addBufferEnemyHPBar(activeUnitsList[index].source.posY,activeUnitsList[index].source.posX,activeUnitsList[index].hp,maxHP[activeUnitsList[index].type]);
	}
}
void updateActiveUnits()
{
	U8 index;
	for(index=0;index<enemyUnitsCount+10;++index)
	{
		if(index==indianUnitsCount)
		{
			if(enemyUnitsCount)
				index=10;
			else
				break;
		}
		if(activeUnitsList[index].destination.posX>119)
			activeUnitsList[index].destination.posX=119;
		else if(activeUnitsList[index].destination.posX<0)
			activeUnitsList[index].destination.posX=0;
		if(activeUnitsList[index].destination.posY>6)
			activeUnitsList[index].destination.posY=6;
		else if(activeUnitsList[index].destination.posY<0)
			activeUnitsList[index].destination.posX=0;
		if(activeUnitsList[index].destination.posX>activeUnitsList[index].source.posX)
		{
			activeUnitsList[index].source.posX++;
			activeUnitsList[index].destination.posY=activeUnitsList[index].source.posY;			
			switch(activeUnitsList[index].type)
		{
		case RM_IND_R:
		case RM_IND_L:activeUnitsList[index].type=RM_IND_R;break;
		case RM_PAK_R:
		case RM_PAK_L:activeUnitsList[index].type=RM_PAK_R;break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[index].type=TK_IND_R;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[index].type=TK_PAK_R;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[index].type=HZ_IND_R;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[index].type=HZ_PAK_R;break;
		default:break;
		}
		}
		else if(activeUnitsList[index].destination.posX<activeUnitsList[index].source.posX)
		{
			activeUnitsList[index].source.posX--;
			activeUnitsList[index].destination.posY=activeUnitsList[index].source.posY;			
			switch(activeUnitsList[index].type)
		{
		case RM_IND_R:
		case RM_IND_L:activeUnitsList[index].type=RM_IND_L;break;
		case RM_PAK_R:
		case RM_PAK_L:activeUnitsList[index].type=RM_PAK_L;break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[index].type=TK_IND_L;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[index].type=TK_PAK_L;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[index].type=HZ_IND_L;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[index].type=HZ_PAK_L;break;
		default:break;
		}
		}
		else if(activeUnitsList[index].destination.posY>activeUnitsList[index].source.posY)
		{
			activeUnitsList[index].source.posY++;
			activeUnitsList[index].destination.posX=activeUnitsList[index].source.posX;			
			switch(activeUnitsList[index].type)
		{
		case RM_IND_R:
		case RM_IND_L:
		case RM_PAK_R:
		case RM_PAK_L:break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[index].type=TK_IND_D;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[index].type=TK_PAK_D;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[index].type=HZ_IND_D;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[index].type=HZ_PAK_D;break;
		default:break;
		}
		}
		else if(activeUnitsList[index].destination.posY<activeUnitsList[index].source.posY)
		{
			activeUnitsList[index].source.posY--;
			activeUnitsList[index].destination.posX=activeUnitsList[index].source.posX;			
			switch(activeUnitsList[index].type)
		{
		case RM_IND_R:
		case RM_IND_L:
		case RM_PAK_R:
		case RM_PAK_L:break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[index].type=TK_IND_U;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[index].type=TK_PAK_U;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[index].type=HZ_IND_U;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[index].type=HZ_PAK_U;break;
		default:break;
		}
		}
	}
}
void destroyUnit(U8 targetIndex)
{
	U8 index,selectedCount;
	if(targetIndex<10)
		selectedCount=indianUnitsCount;
	else
		selectedCount=enemyUnitsCount+10;
	for(index=targetIndex+1;index<selectedCount;++index)
	{
		activeUnitsList[index-1]=activeUnitsList[index];
	}
	if(targetIndex<10)
		--indianUnitsCount;
	else
		--enemyUnitsCount;
	if(selectedUnit==indianUnitsCount)
		selectedUnit=0;
}
U8 checkUnitInRange(U8 unitIndex,U8 targetIndex)
{
	if(abs(activeUnitsList[unitIndex].source.posX-activeUnitsList[targetIndex].source.posX)<=range[activeUnitsList[unitIndex].type])
		if(abs(activeUnitsList[unitIndex].source.posY-activeUnitsList[targetIndex].source.posY)<=1)
			return 1;
	return 0;
}
U8 checkUnitNearRange(U8 unitIndex,U8 targetIndex)
{
	if(abs(activeUnitsList[unitIndex].source.posX-activeUnitsList[targetIndex].source.posX)<=range[activeUnitsList[unitIndex].type]+8)
		if(abs(activeUnitsList[unitIndex].source.posY-activeUnitsList[targetIndex].source.posY)<=2)
			return 1;
	return 0;
}
void followUnit(U8 unitIndex,U8 targetIndex)
{
	if(activeUnitsList[unitIndex].source.posX>activeUnitsList[targetIndex].source.posX+range[activeUnitsList[unitIndex].type])
	{
		activeUnitsList[unitIndex].destination.posX=activeUnitsList[targetIndex].source.posX+range[activeUnitsList[unitIndex].type];
	}
	else if(activeUnitsList[unitIndex].source.posX<activeUnitsList[targetIndex].source.posX-range[activeUnitsList[unitIndex].type])
	{
		activeUnitsList[unitIndex].destination.posX=activeUnitsList[targetIndex].source.posX-range[activeUnitsList[unitIndex].type];
	}
	if(activeUnitsList[unitIndex].source.posY>activeUnitsList[targetIndex].source.posY+1)
	{
		activeUnitsList[unitIndex].destination.posY=activeUnitsList[targetIndex].source.posY+1;
	}
	else if(activeUnitsList[unitIndex].source.posY<activeUnitsList[targetIndex].source.posY-1)
	{
		activeUnitsList[unitIndex].destination.posY=activeUnitsList[targetIndex].source.posY-1;
	}
}
void attackUnit(U8 unitIndex,U8 targetIndex)
{
	S16 mockAttack,mockDefence,mockHP;
	mockAttack=attackMatrix[activeUnitsList[unitIndex].type];
	mockDefence=defenceMatrix[activeUnitsList[targetIndex].type][activeUnitsList[unitIndex].type];
	mockHP=activeUnitsList[targetIndex].hp-mockAttack+mockDefence;
	if(mockHP<=0)
		destroyUnit(targetIndex);
	else
		activeUnitsList[targetIndex].hp=mockHP;
	addBufferDestruction(activeUnitsList[targetIndex].source.posY,activeUnitsList[targetIndex].source.posX);
	glcdDisplay();
}
U8 getWeakestUnit(U8 unitIndex)//ENEMY-AI
{
	U8 index,flag;
	S16 mockAttack,mockDefence,mockHP,mockIndex=indianUnitsCount;
	for(index=0;index<indianUnitsCount;++index)
	{
		flag=1;
		if(index==unitIndex)
			flag=0;
		if(flag && !checkUnitInRange(unitIndex,index))
			flag=0;
		if(!flag)
			continue;
		if(mockIndex==indianUnitsCount)
		{
			mockAttack=attackMatrix[activeUnitsList[unitIndex].type];
			mockDefence=defenceMatrix[activeUnitsList[index].type][activeUnitsList[unitIndex].type];
			if(activeUnitsList[index].hp-mockAttack+mockDefence<=0)
				return index;
			mockHP=activeUnitsList[index].hp-mockAttack+mockDefence;
			mockIndex=index;
		}
		else
		{
			mockAttack=attackMatrix[activeUnitsList[unitIndex].type];
			mockDefence=defenceMatrix[activeUnitsList[index].type][activeUnitsList[unitIndex].type];
			if(activeUnitsList[index].hp-mockAttack+mockDefence<=0)
				return index;
			if(activeUnitsList[index].hp-mockAttack+mockDefence<mockHP)
			{
				mockHP=activeUnitsList[index].hp-mockAttack+mockDefence;
				mockIndex=index;
			}
		}
	}
	return mockIndex;
}
void faceTarget(U8 unitIndex,U8 targetIndex)
{
	if(activeUnitsList[targetIndex].source.posY>activeUnitsList[unitIndex].source.posY)
		{	
			switch(activeUnitsList[unitIndex].type)
		{
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[unitIndex].type=TK_IND_D;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[unitIndex].type=TK_PAK_D;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[unitIndex].type=HZ_IND_D;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[unitIndex].type=HZ_PAK_D;break;
		default:break;
		}
		}
		else if(activeUnitsList[targetIndex].source.posY<activeUnitsList[unitIndex].source.posY)
		{	
			switch(activeUnitsList[unitIndex].type)
		{
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[unitIndex].type=TK_IND_U;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[unitIndex].type=TK_PAK_U;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[unitIndex].type=HZ_IND_U;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[unitIndex].type=HZ_PAK_U;break;
		default:break;
		}
		}
		else if(activeUnitsList[targetIndex].source.posX>activeUnitsList[unitIndex].source.posX)
		{	
			switch(activeUnitsList[unitIndex].type)
		{
		case RM_IND_R:
		case RM_IND_L:activeUnitsList[unitIndex].type=RM_IND_R;break;
		case RM_PAK_R:
		case RM_PAK_L:activeUnitsList[unitIndex].type=RM_PAK_R;break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[unitIndex].type=TK_IND_R;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[unitIndex].type=TK_PAK_R;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[unitIndex].type=HZ_IND_R;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[unitIndex].type=HZ_PAK_R;break;
		default:break;
		}
		}
		else if(activeUnitsList[targetIndex].source.posX<activeUnitsList[unitIndex].source.posX)
		{	
			switch(activeUnitsList[unitIndex].type)
		{
		case RM_IND_R:
		case RM_IND_L:activeUnitsList[unitIndex].type=RM_IND_L;break;
		case RM_PAK_R:
		case RM_PAK_L:activeUnitsList[unitIndex].type=RM_PAK_L;break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:activeUnitsList[unitIndex].type=TK_IND_L;break;
		case TK_PAK_R:
		case TK_PAK_L:
		case TK_PAK_U:
		case TK_PAK_D:activeUnitsList[unitIndex].type=TK_PAK_L;break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:activeUnitsList[unitIndex].type=HZ_IND_L;break;
		case HZ_PAK_R:
		case HZ_PAK_L:
		case HZ_PAK_U:
		case HZ_PAK_D:activeUnitsList[unitIndex].type=HZ_PAK_L;break;
		default:break;
		}
		}
}
void indianAction()
{
	U8 index,index2;
	for(index=0;index<indianUnitsCount;++index)
	{
		for(index2=10;index2<enemyUnitsCount+10;++index2)
		{
			if(checkUnitInRange(index,index2))
			{
				faceTarget(index,index2);
				if(activeUnitsList[index].type==RM_IND_R||activeUnitsList[index].type==RM_IND_L)
				{
					if(gameCounter%16==0)
					attackUnit(index,index2);
				}
				else if(gameCounter%32==0)
					attackUnit(index,index2);
				break;
			}
		}
	}
}
void enemyAction()
{
	U8 index,index2,mockIndianIndex;
	for(index=10;index<enemyUnitsCount+10;++index)
	{
		mockIndianIndex=getWeakestUnit(index);
		if(mockIndianIndex!=indianUnitsCount)
		{
			faceTarget(index,mockIndianIndex);
			if(activeUnitsList[index].type==RM_PAK_R||activeUnitsList[index].type==RM_PAK_L)
				{
					if(gameCounter%16==8)
					attackUnit(index,mockIndianIndex);
				}
			else if(gameCounter%32==16)
				attackUnit(index,mockIndianIndex);
		}
		else
		for(index2=0;index2<indianUnitsCount;++index2)
		{
			if(checkUnitNearRange(index,index2))
			{
				faceTarget(index,index2);
				followUnit(index,index2);
				break;
			}
		}
	}
}
void gameScreen()
{
	U8 rmMessage[]="Rifleman  HP:   ";
	U8 tkMessage[]="Tank      HP:   ";
	U8 hzMessage[]="Howitzer  HP:   ";
	while(1)
	{
		modifyBuffer(CLEAR);
		addActiveUnits();
		addBufferSelection(activeUnitsList[selectedUnit].source.posY,activeUnitsList[selectedUnit].source.posX);
		switch(activeUnitsList[selectedUnit].type)
		{
		case RM_IND_R:
		case RM_IND_L:addBufferBoldString(rmMessage,16,7,0);break;
		case TK_IND_R:
		case TK_IND_L:
		case TK_IND_U:
		case TK_IND_D:addBufferBoldString(tkMessage,16,7,0);break;
		case HZ_IND_R:
		case HZ_IND_L:
		case HZ_IND_U:
		case HZ_IND_D:addBufferBoldString(hzMessage,16,7,0);break;
		default:break;
		}
		addBufferNumber(activeUnitsList[selectedUnit].hp,7,104);
		glcdDisplay();
		if(indianUnitsCount && enemyUnitsCount)
		{
			getJoystickResponse();
			getSwitchResponse();
			indianAction();
			enemyAction();
			updateActiveUnits();
			++gameCounter;
		}
		else
			currentScreen=RESULT;
		
		if(currentScreen!=GAMEPLAY)break;
	}
}
void exitGame()
{
		U8 data[]=" EXIT TO MAIN MENU   ";
		U8 foot[]="X:Exit    Y:Back";
		modifyBuffer(CLEAR);
		addBufferSmallString(data,21,2,0);
		addBufferBoldString(foot,16,7,0);
		addBufferChar('?',2,114);
		glcdDisplay();
	while(1)
	{
		getSwitchResponse();
		if(currentScreen!=EXITGAME)break;
	}
}
void resultScreen()
{
	U8 data[]="<Press any key> ";
	if(!enemyUnitsCount)
		modifyBuffer(VICTORY);
	else
		modifyBuffer(DEFEAT);
	glcdDisplay();
	delay_us(1000000);
	addBufferBoldString(data,16,7,0);
	glcdDisplay();
	while(1)
	{
		getSwitchResponse();
		if(currentScreen!=RESULT)
			break;
	}
}
int main()
{
	PINSEL0=0x00000000;//lcd and input
	PINSEL1=0x05000000;//joystick
	glcdInit();
	currentScreen=HOMELOGO;
	//menu switching
	while(1)
	{
		switch(currentScreen)
		{
			case HOMELOGO:homeScreen();break;
			case MAINMENU:mainMenu();break;
			case CAMPAIGN:campaignMenu();break;
			case HELP:helpScreen();break;
			case LV1MSG:
			case LV2MSG:
			case LV3MSG:
			case LV4MSG:
			case DMATCH:messageScreen();break;
			case GAMEPLAY:gameScreen();break;
			case EXITGAME:exitGame();break;
			case RESULT:resultScreen();break;
			default:break;
		}
	}
}
