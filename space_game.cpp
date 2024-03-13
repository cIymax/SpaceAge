//#define	CHEAT 				1
#define START_LIVES 		3


typedef enum
{
	MAP_NULL,  	//0
	MAP_ENERGY, //1
	MAP_FLOOR,  //2
	MAP_STARS,  //3
	MAP_ORB,	//4
	MAP_TARGET,	//5
 	MAP_WALL,	//6
	MAP_OXYGEN,	//7
} MAP;

typedef enum
{
	BLOCK_FLOOR,
	BLOCK_PLAYER,
	BLOCK_ICE,
	BLOCK_INVISIBLE,
	BLOCK_SUPA,
 	BLOCK_SUPAB,
 	BLOCK_SUPAC,
	BLOCK_OXYGEN,
	BLOCK_WALL,
	BLOCK_HOLE,
    BLOCK_SUPAFIXED,
} BLOCKTYPE;





#define KICK_DELAY 						10
#define DELAY_KICK_WALL  				10
#define TIME_WALL_WOBBLE				30
#define TIME_SUPA_BADDIE_STUN			(60*8)
#define TIME_BADDIE_STUN				(60*3)

#define TIME_EGG_START_LO				(60*1)
#define TIME_EGG_START_HI				(60*3)

#define TIME_STARS						400




#define SCORE_KILL_BADDIE_WITH_BLOCK 	40
#define SCORE_KILL_STUNNED_BADDIE 	 	10
#define SCORE_BREAK_BLOCK				3
#define SCORE_BONUS_ADD					10

#define MUL8(a) ((a)<<3)
#define DIV8(a) ((a)>>3)


#define MUL16(a) ((a)<<4)
#define DIV16(a) ((a)>>4)



#define MAPW 13
#define MAPH 11

#define PANELY 	0	//1
#define TOPY 	1	//2
#define BOTY (TOPY+2*MAPH+2)
#define TOPX ((30-2*MAPW-2)/2)
#define xoff ((TOPX+1)*8)
#define yoff ((TOPY+1)*8)

#ifdef PLATFORM_DS
#define MAPX 32
#else
#define MAPX 30
#endif

#define XWIN 240
#define YWIN 208

#define XGAP	16
#define YGAP	16


#if defined(GM_SPACE) || defined(GM_ALL)


#define MAX_HISCORE 5
#define NAMESIZE 	4
typedef struct
{
	unsigned short val;
	char name[NAMESIZE];
} SCORE;


SCORE *scores;


#define TWOPLAYER 1


#ifdef TWOPLAYER
#define MAXPLAYERS 2
#else
#define MAXPLAYERS 1
#endif


#ifndef UZE
#include	"xx.h"
#include 	"exec.h"
#include	"game.h"
#include	"win.h"
#include	"sound.h"
#include	"debug.h"
#include	"vkcodes.h"
#include	"font.h"
#include	"bbc.h"
#include	"sys.h"
#include	"resource.h"
#include	"sounds.h"
#include	"useful.h"
#include	"cmaths.h"
#include	"pause.h"
#include 	"space_ts.h"
#include 	"space_sp_gfx.h"
#include 	"space_game.h"
#include 	"space_levels.h"


#define XREZ ((u8) xrez)
#define YREZ ((u8) yrez)

#define BPTR(a)	(*(a))


// externals
extern u16 pal332[];

struct EepromBlockStruct{
		//some unique block ID assigned by ?. If 0xffff, block is free.
		unsigned char id;
		unsigned char id2;

		//application specific data
		//cast to your own types
		unsigned char data[30];
	};


#define SPACEAGE_EEPROM_ID	15

EepromBlockStruct eeprom_memory;

char EepromWriteBlock(struct EepromBlockStruct *block)
{
	memcpy( &eeprom_memory,block,sizeof(EepromBlockStruct));
	return 0;
}

/*
 * Reads a data block in the specified structure.
 *
 * Returns: 0 on success or error codes
 */
char EepromReadBlock(u16 blockId,struct EepromBlockStruct *block)
{
	static char first=1;
	char res=first;
	first=0;
	blockId=blockId;
	memcpy( block,&eeprom_memory,sizeof(EepromBlockStruct));
	return res;
}


void 	ResumeSong()
{

}

void	StopSong()
{

}


void sound2(u8 index,u8 pri)
{
	index=index;
	pri=pri;
}


u8		music_vol;
u16 	smap[2*32*32];

u8 TSPRITE_GET_INDEX(u8 index,u8 offset)
{
   	const char *data=(const char *)tsprite_table[index];
    return(data[2+offset]);
}

void	TSPRITE_DIRECT( u8 *data, s16 x, s16 y )
{
	u8 w=*data++;
	u8 h=*data++;
	s16 i,j;

	x>>=3;
	y>>=3;

	for( j=0; j<h; j++ )
	for( i=0; i<w; i++ 	)
		smap[x+i+MAPX*(y+j)]=*data++;
}

u8		TSPRITE_TILE( u8 index )
{
	u8 *data=(u8 *)tsprite_table[index];
	return(data[2]);
}



void	TSPRITE( s16 index, s16 x, s16 y )
{
	u8 *data=(u8 *)tsprite_table[index];
	u8 w=*data++;
	u8 h=*data++;
	s16 i,j;

	x>>=3;
	y>>=3;

	for( j=0; j<h; j++ )
	for( i=0; i<w; i++ 	)
		smap[x+i+MAPX*(y+j)]=*data++;
}

void	SetTileX( s16 x, s16 y, s8 index )
{
	u8 *data=(u8 *)tsprite_table[index];
	u8 w = *data++;
	u8 h = *data++;
	u8 xs = x&1;
	u8 ys = y&1;

	smap[x+MAPX*(y)]=data[xs+2*ys];
}


void	TSPRITE_BLANK( s16 index, s16 x, s16 y )
{
	u8 *data=(u8 *)tsprite_table[index];
	u8 w=*data++;
	u8 h=*data++;
	s16 i,j;

	x>>=3;
	y>>=3;

	for( j=0; j<h; j++ )
	for( i=0; i<w; i++ 	)
		smap[x+i+MAPX*(y+j)]=0;
}



void	ClearMap(u8 val)
{
  	s16 x,y;
   	for(y=0;y<YREZ/8;y++)
   	for(x=0;x<XREZ/8;x++)
   		smap[x+MAPX*y]=val;
}

void	ClearMapY(u8 val,u8 y1,u8 y2)
{
  	s16 x,y;
   	for(y=y1;y<y2;y++)
   	for(x=0;x<XREZ/8;x++)
   		smap[x+MAPX*y]=val;
}


void	ClearSubMap(u8 val)
{
  	s16 x,y;
   	for(y=1;y<YREZ/8-2;y++)
   	for(x=1;x<XREZ/8-3;x++)
   		smap[x+MAPX*y] = val;
}


void SetTile( u8 x, u8 y, u8 a )
{
	smap[x+MAPX*y] = a;
}


u8 TileRead( u8 x, u8 y )
{
	return( smap[x+MAPX*y] );
}



#define P_strlen(a) strlen(a)

#endif







const char txt_paused[] PROGMEM = "PAUSED";
const char txt_continue_exit1[] PROGMEM = " CONTINUE ";
const char txt_continue_exit2[] PROGMEM = " EXIT ";


const char txt_pjones[] PROGMEM = "@ JONES 2013";
const char txt_get_ready[] PROGMEM = "ACTIVATE";
const char txt_blank10[] PROGMEM = "               ";
const char txt_blank3[] PROGMEM = "   ";
const char txt_level[] PROGMEM = "ENTERING SECTOR";
const char txt_youareinthetopten[] PROGMEM = "YOU ARE IN THE TOP FIVE";
const char txt_congratulations[] PROGMEM = "CONGRATULATIONS";
const char txt_playerone[] PROGMEM= "PLAYER ONE";
const char txt_playertwo[] PROGMEM= "PLAYER TWO";
const char txt_1player[] PROGMEM= "1 PLAYER ";
const char txt_2player[] PROGMEM= "2 PLAYER ";
const char txt_please_enter_your_name[] PROGMEM = "PLEASE ENTER YOUR NAME";
const char txt_game_over[] PROGMEM = "GAME OVER";
const char his_lets[] PROGMEM ="ABCDEFGHIJKLMNOPQRSTUVWXYZ.";
const char txt_1up[] PROGMEM ="1UP";
const char txt_2up[] PROGMEM ="2UP";
const char txt_sector[] PROGMEM ="S:";
const char txt_lives[] PROGMEM ="LIVES";
const char txt_hi[] PROGMEM = "HI";
const char txt_sound[] PROGMEM= "SOUND ";
const char txt_level_completed[] PROGMEM= "SECTOR EXPURGATED";
const char txt_time_bonus[] PROGMEM= "TIME BONUS";
const char txt_music_on[] PROGMEM=   " MUSIC ON ";
const char txt_music_off[] PROGMEM=  " MUSIC OFF";

//expansion
const char txt_forker[] PROGMEM = "DIRECTOR'S CUT BY CLYMAX";
const char txt_RA[] PROGMEM = "FOR RETROACHIEVEMENTS.ORG";



//const char *txt_player[2] PROGMEM =
const char * const txt_player[2] PROGMEM =
{
	txt_playerone,
	txt_playertwo
};


u8 game_grid[256];

#define rmap(x,y) game_grid[x+MUL16(y)]

#define hiscore_mem game_grid


// enums and defines
typedef enum
{
	STATE_PREMENU,
	STATE_MAINMENU,
	STATE_PREGAME,
	STATE_FADER_START,
	STATE_FADER,
	STATE_INGAME,
	STATE_DIAMONDS,
	STATE_POSTGAME,
	STATE_RESTART,
	STATE_GAMEOVER,
	STATE_ENTERNAME,
	STATE_HISCORE,
	STATE_PAUSED,
} STATE;


typedef enum
{
	DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
} DIRS;

typedef enum
{
	WALL_TOP=1,
    WALL_RIGHT=2,
    WALL_BOT=4,
    WALL_LEFT=8,
} WALLS;

typedef enum
{
	TILESET_TITLE,
	TILESET_GAME,
} TILESETS;


#define 	PLATFORM 1

typedef enum
{
	BADDIE_DEAD,
	BADDIE_EGG,
	BADDIE_ALIVE,
	BADDIE_MOVE,
 	BADDIE_STUN,
	BADDIE_CRUSH,
	BADDIE_PUSHED,
	BADDIE_LIMBO,
	BADDIE_POP,
} BADDIE_MODES;

typedef enum
{
	PLAYER_DEAD,
	PLAYER_ALIVE,
 	PLAYER_MOVE,
	PLAYER_DIE,
} PLAYER_MODES;

typedef enum
{
	BLOCK_DEAD,
	BLOCK_ALIVE,
	BLOCK_CRUSH,
	BLOCK_REMOVE,
} BLOCK_MODES;


#define DIRTY_P1 		1
#define DIRTY_P2 		2
#define DIRTY_HISCORE 	4

#define FADER_DELAY	2


typedef struct
{
	u8 width;
	u8 height;
	u8 xpos;
	u8 ypos;
	u8 size;
	u8 maxsize;
	u8 color;
	u8 basecolor;
	s8 trigger;
	u8 ctrl;
} FADER;

FADER faders[1];


// variables ...
s8 baddies_active;
u8 sound_index;
u8 pend_state;
u8 TILE_FADER;
u8 bullet_slot=0;
u8 bullet_x[4];
u8 bullet_oldx[4];
u8 bullet_oldy[4];
u8 bullet_y[4];
u8 bullet_dir[4];
s8 bullet_life[4];
u8 eggCount;
u8 swan_exp[4];

s8 	supa_done;
s8 	continue_exit;
s8	pend_gtimer;
u16 g_bonus;
s8 	current_player;
u8 	number_of_players;
s8 	lives[MAXPLAYERS];
u16 score[MAXPLAYERS];
u8 	level[MAXPLAYERS];
u8 	dirty;
u8 	g_oxygen;

// overlay these ??
s8 his_pos;
s8 his_rank;
s8 his_let;
//

u8 paused;
u8 state;
u8 font_base;


#define MAX_BADDIE 3

s16 baddie_x[MAX_BADDIE];
s16 baddie_y[MAX_BADDIE];
u8 baddie_ctrl[MAX_BADDIE];
u8 baddie_callb[MAX_BADDIE];
u8 baddie_sprite[MAX_BADDIE];
u8 baddie_frame[MAX_BADDIE];
u8 baddie_dir[MAX_BADDIE];
s16 baddie_trigger[MAX_BADDIE];
s8 baddie_gtimer[MAX_BADDIE];
u8 uxtab,uytab;

u8 supa_x[4];
u8 supa_y[4];
u8 supa_move[4];
s8 supa_trigger[4];
u8 supa_count;

s16 gtimer;
u8 g_hitwall;
u8 g_oldwall;
s16 g_wallgtimer;
s16 hiscore;
s16 g_frame;


#define MAX_DIGIT 5
char digits[MAX_DIGIT];

int	player_x;
s16 player_y;
u8 player_dir;
u8 player_frame;
u8 player_ctrl;
s8 player_gtimer;
s8 player_kicked;
s8 player_trigger;

u8 snoblock_x;
u8 snoblock_y;
u8 snoblock_frame;
s8 snoblock_trigger;
u8 snoblock_ctrl;


u8 block_x;
u8 block_y;

u8 block_dir;
u8 block_frame;
u8 block_ctrl;
s8 block_count;

u8 block_type;
u8 block_trigger;
u8 block_sprite;
u8 block_oldx;

u8 block_oldy;
u8 block_oldsprite;
u8 block_index;

// could be in progmem
s2 spd[]=
{
	{0,-1},
	{1,0},
	{0,1},
	{-1,0}
};


const u8 frames_pengo_walk[] PROGMEM =
{
	PENGO_WALK_UP_A_SP,
    PENGO_WALK_RIGHT_A_SP,
    PENGO_WALK_DOWN_A_SP,
    PENGO_WALK_LEFT_A_SP,
};


const u8 frames_baddie_walk[] PROGMEM =
{
	BADDIE_U_A_SP,
    BADDIE_R_A_SP,
    BADDIE_D_A_SP,
    BADDIE_L_A_SP,

};

// expansion
bool inputEdge = false;

void	tile_frame( int a, int x1, int y1, int x2, int y2 )
{
	int x,y;

	for(x=x1;x<=x2;x++)
	{
		SetTile(x,y1,a);
		SetTile(x,y2,a);
	}

  	for(y=y1;y<=y2;y++)
	{
		SetTile(x1,y,a);
		SetTile(x2,y,a);
	}
}



void	draw_fader( u8 index )
{
	FADER *p = &faders[index];
   	int xm = p->xpos;
	int ym = p->ypos;
	int x1,y1,x2,y2;
	int ygap = p->size+p->height;
	int xgap = ygap+p->width;

	if (p->ctrl==2) return;

	x1=xm-xgap-1;
	x2=xm+xgap;
	y1=ym-ygap-1;
	y2=ym+ygap;

	if (p->color==0)
	tile_frame( 0 , x1, y1, x2, y2 );
	else
    tile_frame( p->color + TILE_FADER , x1, y1, x2, y2 );
}




void	do_fader(u8 index)
{
    int xxlat[]={ 1,5,2,6 };

	FADER *p = &faders[index];

	switch(p->ctrl)
	{
		case 0:
			p->color = xxlat[(p->basecolor+p->size)&3];
			p->trigger--;
			if (p->trigger<=0)
			{
				p->trigger = FADER_DELAY;
				p->size++;
				if (p->size == p->maxsize-1)
				{
			    	p->trigger = 5*FADER_DELAY;
				}
				if (p->size >= p->maxsize)
				{
					p->size = 0;
					p->color = 0;
					p->ctrl = 1;
				}
			}
			break;
		case 1:
			p->trigger--;
			if (p->trigger<=0)
			{
				p->trigger=FADER_DELAY;
				p->size++;
				if (p->size>=p->maxsize)
				{
					p->ctrl=2;
					p->size--;
				}
			}
			break;
		default:
			break;
	}
}


void	start_fader0(u8 index,int xm,int ym,int w,int h,int lim,int col)
{
	FADER *p=&faders[index];

   	p->size = 0;
   	p->color = 0;
	p->maxsize = lim;
	p->basecolor = col;
   	p->trigger = FADER_DELAY;
   	p->xpos = xm;
   	p->ypos = ym;
   	p->width = w;
   	p->height = h;
	p->ctrl = 0;
}


void	start_fader_single()
{
    int xm=XREZ/16;
	int ym=YREZ/16;
	start_fader0(0,xm,ym,xm-ym,0,13,0);
}


void	start_fader()
{
    int xt1=6;
	int y14=YREZ/(16*2);

	start_fader0( 0, xt1,	y14,		1,0,	5,0);
}




void	add_score(u16  val)
{
	score[current_player]+=val;
	if (score[current_player]>hiscore)
	{
		hiscore=score[current_player];
		dirty|=DIRTY_HISCORE;
	}
	dirty|=DIRTY_P1+current_player;
}


SCORE *hiscore_load_from_eeprom(u8 *ram)
{
	struct EepromBlockStruct *ebs = (struct EepromBlockStruct*)ram;
	ebs->id = SPACEAGE_EEPROM_ID;
   	scores = (SCORE *)&ebs->data[0];

	// if read failed return non zero so init the hiscore
	if (EepromReadBlock(ebs->id, ebs)!=0)
	{
    	uze_hiscore_init();
	   	EepromWriteBlock(ebs);
	}
	return(scores);
}

void	hiscore_save_to_eeprom(u8 *ram)
{
	struct EepromBlockStruct *ebs = (struct EepromBlockStruct*)ram;
	ebs->id = SPACEAGE_EEPROM_ID;
	EepromWriteBlock(ebs);
}


void ugo(u8 x,u8 y)
{
	uxtab = x;
	uytab = y;
}

void uprintc(u8 a)
{
	u8 x=uxtab>>3;
	u8 y=uytab>>3;

	if (a<10) a+='0';
	if (a>=32 && a<='Z')
	{
		a = font_base+TSPRITE_GET_INDEX(A1_CHARSET_TSP,a-32);
		SetTile(x,y,a);
	}
	uxtab+=8;
	if (uxtab>=XREZ)
	{
		uxtab=0;
		uytab+=8;
	}
}

void ucprintx(const char *text)
{
	char a;

	u8 len=(u8) P_strlen((char*)text);
	uxtab-=MUL8(len)>>1;

	while(1)
	{
		a=BPTR(text);
		text++;
		if (a==0) break;
		uprintc(a);
	}
}

void ucprintx_blank(const char *text)
{
	char a;

	u8 len=(u8) P_strlen((char*)text);
	uxtab-=MUL8(len)>>1;

	while(1)
	{
		a=BPTR(text);
		text++;
		if (a==0) break;
		uprintc(32);
	}
}


void ucprintx_dot(const char *text)
{
	char a;

	u8 len=(u8) P_strlen((char*)text);
	uxtab-=MUL8(len)>>1;

	u8 k=(g_frame>>2)&15;
	u8 pos=0;
	while(1)
	{
		a=BPTR(text);
		text++;
		if (a==0) break;
		if (k==pos) a='?';
		uprintc(a);
		if (k==pos) break;
		pos++;
	}
}

void uprintx(const char *text)
{
	char a;
	while(1)
	{
		a=BPTR(text);
		text++;
		if (a==0) break;
		uprintc(a);
	}
}

void printx_ram(const char *text)
{
	char a;

	while(1)
	{
		a=*text;
		text++;
		if (a==0) break;
		uprintc(a);
	}
}


u8 decimal_sub2( char *digit, u16  inval )
{
  u8 i;
  s16 val;

  if (inval>=60000)
  {
  	val=inval-60000;
  	i=6;
  }
  else if (inval>=30000)
  {
  	val=inval-30000;
  	i=3;
  }
  else
  {
  	val=(int)inval;
  	i = 0;
  }

  while(val>0)
  {
    i++;
	val-=10000;
  }
  if (val<0) { i--; val+=10000; }
  digit[4] = i;

   // 1000s
  i = 0;
  while(val>0)
  {
    i++;
	val-=1000;
  }
  if (val<0) { i--; val+=1000; }
  digit[3] = i;

  // 100s
  i = 0;
  while(val>0)
  {
    i++;
	val-=100;
  }
  if (val<0) { i--; val+=100; }
  digit[2] = i;

  // tens
  i = 0;
  while(val>0)
  {
    i++;
	val-=10;
  }
  if (val<0) { i--; val+=10; }
  digit[1] = i;

  digit[0] = (char) val;

  return(i+1);
}


u8 decimal_sub3(char *dig,u16  num)
{
	u8 i;

	memset(dig,0,MAX_DIGIT);

  	for(i=0;i<MAX_DIGIT;i++)
	{
	 	dig[i] = (u8) (num % 10);
	  	num=num/10;
 		if (num==0) break;
	}
	return(i+1);
}



void print_decimal(u16  val,u8 num)
{
	s8 i;

   	decimal_sub2(digits,val);

  	for(i=num-1;i>=0;i--)
   		uprintc(digits[i]);
}

void print_digit(u8 val)
{
	if (val<10)
		uprintc(val);
	else
    	uprintc(val+'A'-10);
}

void print_byte(u8 val)
{
	print_digit(val>>4);
	print_digit(val&15);
}







void print_score(u16  val,u8 num)
{
	s8 i;

	decimal_sub2(digits,val);

   	if (digits[num-1]!=0)
   	{
   		uxtab-=8;
   		uprintc(digits[num-1]);
   	}

	for(i=num-2;i>=0;i--)
   		uprintc(digits[i]);
    uprintc('0');
}




#define HISCORE_GAPY	16
#define HISCORE_STARTY (96+16)

void draw_hiscore()
{
	u8 i;

	if (scores==0)
	{
		scores=hiscore_load_from_eeprom( hiscore_mem );
	}


	for(i=0;i<MAX_HISCORE;i++)
	{
		ugo(16,HISCORE_GAPY*i+HISCORE_STARTY);
		print_decimal(i+1,1);

	    ugo(XREZ/2-2*8,HISCORE_GAPY*i+HISCORE_STARTY);
		print_score(scores[i].val,5);

	   	ugo(XREZ-40,HISCORE_GAPY*i+HISCORE_STARTY);
	   	printx_ram(scores[i].name);
	}
}

void draw_enter()
{
	ClearMap(0);

	ugo(XREZ/2,8);
	ucprintx(txt_congratulations);

    ugo(XREZ/2,4*8);
	ucprintx(txt_youareinthetopten);

    ugo(XREZ/2,7*8);
	ucprintx(txt_please_enter_your_name);
}


#define ENTER_STARTY 5*16


void uze_hiscore_enter()
{
	u8 i=his_rank;
	const u8 xpos = (XREZ/2)-16;

	if (xpad(JOY_A) && his_pos<3)
	{
		sound(SFX_HISCORE_ENTER,SD_NORMAL);
	  	his_pos++;
	}
	gtimer--;

    if (xpad( (JOY_LEFT|JOY_RIGHT) ))  gtimer=0;

	if (gtimer<=0)
	{
 		if (pad(JOY_LEFT)) { sound(SFX_HISCORE_CLICK,SD_NORMAL); gtimer=10; his_let--;  }
   		if (pad(JOY_RIGHT)) { sound(SFX_HISCORE_CLICK,SD_NORMAL);gtimer=10; his_let++; }
   		if (his_let<0) his_let=26;
   		if (his_let>26) his_let=0;
	}
	if (his_pos<=2)
 		scores[i].name[his_pos]=BPTR(&his_lets[his_let]);

   	ugo(xpos,ENTER_STARTY);
	printx_ram(scores[i].name);

   	ugo(xpos,8+ENTER_STARTY);
	uprintx(txt_blank10);


	// blank out current letter
	if (his_pos<=2)
    if ((g_frame>>3)&1)
	{
	   	ugo(xpos+MUL8(his_pos),8+ENTER_STARTY);
		uprintc('_');
	}

	// end of hiscore enter
	if (his_pos>=3)
	{
		hiscore_save_to_eeprom( hiscore_mem );
		state=STATE_HISCORE;
		gtimer=6*SECOND;
		draw_title();
	}
}



s8	uze_hiscore_getrank(s16 newscore)
{
	s8 	new_position;
	s8 	i;

	new_position=-1;
	for (i=0;i<MAX_HISCORE;i++)
	{
		if (newscore > scores[i].val)
		{
			new_position=i;
			break;
		}
	}
	return(new_position);
}

int	uze_hiscore_insert(s16 newscore)
{
	s16 	new_position;
	s16 	i;

	new_position = uze_hiscore_getrank(newscore);
	if (new_position==-1) return(-1);

	// shift all scores down

	if (new_position!=MAX_HISCORE-1)
	{
		for (i=MAX_HISCORE-2;i>=new_position;i--)
		{
			scores[i+1].val = scores[i].val;
			memcpy(&scores[i+1].name[0],&scores[i].name[0],NAMESIZE);
		}
  	}

	// add in new score
   scores[new_position].val = (unsigned short) newscore;
   scores[new_position].name[0]='.';
   scores[new_position].name[1]='.';
   scores[new_position].name[2]='.';
   scores[new_position].name[3]=0;

   return(new_position);
}

void	hiscore_check(s16 newscore)
{
	his_rank=uze_hiscore_getrank(newscore);
	if (his_rank>=0)
	{
	   	uze_hiscore_insert(newscore);
	  	gtimer=0;
		his_pos=0;
		his_let=0;
	   	state=STATE_ENTERNAME;
		ClearMap(0);
		draw_enter();
	}
	else
	{
		gtimer=3*SECOND;
		state=STATE_HISCORE;
	   	ClearMap(0);
		draw_title();
		draw_hiscore();
	}
}


void uze_hiscore_init()
{
	u8 i;

	for ( i=0; i<MAX_HISCORE; i++ )
	{
		scores[i].val = (unsigned short) 10*(MAX_HISCORE-i);
		scores[i].name[0]='U';
		scores[i].name[1]='Z';
		scores[i].name[2]='E';
		scores[i].name[3]=0;
	}
}


void	respawn(u8 i)
{
	s16 x,y;


again4:
	x=ran(0,MAPW-1);
	y=ran(0,MAPH-1);
	if (rmap(x,y)!=BLOCK_FLOOR) goto again4;

	start_baddie(i);
	baddie_x[i] = MUL16(x);
	baddie_y[i] = MUL16(y);
}



void	hit_block_vs_baddie()
{
	u8 i;
	s16 dx,dy;

	for(i=0;i<MAX_BADDIE;i++)
	{
		if (baddie_ctrl[i]!=BADDIE_DEAD && baddie_ctrl[i]!=BADDIE_LIMBO)
		{
			dx=baddie_x[i]-block_x;
			dy=baddie_y[i]-block_y;
			if (abs(dx)<8 && abs(dy)<8)
			{
			  //  baddie_gtimer[i]=SECOND;
			  //	baddie_ctrl[i]=BADDIE_LIMBO;
			  //	baddie_frame[i]=BADDIE_400_SP;
			  //	add_score(SCORE_KILL_BADDIE_WITH_BLOCK);
			  //	sound(SFX_SNOBEE_DIE,SD_NORMAL);
			  //	respawn(i);
			}
		}
	}
}

void	player_start_die()
{
   	player_ctrl = PLAYER_DIE;
   	player_trigger = 0;
   	player_gtimer = 2*SECOND;
   	play_music(0x8000|MUSIC_DIE);
}


void	hit_player_vs_baddie()
{
	u8 i;
	s16 dx,dy;

	for(i=0;i<MAX_BADDIE;i++)
	{
			if (baddie_ctrl[i]==BADDIE_DEAD) continue;
			if (baddie_ctrl[i]==BADDIE_LIMBO) continue;
			if (baddie_ctrl[i]==BADDIE_POP) continue;
		    if (baddie_ctrl[i]==BADDIE_EGG) continue;


			dx=baddie_x[i]-player_x;
			dy=baddie_y[i]-player_y;
			if (abs(dx)<8 && abs(dy)<8)
			{
				if (baddie_ctrl[i]==BADDIE_STUN)
				{
					baddie_gtimer[i]=SECOND;
					baddie_ctrl[i]=BADDIE_LIMBO;
					baddie_frame[i]=BADDIE_100_SP;
					add_score(SCORE_KILL_STUNNED_BADDIE);
					sound(SFX_SNOBEE_DIE,SD_NORMAL);
				   	respawn(i);
				}
				else
				{
			 	  	baddie_ctrl[i] = BADDIE_DEAD;
					player_start_die();
			   	  	return;
				}
			}
	}
}




void	draw_player()
{
	s16 x,y,f,k;
	u8 offset=0;


	f=(player_frame>>2)&1;
	k=BPTR(&frames_pengo_walk[player_dir]);


	// push
   	if (player_kicked && ((g_frame>>1)&1))
	{
		return;
	}

	x=player_x+xoff;
	y=player_y+yoff;
	SPRITE(k+f+offset,x,y,0,0);
}

void	draw_baddie(s16 idx)
{
	s16 x,y,f,k;

	x=baddie_x[idx]+xoff;
	y=baddie_y[idx]+yoff;

	if (baddie_ctrl[idx]==BADDIE_DEAD) return;

    if (baddie_ctrl[idx]==BADDIE_LIMBO)
    {
    	SPRITE(baddie_frame[idx],x,y,0,0);
		return;
	}

	// egg state
  	if (baddie_ctrl[idx] == BADDIE_EGG)
    {
		if (baddie_trigger[idx]>2*SECOND) return;

		if (((g_frame+idx)&3)==0)
		{
			baddie_frame[idx]+=ran(1,2);
			baddie_frame[idx]&=3;
		}

		f = baddie_frame[idx];
		if (f<4)
		SPRITE(BEAM_02_SP+f,x,y,0,0);
		else
	    SPRITE( BADDIE_D_A_SP,x,y,0,0);
		return;
	}

	if (baddie_ctrl[idx]==BADDIE_POP)
	{
		k = CH90_EXP_A_SP + baddie_frame[idx];
		SPRITE( k ,x, y, 0, 0);
		return;
	}
	else if (baddie_ctrl[idx]==BADDIE_STUN)
	{
		k = baddie_sprite[idx];
	}
	else
	{
		k = BPTR(&frames_baddie_walk[baddie_dir[idx]]);
	}
	f=(baddie_frame[idx]>>2)&1;
	SPRITE( k + f,x,y,0,0);
}


int		read_map(s16 x,s16 y)
{
	if (x<0) return(0x80|WALL_LEFT);
	if (x>=MAPW)return(0x80|WALL_RIGHT);
	if (y<0) return(0x80|WALL_TOP);
	if (y>=MAPH) return(0x80|WALL_BOT);
	return(rmap(x,y)&15);
}

int		read_map_index(s16 x,s16 y)
{
	if (x<0) return(0x80|WALL_LEFT);
	if (x>=MAPW)return(0x80|WALL_RIGHT);
	if (y<0) return(0x80|WALL_TOP);
	if (y>=MAPH) return(0x80|WALL_BOT);
	return((rmap(x,y)>>4)&15);
}



void	set_map(s16 x,s16 y,u8 a)
{
	if (x<0|| x>=MAPW) return;
	if (y<0|| y>=MAPH) return;
	rmap(x,y) = a;
}






void	player_walk()
{
  	player_frame++;
  	player_x+=2*spd[player_dir].x;
  	player_y+=2*spd[player_dir].y;
	player_gtimer--;
	player_gtimer--;
	if (player_gtimer<=0)
	{
		player_ctrl=PLAYER_ALIVE;
	}
}



void trigger_stars()
{
	state = STATE_DIAMONDS;
 	gtimer = TIME_STARS;
}

void check_supas()
{
	s16 dx01,dx12,dx20;
	s16 dy01,dy12,dy20;

	supa_count=0;
   	dx01 = abs(supa_x[0]-supa_x[1]);
	dx12 = abs(supa_x[1]-supa_x[2]);
	dx20 = abs(supa_x[2]-supa_x[0]);
   	dy01 = abs(supa_y[0]-supa_y[1]);
	dy12 = abs(supa_y[1]-supa_y[2]);
	dy20 = abs(supa_y[2]-supa_y[0]);

	if (dx01&dx12&dx20&dy01&dy12&dy20)
		return;

	dx01>>=4;
	dx12>>=4;
	dx20>>=4;
    dy01>>=4;
	dy12>>=4;
	dy20>>=4;

	if (dx01==0 && dx12==0 && dy01<=1 && dy12<=1)
	{
	   supa_count=2;
	   return;
	}
    if (dx01==0 && dx20==0 && dy01<=1 && dy20<=1)
	{
	   supa_count=2;
	   return;
	}
    if (dx12==0 && dx20==0 && dy12<=1 && dy20<=1)
	{
	   supa_count=2;
	   return;
	}

    if (dy01==0 && dy12==0 && dx01<=1 && dx12<=1)
	{
	   supa_count=2;
	   return;
	}

	if (dy01==0 && dy20==0 && dx01<=1 && dx20<=1)
	{
	   supa_count=2;
	   return;
	}

    if (dy12==0 && dy20==0 && dx12<=1 && dx20<=1)
	{
	   supa_count=2;
	   return;
	}


	if (dx01==0 && dy01<=1)
	{
	   supa_count=1;
	   return;
	}
    if (dx12==0 && dy12<=1)
	{
       supa_count=1;
	   return;
	}
    if (dx20==0 && dy20<=1)
	{
       supa_count=1;
	   return;
	}
    if (dy01==0 && dx01<=1)
	{
       supa_count=1;
	   return;
	}
    if (dy12==0 && dx12<=1)
	{
	   supa_count=1;
	   return;
	}
    if (dy20==0 && dx20<=1)
	{
	   supa_count=1;
	   return;
	}
}


void	block_move()
{
	s16 x,y;
	s16 ok;
	#define m 2

  	block_x+=m*spd[block_dir].x;
  	block_y+=m*spd[block_dir].y;
	block_count-=m;

	if (block_index!=255)
	{
		supa_x[block_index] = block_x;
		supa_y[block_index] = block_y;
	}

	if (block_count<=0)
	{
		x=DIV16(block_x);
		y=DIV16(block_y);
		ok=read_map(x+spd[block_dir].x,y+spd[block_dir].y);
		// stop moving
		if (ok)
		{
			set_map(x,y,block_type);

			if (block_type>=BLOCK_SUPA && block_type<=BLOCK_SUPA+2)
			{
			    if (supa_count!=255)
			   		check_supas();
				if (supa_count==2)
					trigger_stars();
				block_index=255;
			}
	 		block_ctrl=BLOCK_REMOVE;
		   	block_frame=0;
			return;
		}
		else
		{
			block_count=YGAP;
			if (block_dir==1 || block_dir==3) block_count=XGAP;
		}
	}
	hit_block_vs_baddie();
}



void	block_crush()
{
	s16 x,y;

	block_trigger--;
	if (block_trigger<=0)
	{
		block_frame++;
		block_trigger=10;
		if (block_frame>=4)
		{
		    x=DIV16(block_x);
			y=DIV16(block_y);
			set_map(x,y,0);
			block_ctrl=BLOCK_DEAD;
			TSPRITE(FLOOR_TILE_D_TSP,block_x+xoff,block_y+yoff);
		}
	}
}

void	block_remove()
{
	block_index=255;
  	block_ctrl=BLOCK_DEAD;
}



void do_block()
{
	if (block_ctrl==BLOCK_CRUSH)
		block_crush();
    else if (block_ctrl==BLOCK_ALIVE)
		block_move();
    else if (block_ctrl==BLOCK_REMOVE)
		block_remove();
}


u8	TSPRITE2(s16 index,s16 x,s16 y)
{
	u8 xdiv = (u8) ((x>>1)&3);
	u8 ydiv = (u8) ((y>>1)&3);
	u8 fr = (u8) index;
	u8 basex = 0;//BLOCK_X00_TSP;
    u8 basey = 0;//BLOCK_Y00_TSP;


	if (block_type>=BLOCK_SUPA && block_type<=BLOCK_SUPAC)
	{
    	//basex = BLOCK_SB_X00_TSP;
    	//basey = BLOCK_SB_Y00_TSP;
	}

	if (xdiv)
	{
		TSPRITE(basex+xdiv,x,y);
		fr=basex+xdiv;
	}
	else if (ydiv)
	{
		TSPRITE(basey+ydiv,x,y);
	 	fr=basey+ydiv;
	}
	else
	{
   		TSPRITE(index,x,y);
	}
	return(fr);
}

void	draw_block()
{
	s16 ex;

	if (block_ctrl==0) return;

    ex=0;

    TSPRITE_BLANK( block_oldsprite, block_oldx+xoff, block_oldy+yoff );
    block_oldsprite=TSPRITE2( block_sprite+ex+block_frame, block_x+xoff, block_y+yoff );
	block_oldx=block_x;
	block_oldy=block_y;
}


void	draw_snoblock()
{
	if (snoblock_ctrl==0) return;

	if (snoblock_ctrl==2)
	{
		snoblock_ctrl=0;
	   	TSPRITE_BLANK( BLOCK_DIE_00_TSP, snoblock_x+xoff, snoblock_y+yoff );
	}
	else
	{
    	TSPRITE( BLOCK_DIE_00_TSP + snoblock_frame, snoblock_x+xoff, snoblock_y+yoff );
	}
}


void	snoblock_crush()
{
	s16 x,y;

	if (snoblock_ctrl==0) return;

	snoblock_trigger--;
	if (snoblock_trigger<=0)
	{
		snoblock_frame++;
		snoblock_trigger = 10;
		if (snoblock_frame >= 4)
		{
		    x=DIV16(snoblock_x);
			y=DIV16(snoblock_y);
			set_map(x,y,BLOCK_FLOOR);
			snoblock_ctrl=2;
		}
	}
}


void	draw_supa(u8 index)
{
	s16 ex;
	u8 k;
	u8 x,y,mx,my;
	u8 ok;

	if (supa_trigger[index]>0)
	{
		k = supa_move[index];
		x = supa_x[index];
		y = supa_y[index];
		mx = DIV16(x);
		my = DIV16(y);
		// start check if its ok
		if (supa_trigger[index]==2)
		{
			ok=read_map(mx+spd[k].x,my+spd[k].y);
			if (ok!=BLOCK_FLOOR && ok!=BLOCK_HOLE)
			{
				supa_trigger[index]=0;
				return;
			}
		}
	  	if (x&15)
	  		TSPRITE( FLOOR_TILE_D_X_TSP, x+xoff, y+yoff );
	  	else if (y&15)
	  		TSPRITE( FLOOR_TILE_D_Y_TSP, x+xoff, y+yoff );
		else
	   		TSPRITE( FLOOR_TILE_D_TSP, 	 x+xoff, y+yoff );

	  	x+=8*spd[k].x;
		y+=8*spd[k].y;
		supa_x[index]=x;
		supa_y[index]=y;
	    mx=DIV16(x);
		my=DIV16(y);
		supa_trigger[index]--;
		if (supa_trigger[index]==0)
		{
			// made it to the target
			if (read_map(mx,my)==BLOCK_HOLE)
			{
				sound(SFX_STUN,SD_NORMAL);
				// expansion
    			//set_map(mx,my,BLOCK_SUPAFIXED);
				set_map(mx,my,BLOCK_FLOOR);
				supa_trigger[index]=-1;
				supa_done--;
			}
			else
			{
    			set_map(mx,my,BLOCK_SUPA|(index<<4));
			}
			// cleat the original position
			mx-=spd[k].x;
			my-=spd[k].y;
			set_map(mx,my,0);
		}
	}


	// normal flicker
   	ex=((g_frame>>4)&1);
   	if (supa_trigger[index]<0)
   		ex=((g_frame>>2)&1);
	TSPRITE( BLOCK_SUPA_GREEN_TSP+ex, supa_x[index]+xoff, supa_y[index]+yoff );
}


void	start_block(s16 type,s16 x,s16 y)
{
	block_ctrl = BLOCK_ALIVE;
	block_x = (u8) MUL16(x);
	block_y = (u8) MUL16(y);
	block_oldx = (u8) MUL16(x);
	block_oldy = (u8) MUL16(y);
	block_oldsprite = BLOCK_SUPA_GREEN_TSP;
	block_type = (u8) type;
	block_index = 255;
	block_frame=0;
}


void player_wait_for_block()
{

}


void	do_player2()
{
	s16 k=-1;
	s16 x=DIV16(player_x+8);
	s16 y=DIV16(player_y+8);
	s16 ok=0xff;
//	s16 next;
	u8 index;
	u8 i;
	u8 frame;
	u8 nx,ny;

	// safety
	player_x = MUL16(x);
	player_y = MUL16(y);

	// stop player moving if _kicking
   if (g_wallgtimer)
	{
		g_wallgtimer--;
		if (g_wallgtimer<=0)
		{
			g_hitwall=0;
		}
	}

    if (player_kicked)
	{
		if (pad(JOY_B))
		{
	   		k = player_dir;
			for (i=0;i<MAPW;i++)
			{
				x+=spd[k].x;
				y+=spd[k].y;
	    		ok = read_map(x,y);
			    if (ok==BLOCK_FLOOR||ok==BLOCK_HOLE)
				{
					frame=(g_frame>>2)&1;
					if ((g_frame>>1)&1)
					{
					 	if (ok==BLOCK_FLOOR)
				    		TSPRITE(FLOOR_TILE_D_TSP,MUL16(x)+xoff,MUL16(y)+yoff);
						else
					    	TSPRITE(FLOOR_TILE_F_TSP,MUL16(x)+xoff,MUL16(y)+yoff);
					}
					else
					{
					   //	frame=ran(0,7);
				    	TSPRITE(MAGNET_DOWN_00_TSP+ran(0,7),MUL16(x)+xoff,MUL16(y)+yoff);
					}
				}
				else
				{
					i=MAPW;
				}
			}
	 		return;
		}
		// B just released so clear the screen
		else
		{
			 sound(SFX_BUZZOFF,SD_NORMAL);


			k = player_dir;

			for (i=0;i<MAPW;i++)
			{
				x+=spd[k].x;
				y+=spd[k].y;
	    		ok=read_map(x,y);
			    if (ok==BLOCK_FLOOR)
				{
				   	TSPRITE(FLOOR_TILE_D_TSP,MUL16(x)+xoff,MUL16(y)+yoff);
				}
				else if (ok==BLOCK_HOLE)
				{
				   	TSPRITE(FLOOR_TILE_F_TSP,MUL16(x)+xoff,MUL16(y)+yoff);
				}
				else
				{
					i=MAPW;
				}
			}
			x=DIV16(player_x+8);
			y=DIV16(player_y+8);
			k=-1;
		}
	}
    player_kicked=0;

	if (pad(JOY_UP)) 	k=0;
    if (pad(JOY_RIGHT)) k=1;
	if (pad(JOY_DOWN)) 	k=2;
    if (pad(JOY_LEFT)) 	k=3;
	
	if (k!=-1)
	{
		player_dir=(u8) k;
		ok = read_map(x+spd[k].x,y+spd[k].y);
	}
	// ok to move
	if (ok==BLOCK_FLOOR || ok==BLOCK_HOLE)
	{
		player_ctrl=PLAYER_MOVE;
		player_gtimer=YGAP;
		if (k==1 || k==3) player_gtimer=XGAP;
		player_walk();
		return;
	}
	if (ok==BLOCK_OXYGEN)
	{
		sound(SFX_PICKUP,SD_NORMAL);
		//g_oxygen+=4;
		//expansion
		g_oxygen+=6;
		nx=x+spd[k].x;
		ny=y+spd[k].y;
		TSPRITE(FLOOR_TILE_D_TSP,MUL16(nx)+xoff,MUL16(ny)+yoff);
	    set_map(nx,ny,0);
	}

	  // do fire
	if (xpad(JOY_A))
	{
		fire_bullet();
		return;
	}

	// do magnet
	if (xpad(JOY_B))
	{
		sound(SFX_BUZZ,SD_NORMAL);

		k = player_dir;

		for (i=0;i<MAPW;i++)
		{
			x+=spd[k].x;
			y+=spd[k].y;
    		ok=read_map(x,y);
		   	if (ok==BLOCK_SUPA)
			{
				if (i!=0)
				{
					index = read_map_index(x,y);
					if (supa_trigger[index]==0)
					{
				   		supa_trigger[index]=2;
			    		supa_move[index]=(k+2)&3;
					}
				}
				i=MAPW;
			}
			else  if (ok==BLOCK_FLOOR || ok==BLOCK_HOLE)
			{
			}
			else
			{
				i=MAPW;
			}
		}
		player_kicked = KICK_DELAY;
	}
	
	// expansion: do about-face
	if (pad(JOY_RS)) {
		 if (!inputEdge) {
			inputEdge = true;
			player_dir+=2;
			if (player_dir>3) player_dir-=4;
		 }
	}
	else {
		inputEdge = false;
	}
}


s8 next_player(s8 i)
{
	// select next player
	if (number_of_players==2)
				i=i^1;

	if (lives[i]<=0)
	{
	  	if (number_of_players==2)
			i=i^1;
		if (lives[i]<=0)
		{
			return(-1);
		}
	}
	return(i);
}

void player_die()
{
	// spin player
	player_trigger--;
	if (player_trigger<=0)
	{
		player_trigger=10;
		player_dir++;
		player_dir&=3;
	}
	player_gtimer--;
	if (player_gtimer<=0)
    {
 		lives[current_player]--;
		if (lives[current_player]<=0)
	  	{
		  	lives[current_player]=0;
		  	state=STATE_GAMEOVER;
			gtimer=2*SECOND;
		}
		else
		{
#ifdef TWOPLAYER
		   	current_player=next_player(current_player);
		   	if (current_player==-1)
			{
				state = STATE_HISCORE;
				gtimer = 2*SECOND;
			}
			else
			{
		   		pend_state=STATE_PREGAME;
				state = STATE_FADER_START;
				gtimer=SECOND;
		   	}
#else
			pend_state=STATE_RESTART;
			state = STATE_FADER_START;
			gtimer=SECOND;
#endif
		}
    }
}


void	do_player()
{
   	if (player_ctrl==PLAYER_DIE)
			player_die();
	else if (player_ctrl==PLAYER_ALIVE)
			do_player2();
    else if (player_ctrl==PLAYER_MOVE)
			player_walk();

	if (player_ctrl!=PLAYER_DIE && player_ctrl!=PLAYER_DEAD)
   		hit_player_vs_baddie();
}


void SelectTileset(u8 setindex)
{
	switch(setindex)
	{
		case TILESET_TITLE:
	   		font_base = 0x0;
  	        ds_setts( (char *)ts_data, sizeof(ts_data),(short *)smap );
			break;
	   case TILESET_GAME:
	   default:
		    ds_setts((char *)ts_data,sizeof(ts_data),(short *)smap);
		   	font_base=0;
			break;
	}
}


void	draw_title()
{
  	ClearMap(0);
    SelectTileset(TILESET_TITLE);
	TSPRITE_DIRECT((u8 *)a0_logo_smap,4*(XREZ/8-BPTR(a0_logo_smap)) ,2*8);
}

void	draw_oxygen_bar()
{
	u8 x;
	u8 ex;


   	for(x=0;x<22;x+=1)
	{
		ex=1;
	 	if (x<g_oxygen)
			ex=0;

	   	TSPRITE(BAR_GREEN_TSP+ex,3*8+MUL8(x+TOPX), MUL8(TOPY+1+2*MAPH+1) );
	}
}




u8 get_map(s16 x,s16 y)
{
	u8 a;
	u16 offset =  level[current_player];

	offset*=8*13;

	if (x<0) return(MAP_STARS);
	if (x>=15) return(MAP_STARS);
    if (y<0) return(MAP_STARS);
	if (y>=13) return(MAP_STARS);

	a = BPTR(&levels[(x/2)+(8*y)+offset]);
	if ((x&1)) a>>=4;
	a&=15;
	return(a);
}

u8 get_mapf(s16 x,s16 y)
{
	u8 a;
	a=get_map(x,y);
	if (a==MAP_OXYGEN)  a=MAP_FLOOR;
	if (a==MAP_TARGET)  a=MAP_FLOOR;
	if (a==MAP_ORB) 	a=MAP_FLOOR;
	if (a==MAP_WALL) 	a=MAP_FLOOR;
	return(a);
}



void	setup_collision()
{
	s16 x,y;
	s16 nx,ny;
	u8 a;

	memset(game_grid,BLOCK_FLOOR,sizeof(game_grid));
   // 	map
	for(y=1;y<MAPH+2;y++)
	for(x=1;x<MAPW+2;x++)
	{
		nx = x-1;
		ny = y-1;

		a = get_map(x,y);

		switch( a )
		{
			case MAP_FLOOR:
				break;
			case MAP_ENERGY:
	    		rmap(nx,ny)=BLOCK_WALL;
				break;
	    	case MAP_STARS:
				rmap(nx,ny)=BLOCK_INVISIBLE;
				break;
	    	case MAP_OXYGEN:
	    		rmap(nx,ny)=BLOCK_OXYGEN;
				break;
	    	case MAP_TARGET:
	    		rmap(nx,ny)=BLOCK_HOLE;
				break;
	  		case MAP_WALL:
				rmap(nx,ny)=BLOCK_ICE;
				break;
			case MAP_ORB:
				rmap(nx,ny)=BLOCK_SUPA|(supa_count<<4);
				supa_x[supa_count]=MUL16(nx);
				supa_y[supa_count]=MUL16(ny);
				supa_move[supa_count]=0;
				supa_count++;
				break;
		}
	}
}

void	draw_background()
{
	s16 xt,yt;
	s16 x,y;
  //	u8 ex;
//	u8 exa,exb;
	u8 a;
	u8 lf,rg,up,dn;
	u8 uplf,uprg;
    u8 dnlf,dnrg;

	SelectTileset(TILESET_GAME);
	dirty = 0xff;

   // 	map
	for(y=0;y<MAPH+2;y++)
	for(x=0;x<MAPW+2;x++)
	{
		xt=TOPX+(x<<1)-1;
		yt=TOPY+(y<<1)-1;
		xt<<=3;
		yt<<=3;

		a = get_map(x,y);
		lf = get_mapf(x-1,y);
		rg = get_mapf(x+1,y);
		up = get_mapf(x,y-1);
		dn = get_mapf(x,y+1);

		uplf = get_mapf(x-1,y-1);
		uprg = get_mapf(x+1,y-1);
		dnlf = get_mapf(x-1,y+1);
		dnrg = get_mapf(x+1,y+1);


		if (a==MAP_FLOOR)
	   		TSPRITE(FLOOR_TILE_D_TSP,xt,yt);

		if (a==MAP_ENERGY)
		{
			// c1
			if (lf == MAP_FLOOR && up==MAP_FLOOR)
					TSPRITE(BORDER_C1_TSP,xt,yt);
			else if (lf == MAP_FLOOR)
					TSPRITE(BORDER_SIDE_TSP,xt,yt);
		    else if (up == MAP_FLOOR)
					TSPRITE(BORDER_TOP_TSP,xt,yt);
			else  if (uplf == MAP_FLOOR)
	    			TSPRITE(BORDER_C4_TSP,xt,yt);
			else
		    		TSPRITE(STAR_00_TSP+ran(0,7),xt,yt);

		    // c2
			if (rg == MAP_FLOOR && up==MAP_FLOOR)
					TSPRITE(BORDER_C2_TSP,xt+8,yt);
			else if (rg == MAP_FLOOR)
					TSPRITE(BORDER_SIDE_TSP,xt+8,yt);
		    else if (up == MAP_FLOOR)
					TSPRITE(BORDER_TOP_TSP,xt+8,yt);
			else  if (uprg == MAP_FLOOR)
	    			TSPRITE(BORDER_C3_TSP,xt+8,yt);
			else
		    		TSPRITE(STAR_00_TSP+ran(0,7),xt+8,yt);

		    // c3
			if (lf == MAP_FLOOR && dn==MAP_FLOOR)
					TSPRITE(BORDER_C3_TSP,xt,yt+8);
			else if (lf == MAP_FLOOR)
					TSPRITE(BORDER_SIDE_TSP,xt,yt+8);
		    else if (dn == MAP_FLOOR)
					TSPRITE(BORDER_TOP_TSP,xt,yt+8);
			else  if (dnlf == MAP_FLOOR)
	    			TSPRITE(BORDER_C2_TSP,xt,yt+8);
			else
		    		TSPRITE(STAR_00_TSP+ran(0,7),xt,yt+8);

		    // c4
			if (rg == MAP_FLOOR && dn==MAP_FLOOR)
					TSPRITE(BORDER_C4_TSP,xt+8,yt+8);
			else if (rg == MAP_FLOOR)
					TSPRITE(BORDER_SIDE_TSP,xt+8,yt+8);
		    else if (dn == MAP_FLOOR)
					TSPRITE(BORDER_TOP_TSP,xt+8,yt+8);
			else  if (dnrg == MAP_FLOOR)
	    			TSPRITE(BORDER_C1_TSP,xt+8,yt+8);
			else
		    		TSPRITE(STAR_00_TSP+ran(0,7),xt+8,yt+8);

		}

		// stars
	    if (a==MAP_STARS)
		{
	   		TSPRITE(STAR_00_TSP+ran(0,7),xt,yt);
			TSPRITE(STAR_00_TSP+ran(0,7),xt+8,yt);
			TSPRITE(STAR_00_TSP+ran(0,7),xt+8,yt+8);
			TSPRITE(STAR_00_TSP+ran(0,7),xt,yt+8);
		}
	    if (a==MAP_OXYGEN)
	  	 	TSPRITE(OXYGEN_TSP,xt,yt);
	    if (a==MAP_TARGET)
	   		TSPRITE(FLOOR_TILE_F_TSP,xt,yt);
	  	if (a==MAP_WALL)
	   		TSPRITE(BLOCK_G_TSP,xt,yt);
		if (a==MAP_ORB)
		  	TSPRITE(BLOCK_SUPA_GREEN_TSP,xt,yt);

	}
}



void	draw_background_debug()
{
#ifdef PLATFORM_WIN32
	s16 xt,yt;
	s16 x,y;
	char temp[256];

    font_select(FONT_TINY);



   // map
	for(y=0;y<MAPH+2;y++)
	for(x=0;x<MAPW+2;x++)
	{
		xt=1+TOPX+(x<<1);
		yt=1+TOPY+(y<<1);
		xt<<=3;
		yt<<=3;
		if (rmap(x,y))
		{
   			sprintf(temp,"%02X",rmap(x,y));
			font_colour(0,0,0);
			go(xt,yt);
			printx(temp);
		    font_colour(255,255,255);
			go(xt+1,yt+1);
			printx(temp);
		}
	}
#endif
}



void	baddie_stun(s16 idx)
{
	baddie_frame[idx]++;
	baddie_trigger[idx]--;
	if (baddie_trigger[idx]<=0)
 	{
		baddie_ctrl[idx]=baddie_callb[idx];
	}
}


void force_stun(u8 idx)
{
		if (baddie_ctrl[idx]==0) return;

 		baddie_trigger[idx]=TIME_SUPA_BADDIE_STUN;
		baddie_sprite[idx]=BADDIE_STUN_A_SP;
		baddie_callb[idx]=baddie_ctrl[idx];
		baddie_ctrl[idx]=BADDIE_STUN;
		baddie_stun(idx);
}

int	check_stun(s16 idx)
{
	s16 x,y;
	u8 stun=0;


	x=baddie_x[idx];
	y=baddie_y[idx];

    if ((g_hitwall&WALL_TOP) && y==0) stun=1;
	if ((g_hitwall&WALL_BOT) && y==(MAPH-1)*YGAP) stun=1;
	if ((g_hitwall&WALL_LEFT) && x==0) stun=1;
	if ((g_hitwall&WALL_RIGHT) && x==(MAPW-1)*XGAP) stun=1;

  //	if (pad(JOY_A)==0) stun=0;

	if (stun)
	{
		baddie_trigger[idx] = TIME_BADDIE_STUN;
		baddie_sprite[idx] = BADDIE_STUN_A_SP;
		baddie_callb[idx] = baddie_ctrl[idx];
		baddie_ctrl[idx] = BADDIE_STUN;
		baddie_stun(idx);
		sound(SFX_STUN,SD_NORMAL);
		return(1);
	}
	return(0);
}

void	baddie_move(s16 idx)
{
	s16 x,y;

	x=baddie_x[idx];
	y=baddie_y[idx];

	if (check_stun(idx)) return;

  	baddie_frame[idx]++;
  	baddie_x[idx]+=1*spd[baddie_dir[idx]].x;
  	baddie_y[idx]+=1*spd[baddie_dir[idx]].y;
	baddie_gtimer[idx]--;
	if (baddie_gtimer[idx]<=0)
	{
		baddie_ctrl[idx] = BADDIE_ALIVE;
	}
}


void start_snoblock(s16 x,s16 y)
{
	if (snoblock_ctrl) return;

	snoblock_x= (u8) x;
	snoblock_y= (u8) y;
	snoblock_frame=0;
	snoblock_trigger=10;
	snoblock_ctrl=1;
	set_map(DIV16(x),DIV16(y),BLOCK_INVISIBLE);
}

void	do_baddie2(s16 idx)
{

	s16 ok;
	s16 x,y;
	s16 k;

	x=DIV16(baddie_x[idx]+8);
	y=DIV16(baddie_y[idx]+8);

	// force position
	baddie_x[idx]=MUL16(x);
	baddie_y[idx]=MUL16(y);


	if (check_stun(idx)) return;

	baddie_frame[idx]++;

    k=baddie_dir[idx];
	ok=read_map(x+spd[k].x,y+spd[k].y);
    // ok for snobee to move
	if (ok==BLOCK_FLOOR || ok==BLOCK_HOLE)
	{
		baddie_ctrl[idx]=BADDIE_MOVE;
		baddie_gtimer[idx]=YGAP;
		if (k==1 || k==3) baddie_gtimer[idx]=XGAP;
		baddie_move(idx);
		return;
	}
	/*
	if (ok==BLOCK_ICE && ran(0,100)>80 && snoblock_ctrl==0)
	{
    	baddie_ctrl[idx]=BADDIE_CRUSH;
		baddie_gtimer[idx]=60;
		start_snoblock(MUL16(x+spd[k].x),MUL16(y+spd[k].y));
		return;
	}
	*/
	// pick a random direction
	baddie_dir[idx]=(u8) ran(0,3);
}

void do_egg(s16 idx)
{
  //	s16 x,y;

   	baddie_trigger[idx]--;
   	if (baddie_trigger[idx]<=0)
   	{
		baddie_ctrl[idx] = BADDIE_ALIVE;
   	}
}

void baddie_crush(s16 idx)
{
	baddie_gtimer[idx]--;
	if (baddie_gtimer[idx]<=0)
	{
		baddie_ctrl[idx]=BADDIE_ALIVE;
	}
}

void baddie_pop(s16 idx)
{
    baddie_gtimer[idx]--;
	if ((baddie_gtimer[idx]&15)==0)
    	baddie_frame[idx]++;

	if (baddie_gtimer[idx]<=0)
	{
		baddie_ctrl[idx] = BADDIE_DEAD;
	}
}


void baddie_limbo(s16 idx)
{
	baddie_gtimer[idx]--;
	if (baddie_gtimer[idx]<=0)
	{
		baddie_ctrl[idx] = BADDIE_DEAD;
	}
}


s16		baddie_get_spawn_time()
{
	s16 x,y;
	x=10-level[current_player]-(g_frame>>6);
	y=30-level[current_player]-(g_frame>>6);
	if (x<0) x=0;
	if (y<1) y=1;
	return( ran(x,y)*SECOND+1 );
}


void	do_baddie(s16 idx)
{
    if (baddie_ctrl[idx]==BADDIE_LIMBO)
		baddie_limbo(idx);
    else if (baddie_ctrl[idx]==BADDIE_CRUSH)
		baddie_crush(idx);
	else if (baddie_ctrl[idx]==BADDIE_EGG)
		do_egg(idx);
	else if (baddie_ctrl[idx]==BADDIE_ALIVE)
   		do_baddie2(idx);
    else if (baddie_ctrl[idx]==BADDIE_MOVE)
   		baddie_move(idx);
    else if (baddie_ctrl[idx]==BADDIE_STUN)
   		baddie_stun(idx);
    else if (baddie_ctrl[idx]==BADDIE_POP)
   		baddie_pop(idx);
    else if (baddie_ctrl[idx]==BADDIE_DEAD)
   		respawn(idx);
}




void	start_baddie(s16 idx)
{
	baddies_active++;
	baddie_ctrl[idx] = BADDIE_EGG;
	baddie_dir[idx] = DIR_DOWN;
	baddie_trigger[idx] = baddie_get_spawn_time();	//ran(TIME_EGG_START_LO,TIME_EGG_START_HI);
}



#if 0
void draw_topwall(u8 ex)
{
	u8 x;
	u8 exa=BORDER_TOP_TSP;
	u8 exb=BORDER_TOP_TSP;
	u8 fr=(u8) ((g_frame>>2)&1);

	if (ex)
	{
		exa=BORDER_WOBBOT1_TSP;
		exb=BORDER_WOBBOT2_TSP;
		if (fr)
		{
	    	exa=BORDER_WOBBOT2_TSP;
	  		exb=BORDER_WOBBOT1_TSP;
		}
	}


    for(x=1;x<MAPW*2+1;x+=2)
	{
		 TSPRITE(exa,MUL8(x+TOPX), MUL8(TOPY) );
		 TSPRITE(exb,MUL8(1+x+TOPX), MUL8(TOPY) );
	}
}

void draw_botwall(u8 ex)
{
	u8 x;
	u8 exa=BORDER_TOP_TSP;
	u8 exb=BORDER_TOP_TSP;
	u8 fr=(u8) ((g_frame>>2)&1);

	if (ex)
	{
		exa=BORDER_WOBBOT1_TSP;
		exb=BORDER_WOBBOT2_TSP;
		if (fr)
		{
	    	exa=BORDER_WOBBOT2_TSP;
	  		exb=BORDER_WOBBOT1_TSP;
		}
	}

    for(x=1;x<MAPW*2+1;x+=2)
	{
		 TSPRITE(exa,MUL8(x+TOPX), MUL8(TOPY+2*MAPH+1) );
		 TSPRITE(exb,MUL8(1+x+TOPX), MUL8(TOPY+2*MAPH+1) );
	}
}

void draw_leftwall(u8 ex)
{
	u8 y;
	u8 exa=BORDER_SIDE_TSP;
	u8 exb=BORDER_SIDE_TSP;
	u8 fr=(u8) ((g_frame>>2)&1);



	if (ex)
	{
		exa=BORDER_WOBA1_TSP;
		exb=BORDER_WOBA2_TSP;
		if (fr)
		{
	    	exa=BORDER_WOBA2_TSP;
	  		exb=BORDER_WOBA1_TSP;
		}
	}
 	for(y=0;y<2*MAPH;y+=2)
	{
		 TSPRITE(exa, MUL8(0+TOPX),  MUL8(1+y+TOPY));
		 TSPRITE(exb, MUL8(0+TOPX),  MUL8(2+y+TOPY));
	}
}

void draw_rightwall(u8 ex)
{
	u8 y;
	u8 exa=BORDER_SIDE_TSP;
	u8 exb=BORDER_SIDE_TSP;
	u8 fr=(u8) ((g_frame>>2)&1);

	if (ex)
	{
		exa=BORDER_WOBA1_TSP;
		exb=BORDER_WOBA2_TSP;
		if (fr)
		{
	    	exa=BORDER_WOBA2_TSP;
	  		exb=BORDER_WOBA1_TSP;
		}
	}


 	for(y=0;y<2*MAPH;y+=2)
	{
		 TSPRITE(exa, MUL8(2*MAPW+1+TOPX), MUL8(1+y+TOPY));
		 TSPRITE(exb, MUL8(2*MAPW+1+TOPX), MUL8(2+y+TOPY));
	}
}
#endif


void update_panel()
{
//	u8 wall;

	ugo(8,PANELY*8);
	if ((g_frame>>4)&1 && current_player==0)
       uprintx(txt_blank3);
	else
   	   uprintx(txt_1up);
	uxtab+=8;
	if (dirty & DIRTY_P1)
	{
	 	print_score(score[0],5);
		dirty&=~DIRTY_P1;
	}

    ugo(11*8,PANELY*8);
	uprintx(txt_hi);

   	if (dirty & DIRTY_HISCORE)
	{
		uxtab+=8;
		print_score(hiscore,5);
	 	dirty&=~DIRTY_HISCORE;
	}

	ugo(20*8,PANELY*8);
	if ((g_frame>>4)&1 && current_player==1)
       	uprintx(txt_blank3);
	else
		uprintx(txt_2up);

	if (dirty & DIRTY_P2)
	{
   		uxtab+=8;
   		print_score(score[1],5);
		dirty&=~DIRTY_P2;
	}

	// act ??
    ugo(8,MUL8(BOTY));
	if (state==STATE_PREGAME &&  ((g_frame>>4)&1))
	{
 	   uprintx(txt_blank10);
	}
	else
 	{
   	 //	uprintx(txt_sector);
   		print_decimal(level[current_player]+1,2);
  		uprintc(32);
	}

	  // lives
    ugo(22*8+6*8,MUL8(BOTY));
   //	uprintx(txt_lives);
   //	uxtab+=8;
	print_decimal(lives[current_player],1);


  #if 0
	// draw walls
	wall = g_hitwall | g_oldwall;

	if (wall & WALL_TOP)
			draw_topwall( (u8) (g_hitwall&WALL_TOP) );

    if (wall & WALL_BOT)
			draw_botwall( (u8) (g_hitwall&WALL_BOT) );

    if (wall & WALL_LEFT)
			draw_leftwall( (u8) (g_hitwall&WALL_LEFT) );

	if (wall & WALL_RIGHT)
			draw_rightwall( (u8) (g_hitwall&WALL_RIGHT) );
 #endif

	draw_oxygen_bar();

 //	ugo(10*8,MUL8(BOTY));
 //   print_byte(g_oxygen);

	g_oldwall = g_hitwall;
}



void draw_main()
{
#ifdef TWOPLAYER
		if (xpad(JOY_UP))
		{
			sound(SFX_SHOOT,SD_NORMAL);
			number_of_players=1;
		  	g_frame=0;
      		ugo(XREZ/2,11*8);
		 	ucprintx_blank(txt_1player);
		}
		if (xpad(JOY_DOWN))
		{
			sound(SFX_SHOOT,SD_NORMAL);
			number_of_players=2;
			g_frame=0;
			ugo(XREZ/2,13*8);
		 	ucprintx_blank(txt_2player);
		}

	    ugo(XREZ/2,11*8);
		if (number_of_players==1)
	   	ucprintx_dot(txt_1player);
		else
	    ucprintx(txt_1player);


		ugo(XREZ/2,13*8);
		if (number_of_players==2)
		ucprintx_dot(txt_2player);
		else
	    ucprintx(txt_2player);

		// clear marker
		//ugo((XREZ/2)-6*8,15*8);
		ugo((XREZ/2)-6*8,11*8);
	   	uprintc(' ');
	    //ugo((XREZ/2)-6*8,17*8);
		ugo((XREZ/2)-6*8,13*8);
	   	uprintc(' ');

		if (((g_frame>>3)&1))
		{
			ugo((XREZ/2)-6*8,15*8+16*(number_of_players-1));
			//uprintc('>');
		}


#else
		    ugo(XREZ/2,3*YREZ/4);
			if (((g_frame>>5)&1))
		 		ucprintx(txt_pressfiretostart);
			else
		    	ucprintx(txt_blank10);
#endif

			//ugo(XREZ/2,YREZ-8);
			//ugo(XREZ/2,YREZ-40);
			ugo(XREZ/2,YREZ-72);
			ucprintx(txt_pjones);
			
			//expansion
			ugo(XREZ/2,YREZ-40);
			ucprintx(txt_forker);
			ugo(XREZ/2,YREZ-24);
			ucprintx(txt_RA);
}


void	draw_menu_stars()
{
	s16 i,x,y;
	u8 star = TSPRITE_GET_INDEX( STAR_00_TSP,0 );


	for(i=0;i<2;i++)
	{
  		x=ran(0,XREZ-1);
		y=ran(0,YREZ-1);
		x>>=3;
		y>>=3;
	   //	ugo(x,y);

	  	if (TileRead(x,y)==0x1B || ( TileRead(x,y)>=star && TileRead(x,y)<=star+7))
	    {
	   		if (ran(0,100)>50)
	   		{
			   SetTile(x,y,0x00);
	   		}
	   		else
	   		{
				SetTile(x,y,ran(0,7)+star);
			}
		}
	}

#ifdef CHEAT
	ugo(0,0);
	print_decimal(MAX_TILES,3);
#endif
}


void PanelClear()
{
	s16 x;
	for(x=0;x<XREZ;x+=8)
	{
		ugo(x,0);
		uprintc(32);
	    ugo(x,YREZ-8);
		uprintc(32);
	}
}

void draw_idle(OBJECT *p)
{
	p=p;
	s16 x,y,count;
	u8 k;


#ifdef PLATFORM_WIN32
	char temp[128];
	font_select(FONT_TINY);

   	sprintf(temp,"PY=(%d,%d) SP=%d TL=%d OX=%d",player_x,player_y,sizeof(sp_data)/64,sizeof(ts_data)/64,g_oxygen);
	colour(0,0,0);
	frect(0,0,strlen(temp)*4,6);
	go(0,0);
	font_colour(255,255,255);
	printx(temp);
#endif
   //	draw_oxygen_bar();

	if (state==STATE_FADER)
   	{
	 	draw_fader(0);
   	}

	switch(state)
	{
		case STATE_PAUSED:
			ugo(XREZ/2,0);
			ucprintx_dot(txt_paused);
			ugo(XREZ/2-5*8,YREZ-8);

						if (continue_exit!=1)
			{
		   		ucprintx_dot(txt_continue_exit1);
			   //	if (((g_frame>>3)&1)==0)
		     	//uprintc('?');
			   //	else
			   // uprintc(32);
			}
			else
			{
		   		ucprintx(txt_continue_exit1);
			 	uprintc(32);
			}

		    ugo(XREZ/2+5*8,YREZ-8);
			if (continue_exit!=0)
			{
		   		ucprintx_dot(txt_continue_exit2);
	 	   	//	if (((g_frame>>3)&1)==0)
		       //	uprintc('?');
			  //	else
			   // uprintc(32);
		    }
			else
			{
		    	ucprintx(txt_continue_exit2);
			    uprintc(32);
			}



		   //	ucprintx_dot(txt_continue_exit);

		   //	if (((g_frame>>3)&1)==0)
		   //	{
		   //		ugo(64-8+80*continue_exit,YREZ-8);
		   //		uprintc('*');
		   //	}
			break;

		case STATE_MAINMENU:
		   //	if (pend_gtimer==0)
		  		draw_main();
		  // 	else
		  // 		draw_hiscore();

			draw_menu_stars();

			break;
	   //
		case STATE_RESTART:
	    case STATE_PREGAME:
			 g_oldwall=0xff;
			 dirty=0xff;

#ifdef TWOPLAYER
    	    ugo(XREZ/2,2*8); //12*8
			if (current_player==1)
		  	  	ucprintx_dot(txt_playertwo);
		    else
				ucprintx_dot(txt_playerone);
#endif


			k=0;
			for(y=0;y<10-4;y++)
			{
				if (y<5) count=y+1; else count=2*5-y-1;
		   		for(x=0;x<count;x++)
				{
					if (k<level[current_player])
					{
		   				TSPRITE(FLOOR_TILE_C_TSP,x*16+120-8*count,48+y*16);
					}
					else if (k==level[current_player])
					{
				    	TSPRITE(FLOOR_TILE_B_TSP+((g_frame>>3)&1),x*16+120-8*count,48+y*16);
					}
					else
				    TSPRITE(FLOOR_TILE_D_TSP,x*16+120-8*count,48+y*16);
					k++;
				}
			}

		    ugo(XREZ/2,3*8);
			ucprintx_dot( txt_level );
		    ugo(XREZ/2-8,4*8);
			print_decimal(level[current_player]+1,2);
			break;
		//
		case STATE_INGAME:
			update_panel();
		 	draw_block();
			draw_snoblock();
			draw_supa(0);
			draw_supa(1);
			draw_supa(2);
			draw_supa(3);
		  	draw_baddie(0);
		  	draw_baddie(1);
		  	draw_baddie(2);
			draw_player();
			draw_background_debug();
			break;

		case STATE_POSTGAME:
		case STATE_DIAMONDS:
			update_panel();
		 	draw_block();
			draw_snoblock();
			draw_supa(0);
			draw_supa(1);
			draw_supa(2);
		  	draw_baddie(0);
		  	draw_baddie(1);
		  	draw_baddie(2);
			draw_player();

			ugo(XREZ/2,11*8);
			ucprintx(txt_level_completed);

			ugo(XREZ/2,13*8);
			ucprintx(txt_time_bonus);

			ugo((XREZ/2)-8*2,15*8);
			print_decimal(g_bonus,4);
			break;


	    case STATE_GAMEOVER:
#ifdef TWOPLAYER
	    //    ugo(XREZ/2,2*YREZ/4-8-8-16);
		//	ucprintx(WORD(txt_player[current_player]));
#endif
		    ugo(XREZ/2,YREZ/2);
			ucprintx(txt_game_over);
			break;
		case STATE_ENTERNAME:
			break;
		case STATE_HISCORE:
			draw_hiscore();
			break;
	}

#ifdef PLATFORM_GBA
	gba_setmap(2,smap);
#endif

#if PLATFORM_DS
   	ds_setts((char *)&ts_data[0],sizeof(ts_data),(short *)&smap[0]);
#endif
}


u8 	hit_alien(s16 x,s16 y)
{
	s16 dx,dy;
	u8 i;

	for(i=0;i<MAX_BADDIE;i++)
	{
		if (baddie_ctrl[i]==BADDIE_MOVE || baddie_ctrl[i]==BADDIE_ALIVE)
		{
			dx=baddie_x[i]-x;
			dy=baddie_y[i]-y;
			if (abs(dx)<8 && abs(dy)<8)
			{
				if (baddie_ctrl[i]!=BADDIE_POP)
				{
					sound(SFX_SNOBEE_DIE,SD_NORMAL);
			  		baddie_ctrl[i] = BADDIE_POP;
			   		baddie_gtimer[i] = SECOND;
					baddie_frame[i] = 0;
				}
				return(1);
			}
		}
	}
	return(0);
}

void move_bullets()
{
	u8 i,k;
	s8 ok;
	s16 x,y;
	u8 frame;
	u8 index;
	u8 mx,my;



   	for (i=0;i<4;i++)
	{
		if (bullet_life[i])
		{
		    mx = DIV16(bullet_x[i]);
			my = DIV16(bullet_y[i]);
		    ok = read_map(mx,my);
		    if (ok==BLOCK_FLOOR)
		        TSPRITE(FLOOR_TILE_D_TSP,MUL16(mx)+xoff,yoff+MUL16(my));
	        if (ok==BLOCK_HOLE)
		        TSPRITE(FLOOR_TILE_F_TSP,MUL16(mx)+xoff,yoff+MUL16(my));
		   // SetTileX( DIV8(xoff+bullet_x[i]), DIV8(yoff+bullet_y[i]), FLOOR_TILE_D_TSP  );
		  	bullet_life[i]--;
		}
	}

	for (i=0;i<4;i++)
	{
		if (bullet_life[i])
		{
			k = bullet_dir[i];
			bullet_x[i]+=spd[k].x<<2;
			bullet_y[i]+=spd[k].y<<2;
			x = bullet_x[i];
			y = bullet_y[i];
			ok = hit_alien(x,y);
			if (ok)
			{
		  		bullet_life[i] = 0;
				continue;
			}
			mx = DIV16(x);
			my = DIV16(y);
			ok = read_map(mx,my);
			if ( ok == BLOCK_SUPAFIXED || ok==BLOCK_INVISIBLE || ok==BLOCK_WALL)
			{
				bullet_life[i]=0;
			}
			else if ( ok == BLOCK_SUPA || ok == BLOCK_SUPAFIXED )
			{
				index = read_map_index(mx,my);
				if (supa_trigger[index]==0)
				{
			  		supa_move[index]=k;
					supa_trigger[index]=2;
				}
		  		bullet_life[i] = 0;
			}
		    else if ( ok == BLOCK_OXYGEN )
			{
			    TSPRITE(FLOOR_TILE_D_TSP,MUL16(mx)+xoff,yoff+MUL16(my));
	    		set_map(mx,my,0);
		    	bullet_life[i] = 0;
			}
			else if ( ok == BLOCK_ICE )
			{
				if (block_ctrl==0)
				{
			 	    sound( SFX_CRUSH, SD_NORMAL );
	  		   	 	set_map(mx,my,BLOCK_INVISIBLE);
	  		   		start_block(BLOCK_ICE, mx, my);
	  		  		block_ctrl = BLOCK_CRUSH;
					block_sprite = BLOCK_DIE_00_TSP;
		  			block_trigger = 10;
					block_frame = 0;
		   			block_crush();
					add_score(SCORE_BREAK_BLOCK);
				}
		  		bullet_life[i] = 0;
			}
			else if (bullet_x[i]<0 || bullet_x[i]>=XREZ-4*8 || bullet_y[i]<0 || bullet_y[i]>=YREZ-4*8)
			{
		  		bullet_life[i] = 0;
			}
			else
			{
				if (bullet_dir[i]&1)
				{
					frame = bullet_x[i]>>1;
					frame &= 7;
					frame = TSPRITE_TILE( BULLET_RIGHT_00_TSP + frame );
		    		SetTile( DIV8(xoff+bullet_x[i]), DIV8(yoff+bullet_y[i]), frame );
				}
				else
				{
			    	frame = bullet_y[i]>>1;
					frame &= 7;
					frame = TSPRITE_TILE( BULLET_DOWN_00_TSP + frame );
			    	SetTile( DIV8(xoff+bullet_x[i]), DIV8(yoff+bullet_y[i]), frame );
				}
			}
		}
	}
}


void	fire_bullet()
{
	if (bullet_life[bullet_slot]<=0)
	{
  		sound(SFX_SHOOT,SD_NORMAL);
   		bullet_life[bullet_slot] = 60;
   		bullet_x[bullet_slot] = player_x;
		bullet_y[bullet_slot] = player_y;
		bullet_dir[bullet_slot] = player_dir;
		bullet_oldx[bullet_slot] = player_x;
		bullet_oldy[bullet_slot] = player_y;
		bullet_slot++;
		bullet_slot&=3;
	}
}


void	do_idle(OBJECT *p)
{
	p=p;
	g_frame++;

	switch(state)
	{
		case STATE_FADER_START:
		   	sound(SFX_ZOOM,SD_NORMAL);
			start_fader_single();
			state = STATE_FADER;
			break;

		case STATE_FADER:
  			do_fader(0);
	   		if ( faders[0].ctrl == 2 )
			{
				g_frame=0;
			 	state = pend_state;
			}
	   		break;

		case STATE_PREMENU:
			state++;
			break;

		case STATE_MAINMENU:
			gtimer++;
			// any key goes to the main menu



		    if (pad(0xFFFF))
				gtimer=0;

			if (pad(0xFFFF) && pend_gtimer)
			{
				gtimer=0;
				pend_gtimer=0;
			}
			// swap hiscore/title
			if ((gtimer&0x200)==0x200)
			{
				pend_gtimer^=1;
			}






#ifdef CHEAT
  			if (xpad(JOY_START))
			{
				state = STATE_FADER_START;
			   	pend_state = STATE_PREGAME;
			  	gtimer = 20*SECOND;
			  	new_game();
			}

   			if (xpad(JOY_UP)) sound_index--;
			if (xpad(JOY_DOWN)) sound_index++;

			if (xpad(JOY_A))  TriggerFxChannel(2,12,255,1);
			if (xpad(JOY_B))  TriggerFxChannel(2,13,255,1);
			if (xpad(JOY_X))  TriggerFxChannel(2,14,255,1);
			if (xpad(JOY_Y))  TriggerFxChannel(2,15,255,1);

#else
	  	    if (xpad((JOY_START|JOY_A)))
			{
				state = STATE_FADER_START;
			   	pend_state = STATE_PREGAME;
			  	gtimer = 20*SECOND;
			  	new_game();
			}

#endif
			break;


		case STATE_PREGAME:
			gtimer--;
			if (gtimer==0 || xpad((JOY_A|JOY_START)))
			{
		  		state=STATE_INGAME;
		  		new_level();
		  		draw_background();
		  		gtimer=3*SECOND;
		  		g_bonus=2000;
			}
			break;

		 case STATE_RESTART:
			gtimer--;
			if (gtimer<=0)
			{
			   	state=STATE_INGAME;
				restart_level();
				draw_background();
			    play_music(MUSIC_INGAMEA);
			}
			break;

		case STATE_PAUSED:
				if (pad(JOY_LEFT))
				{
					sound(SFX_SHOOT,SD_NORMAL);
			 		g_frame=0;
					continue_exit=0;
				}
				if (pad(JOY_RIGHT))
			   	{
					sound(SFX_SHOOT,SD_NORMAL);
			 		g_frame=0;
					continue_exit=1;
			   	}

				// enable
				if (g_frame>=16) continue_exit&=1;


				if (xpad(JOY_A))
				{
				 	if (continue_exit==1)
					{
						g_frame=0;
					   	state=STATE_PREMENU;
						number_of_players=1;
						draw_title();
				        return;
					}
					else
					{
						sound(SFX_PAUSE,SD_NORMAL);
					 	ResumeSong();
				 		state=STATE_INGAME;
				 		PanelClear();
						dirty = 0xff;
				 		return;
					}
				}
			    if (xpad(JOY_START))
				{
				 	ResumeSong();
				 	state=STATE_INGAME;
				 	PanelClear();
					dirty = 0xff;
				 	return;
				}
			break;

		case STATE_INGAME:

			if ((g_frame&127)==0)
			{
				if (g_oxygen)
				{
					g_oxygen--;
			   		if (g_oxygen==0)
			   		{
					   player_start_die();
					}
				}
			}


			  // start pause
			    if (xpad(JOY_START))
				{
				  	StopSong();
				  	continue_exit=4;
				  	state=STATE_PAUSED;
				  	PanelClear();
				  	g_frame=0;
					sound(SFX_PAUSE,SD_NORMAL);
				  	return;
				}



			if (gtimer) gtimer--;
			if (gtimer==1)
			{
		    	play_music(MUSIC_INGAMEA);
			}

			move_bullets();

#ifdef CHEAT
		       if (xpad(JOY_SELECT))
			        g_oxygen=100;

			   if (xpad(JOY_RS))
				{
				    if (level[current_player]<19)
					level[current_player]++;
					state=STATE_PREGAME;
				 	gtimer=1;
					ClearMap(0);
				}
				if (xpad(JOY_LS))
				{
					if (level[current_player]!=0)
			   		level[current_player]--;
					state=STATE_PREGAME;
			        gtimer=1;
					ClearMap(0);
				}
 #endif




			if (gtimer==0)
			{
				if (g_bonus && (g_frame&255)==0) g_bonus-=100;
	 		  	do_player();
	  		 	do_block();
				snoblock_crush();
	   			do_baddie(0);
	 			do_baddie(1);
				do_baddie(2);
				if (supa_done<=0)
				{
					if (pend_gtimer==0)
					{
					  	pend_gtimer = 1*SECOND;
				   	}
				}
			}
			// exits level
			if (pend_gtimer)
			{
				pend_gtimer--;
				if (pend_gtimer<=0)
				{
				    gtimer=2*SECOND;
					state = STATE_POSTGAME;
					play_music(0);
				   	ClearMap(0);
				}
			}

		      if (xpad(JOY_SELECT))
			  {
			//	state = STATE_DIAMONDS;
			//	gtimer = 5*SECOND;
			  }
			break;

		case STATE_DIAMONDS:
			if ((gtimer&3)==3)
			{
				sound(SFX_BONUS_CLICK,SD_NORMAL);
				sound2(SFX_BONUS_CLICKB,SD_NORMAL);
				add_score(SCORE_BONUS_ADD);
			}
		    if (gtimer) gtimer--;
			if (gtimer<=0)
			{
				force_stun(0);
				force_stun(1);
				force_stun(2);
				state = STATE_INGAME;
				g_oldwall=0xff;
				supa_count=255;
			}
			break;

		case STATE_POSTGAME:
			if ((g_frame&3)==0)
			{
		  		if (g_bonus)
		   		{
					sound(SFX_BONUS_CLICK,SD_NORMAL);
					sound2(SFX_BONUS_CLICKB,SD_NORMAL);
		   		 	g_bonus-=100;
					add_score(SCORE_BONUS_ADD);
		   		}
			}
			if (gtimer) gtimer--;
			if (gtimer<=0 && g_bonus==0)
			{
			   	level[current_player]++;
			   	state = STATE_PREGAME;
			   	gtimer = SECOND*10;
				ClearSubMap(0);
			}
			break;

		case STATE_GAMEOVER:
			gtimer--;
			if (gtimer<=0)
			{
				scores = hiscore_load_from_eeprom( hiscore_mem );
				hiscore_check(score[current_player]);
			}
			break;

		case STATE_ENTERNAME:
			uze_hiscore_enter();
			break;

		// after hiscore if no more players goto the mainmenu else go back to the game
		case STATE_HISCORE:
			gtimer--;
			if (gtimer<=0)
			{
				current_player=next_player(current_player);
				if (current_player==-1)
				{
			  		state=STATE_MAINMENU;
					number_of_players=1;
					draw_title();
				}
				else
				{
			 	    ClearMap(0);
		   			state=STATE_PREGAME;
					gtimer=SECOND;
				   	SelectTileset(TILESET_GAME);
				}
			}
			break;
	}
}



void new_level()
{
	u8 i;

	supa_done=4;
	pend_gtimer=0;
	snoblock_ctrl=0;
	block_ctrl=0;

	srand(1000-level[current_player]);
	scores=0;
	//g_oxygen = 18;
	//expansion
	g_oxygen = 26;
	supa_count=0;

	for(i=0;i<4;i++)
		supa_trigger[i]=0;

	setup_collision();

	restart_level();
}


void restart_level()
{
	s16 xm,ym;
	u8 i;
	u8 x,y;

  	player_ctrl=1;
	block_ctrl=0;
	snoblock_ctrl=0;
	block_index=255;
	memset(baddie_ctrl,0,sizeof(baddie_ctrl));
	baddies_active=0;


	// place player
	xm=MAPW/2;
	ym=MAPH/2;
	set_map(xm,ym,0);
	player_x=MUL16(xm);
	player_y=MUL16(ym);


	 for(i=0;i<MAX_BADDIE;i++)
	 {
again1:
		x=ran(0,MAPW-1);
		y=ran(0,MAPH-1);
		if ((x==xm && y==ym) || rmap(x,y)!=0) goto again1;
		start_baddie(i);
		baddie_ctrl[i]=1;
		baddie_x[i]=MUL16(x);
		baddie_y[i]=MUL16(y);
		baddie_frame[i]=0;
	 }


	g_hitwall=0;
	g_oldwall=0;
	player_dir=2;
	player_frame=0;
	player_kicked=0;

   //	WaitVsync(4*60);	//(100);
}





void new_game()
{
	snoblock_ctrl=0;
	block_ctrl=0;
  	current_player=0;
#ifdef TWOPLAYER
    score[0]=0;
	score[1]=0;
	lives[0]=START_LIVES;
	lives[1]=START_LIVES;
	level[0]=0;
	level[1]=0;
#else
  	score[0]=0;
	lives[0]=START_LIVES;
	level[0]=0;
#endif
	new_level();
}



void	boot()
{
#ifndef UZE
	OBJECT *p;
	static s16 cold=1;

    SetRez( XWIN, YWIN );

	// sets up sprites
   	ds_init((u8 *)sp_data,sizeof(sp_data),pal332,512,(void *)&sp_info[0],SP_MAX);
	ds_setts((char *)ts_data,sizeof(ts_data),(short *)smap);


	if (cold)
	{
		cold=0;
		hiscore_init();
		vgm_init(0);
	}

	clear_objs();

	memset(smap,0,sizeof(smap));

	p = place_obj(MENU_ID,0,0,0);
   	p->draw = (SUB) draw_idle;
	p->ctrl = (SUB) do_idle;
	god=p;
#endif

#ifdef PLATFORM_WIN32
	// background right at the back
	p=start_bg(1);
	p->level=0;
#endif


	state=0;
	gtimer=0;
	draw_title();
	number_of_players=1;
	g_frame=0;
	scores=0;
	pend_gtimer=0;
	gtimer=0;
	music_vol=0xff;
   	//	hiscore=2000;
	scores=hiscore_load_from_eeprom(hiscore_mem);
	hiscore = scores[0].val;

	// 	hiscore test
	//	score[0]=0xFFFF;
 	//	state=STATE_GAMEOVER;
 	//	gtimer=1;
 	TILE_FADER		= TSPRITE_GET_INDEX(A1_FADER_TSP,0);
}


#endif



