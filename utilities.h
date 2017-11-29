#ifndef _UTILITIES_H
#define _UTILITIES_H

#define maxUnitsCount 20

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef signed char S8;
typedef signed short S16;
typedef signed long S32;

typedef struct position
{
	S16 posX,posY;
}POS;

typedef struct unit
{
	U8 type;//0 to 19
	POS source;
	POS destination;
	S16 hp;
} UNIT;

enum TEAM
{
	IND,
	PAK
};

enum SCREENS
{
	HOMELOGO,
	MAINMENU,
	CAMPAIGN,
	HELP,
	LV1MSG,
	LV2MSG,
	LV3MSG,
	LV4MSG,
	DMATCH,
	GAMEPLAY,
	EXITGAME,
	RESULT
};

enum UNITTYPES
{
	RM_IND_R,
	RM_IND_L,
	RM_PAK_R,
	RM_PAK_L,
	TK_IND_R,
	TK_IND_L,
	TK_IND_U,
	TK_IND_D,
	TK_PAK_R,
	TK_PAK_L,
	TK_PAK_U,
	TK_PAK_D,
	HZ_IND_R,
	HZ_IND_L,
	HZ_IND_U,
	HZ_IND_D,
	HZ_PAK_R,
	HZ_PAK_L,
	HZ_PAK_U,
	HZ_PAK_D
};

enum BUFDISPLAY
{
	CLEAR,
	HOME,
	VICTORY,
	DEFEAT
};

#endif
