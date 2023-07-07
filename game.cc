#define XREZ 240
#define YREZ 208


#define UZE  				1
#define SPACEAGE_EEPROM_ID 	23

#define X0 					0  //controller

/****************************************
 *			Library Dependencies		*
 ****************************************/

#include <uzebox.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>   //abs//
#include <avr/wdt.h>  //debug//


#define ___a 0
#define SECOND 60
#define OBJECT u8


/****************************************
 *			  	 Data					*
 ****************************************/
#include "space_ts.h"
#include "space_sp.h"
#include "space_levels.h"
#include "patches.h"
#include "music_pengo_music_die.h"
#include "music_pengo_music_start.h"
#include "music_pengo_a.h"
#include "music_telstar.h"



const char music_stop[] PROGMEM = {
0x00,0x90,0x10,0x00,		//note#0 on volume=0
0x00,0x91,0x10,0x00,		//note#1 on volume=0
0x00,0x92,0x10,0x00,		//note#2 on volume=0
0x00,0x93,0x10,0x00,		//note#3 on volume=0
0x00,0xFF,0x2F,0x00			//EOT
};

void	ugo(u8 x,u8 y);
void	uprintx(const char *);
void	ucprintx(const char *);
void	uprintc(u8 a);


void	do_idle(OBJECT *p);
void 	draw_idle(OBJECT *p);
void	boot();


typedef enum
{
	SD_NORMAL,
} SND;


typedef struct
{
	int x,y;
} int2;


u8 music_vol;
u8 free_slot;
u16 btnPressed = 0;
u16 btnHeld = 0;    		// Buttons that are held right now





#define HIDE_X (SCREEN_TILES_H<<3)

typedef struct {
	u8 x;
	u8 y;
} pt;


typedef struct {
  	u8 	spriteFlags;
	pt 	loc;
	u8 	spriteIndex;
	u8  idx;
} actor;

typedef struct {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;





extern	 int rand();

int ran(int lo,int hi)
{
	return(lo+rand()%(hi-lo+1));
}

//extern u8 *vram;

u8 TileRead(u8 x,u8 y)
{
	return(vram[x+VRAM_TILES_H*y]);	//-~(RAM_TILES_COUNT-1));
}

void DKMapSprite(u8 index, const char *map, u8 spriteFlags)
{
	u8 x,y,xStart,xEnd,xStep;
	u8 wid = pgm_read_byte(map), hgt = pgm_read_byte(map+1);

	index=free_slot;
	if (spriteFlags&SPRITE_FLIP_X)
 	{
		xStart = wid-1;
		xEnd = 255;
		xStep = -1;
	}
	else
   	{
		xStart = 0;
		xEnd = wid;
		xStep = 1;
	}

	for (y = 0; y < hgt; y++)
	{
		for (x = xStart; x < xEnd; x += xStep,index++)
		{
			if (index<16)
			{
				sprites[index].tileIndex = pgm_read_byte(&(map[(y*wid)+x+2]));
				sprites[index].flags = spriteFlags;
			}
		}
	}
	free_slot=index;
}

void DKDrawActors(actor *a, u8 count)
{
	for (u8 i = 0; i < count; ++i)
	{
	   	u8 sprite_index=a[i].idx;
	   	const char *data=(const char *)pgm_read_word(&sprite_table[sprite_index]);
	   	u8 w=pgm_read_byte(data);
	   	u8 h=pgm_read_byte(data+1);
       	MoveSprite(a[i].spriteIndex, a[i].loc.x, a[i].loc.y, w, h);
	}
}

void DKFill(const rect *r, u8 tileId)
{
	u8 x,y;

	for (y = r->top; y < r->btm; y+=TILE_WIDTH)
	{
    	for (x = r->left; x < r->right; x+=TILE_HEIGHT)
		{
            SetTile(x>>3,y>>3,tileId);
		}
    }
}


void	SetTileX( s16 x, s16 y, s8 index )
{
	u8 *data=(u8 *)pgm_read_word(&tsprite_table[index]);
	u8 xs = x&1;
	u8 ys = y&1;
	u8 a;
	a=pgm_read_byte(&data[2+xs+2*ys]);
	SetTile(x,y,a);
}


void	ClearMap(u8 val)
{
 	DKFill(&(rect){0,XREZ,0,YREZ}, val);
}

void	ClearMapY(u8 val,u8 y1,u8 y2)
{
 	DKFill(&(rect){0,XREZ,y1<<3,y2<<3}, val);
}


void	ClearSubMap(u8 val)
{
 	DKFill(&(rect){8,XREZ-8,2*8,YREZ-2*8}, val);
}


void	SPRITE(u8 index, u8 x, u8 y, u8 flip, u8 pal)
{
   	const char *data=(const char *)pgm_read_word(&sprite_table[index]);
   	u8 w=pgm_read_byte(data);
   	u8 h=pgm_read_byte(data+1);
   	MoveSprite(free_slot, x, y, w, h);
 	DKMapSprite(0,(char *)pgm_read_word(&sprite_table[index]),0);
}



void	sound(int a,int pri)
{
	TriggerFxChannel(2,a,255,1);
}

void	sound2(int a,int pri)
{
	TriggerFxChannel(1,a,255,1);
}



#define	JOY_LEFT	BTN_LEFT
#define	JOY_RIGHT	BTN_RIGHT
#define	JOY_DOWN	BTN_DOWN
#define	JOY_UP		BTN_UP
#define	JOY_A 		(BTN_A)
#define	JOY_B 		(BTN_B)
#define	JOY_X 		(BTN_X)
#define	JOY_Y 		(BTN_Y)
#define JOY_LS		(BTN_SL)
#define JOY_RS		(BTN_SR)
#define JOY_START	BTN_START
#define JOY_SELECT	BTN_SELECT


//#define	JOY_NOTA	(BTN_B|BTN_X|BTN_Y)
//#define	JOY_A		(BTN_A|BTN_B|BTN_X|BTN_Y)



#define pad(mask) (btnHeld & mask)
#define xpad(mask) (btnPressed & mask)







void TSPRITE(int index,int x,int y)
{
   	u8 i,j;
   	const char *data=(const char *)pgm_read_word(&tsprite_table[index]);
   	u8 w=pgm_read_byte(data++);
   	u8 h=pgm_read_byte(data++);

	x>>=3;
	y>>=3;


	for ( j=y; j<y+h; j++ )
	{
    	for ( i=x; i<x+w; i++ )
		{
            SetTile(i,j,pgm_read_byte(data++));
		}
    }
  //	DrawMap2(x,y,pgm_read_word(&tsprite_table[a]));
}


u8 TSPRITE_TILE(u8 index)
{
   	const char *data=(const char *)pgm_read_word(&tsprite_table[index]);
   	u8 a = pgm_read_byte( &data[2] );
	return( a );
}

u8 TSPRITE_GET_INDEX(u8 index,u8 offset)
{
   	const char *data=(const char *)pgm_read_word(&tsprite_table[index]);
   	u8 w=pgm_read_byte(&data[2+offset]);
	return(w);
}



void TSPRITE_BLANK(int index,int x,int y)
{
   	u8 i,j;
   	const char *data=(const char *)pgm_read_word(&tsprite_table[index]);
   	u8 w=pgm_read_byte(data++);
   	u8 h=pgm_read_byte(data++);

    x>>=3;
	y>>=3;


	for ( j=y; j<y+h; j++ )
	{
    	for ( i=x; i<x+w; i++ )
		{
            SetTile(i,j,0);
		}
    }
}

void TSPRITE_DIRECT(u8 *data,int x,int y)
{
   	u8 i,j;
   	u8 w=pgm_read_byte(data++);
   	u8 h=pgm_read_byte(data++);

	x>>=3;
	y>>=3;


	for ( j=y; j<y+h; j++ )
	{
    	for ( i=x; i<x+w; i++ )
		{
            SetTile(i,j,pgm_read_byte(data++));
		}
    }
  //	DrawMap2(x,y,pgm_read_word(&tsprite_table[a]));
}



u8	P_strlen(char *text)
{
	u8 a;
	u8 c=0;
	while(1)
	{
		a=pgm_read_byte(text);
	  	if (a==0) break;
		text++;
		c++;
	}
	return(c);
}

void	Sprites_HideRest()
{
 	u8 i;
 	for (i = free_slot; i < 16; i++)
 	{
 		sprites[i].x = HIDE_X;
 	}
}

void DKPrintNumber(u8 x, u8 y, u32 val, u8 digits,u8 type)
{
	u8 numeral;
	for (u8 i = 0; i < digits; i++, val /= 10)
	{
		numeral = val % 10;
	}
}



void  	ds_setts( char *data, int pengo_logo_data, short *smap )
{
	SetTileTable((char *)data);
}


void play_music(int a)
{
	a&=0x7FFF;
	switch(a)
	{
	 	case MUSIC_INGAME:
			if (music_vol)
   			//StartSong(music_pengo_music_ingame_sega);
				StartSong(music_telstar);	//pengo_ch0_a);
			else
   		  		StartSong(music_stop);
			break;
	    case MUSIC_DIE:
 		   	StartSong(music_pengo_music_die);
			break;
		case MUSIC_START:
  			StartSong(music_pengo_music_start);
			break;
	   	default:
   		  	StartSong(music_stop);
		  	break;
	}
}





int main(void)
{
	SetTileTable((char *)ts_data);
	SetSpritesTileTable((char *)sp_data);
	SetSpriteVisibility(true);
	InitMusicPlayer(patches);
	SetMasterVolume(0xc0);
	StopSong();

	DKFill(&(rect){0,XREZ,0,YREZ}, 0);

	boot();

	while(1)
	{
		if (GetVsyncFlag())
	   	{
			ClearVsyncFlag();
			btnPressed = btnHeld;
			btnHeld = ReadJoypad(X0);
			btnPressed = btnHeld&(btnHeld^btnPressed);

		   	do_idle(0);

			free_slot=0;
		   	draw_idle(0);

			Sprites_HideRest();
		}
	}
}

#define BPTR(a)  (pgm_read_byte(a))
#define WPTR(a)  (pgm_read_word(a))

#define GM_SPACE 	1

typedef struct
{
	s8 x,y;
} s2;

#define smap 0

#include "space_game.h"
#include "space_game.cpp"


