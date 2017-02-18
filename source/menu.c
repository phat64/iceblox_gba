#include <gba.h>
#include "font.h"
#include "enum.h"
#include "input.h"
#include "direction.h"
#include "iceblox_data.h"


struct s_sprite
{
	int id;
	int x;
	int y;

	int spr;
};


struct s_anim
{
	int id;
	int x;
	int y;

	int animIdx;
	int animLen;

	u8 anim[32];
	int dx[32];
	int dy[32];
};

void Menu_LoadAnim(struct s_anim * anim);
void Menu_LoadSprite(struct s_sprite * sprite);

int Math_Clamp(int x, int min, int max)
{
	if (x < min)
	{
		x = min;
	}
	else if (x > max)
	{
		x = max;
	}
	return x;
}

int Math_Wrap(int x, int min, int max)
{
	int delta;

	delta = max - min;
	x -= min;
	x %= delta;
	x += min;

	return x;
}


void Menu_Init()
{
	SetMode(MODE_0 | BG0_ENABLE| OBJ_ENABLE| OBJ_1D_MAP);

	dmaCopy(iceblox_palette + 0, BG_PALETTE + 0, 256);
	
	REG_BG0CNT = TEXTBG_SIZE_256x256 | CHAR_BASE(0)|SCREEN_BASE(8)|BG_256_COLOR;
	REG_DISPCNT |= BG0_ENABLE;
	
	dmaCopy(((u8*)iceblox_data) + 0, TILE_BASE_ADR(0) + 0, 64 * 16 * 16);
	//dmaCopy(NULL, TILE_BASE_ADR(0) + 0, 1 *  16*16);

	Font_LoadLetters(1, TILE_BASE_ADR(0));
}


void Menu_ClearOAM()
{
	int i; 
	for (i = 0; i < 128; i++)
	{
		OAM[i].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = 0;
	}

}

void Menu_Clear()
{
	int i;
	int j;
	u16* map;

	map = MAP_BASE_ADR(8);
	dmaCopy(NULL, map + 0, sizeof(u16) * 32 * 32);
	for (j = 0; j < 16; j++)
	{
		for (i = 0; i < 16; i++)
		{
			int blockid;
			int x;
			int y;
			
			blockid = 4*EMPTYBLOCK;
			x = i * 2;
			y = j * 2;
						
			map[32*(y+0) + x + 0] = blockid + 0;
			map[32*(y+0) + x + 1] = blockid + 1;
			map[32*(y+1) + x + 0] = blockid + 2;
			map[32*(y+1) + x + 1] = blockid + 3;
		}
	}
	
	Menu_ClearOAM();
}

void Menu_LoadLogo()
{
	u16 * map;
	int i;
	int x;
	int y;
	int w;
	int blockId0;
	int blockId1;
	int blockId2;
	int blockId3;


#define SPR_LOGO_0		48
#define SPR_LOGO_8		(48 + 8)

#define SPR_LOGO_W		(8 * 16)
#define SPR_LOGO_H		(2 * 16)
#define SPR_LOGO_X		(240 - SPR_LOGO_W) / 2
#define SPR_LOGO_Y		(8)

	x = SPR_LOGO_X / 8;			// pixels	-> block8x8
	y = SPR_LOGO_Y / 8;			// pixels	-> block8x8
	blockId0 = SPR_LOGO_0 * 4 + 0;	// 16x16	-> 8x8 (x4)
	blockId1 = SPR_LOGO_0 * 4 + 2;	// 16x16	-> 8x8 (x4)
	blockId2 = SPR_LOGO_8 * 4 + 0;	// 16x16	-> 8x8 (x4)
	blockId3 = SPR_LOGO_8 * 4 + 2;	// 16x16	-> 8x8 (x4)


	w = SPR_LOGO_W / 8;

	map = MAP_BASE_ADR(8);
	for (i = 0; i < w; i+=2)
	{
		map[32 * (y + 0) + x + 0] = blockId0 + 0;
		map[32 * (y + 0) + x + 1] = blockId0 + 1;
		map[32 * (y + 1) + x + 0] = blockId1 + 0;
		map[32 * (y + 1) + x + 1] = blockId1 + 1;

		map[32 * (y + 2) + x + 0] = blockId2 + 0;
		map[32 * (y + 2) + x + 1] = blockId2 + 1;
		map[32 * (y + 3) + x + 0] = blockId3 + 0;
		map[32 * (y + 3) + x + 1] = blockId3 + 1;

		x += 2;
		blockId0 += 4;
		blockId1 += 4;
		blockId2 += 4;
		blockId3 += 4;

	}
}

void Menu_LoadText(int x, int y, char * str)
{
	char c;
	u16 * map;

	map = MAP_BASE_ADR(8);
	while(str && *str)
	{
		c = *str;
		if (c >= 'A' && c <='Z')
		{ 
			map[32 * y + x] = (c - 'A');
		}
		else if (c >= 'a' && c <='z')
		{ 
			map[32 * y + x] = (c - 'a');
		}
		else if (c >= '0' && c <='9')
		{ 
			map[32 * y + x] = 26 + (c - '0');
		}
		else if (c == '!') // penguin
		{
			map[32 * y + x] = 13 * 4 + 2;
		}
		else
		{
			map[32 * y + x] = 37 * 4;
		}
		x ++;
		str++;
	}
}


void Menu_Loop0()
{
	int counter;
	int frame_counter;
	Menu_Clear();
	Menu_LoadLogo();
	
#define TEXT_CENTER_X ((240/8) / 2)

	Menu_LoadText(TEXT_CENTER_X - 18/2 , 6, "ACTORS AND OBJECTS");
	Menu_LoadText(TEXT_CENTER_X, 8,	"PIXEL PETE");
	Menu_LoadText(TEXT_CENTER_X, 10, "EVIL FLAMES");
	Menu_LoadText(TEXT_CENTER_X, 12, "ICE CUBE");
	Menu_LoadText(TEXT_CENTER_X, 14, "SOLID ROCK");
	Menu_LoadText(TEXT_CENTER_X, 16, "FROZEN COIN");

	Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");


#define TEXT_01_PIXEL_PETE_X	8
#define ACTOR_01_PIXEL_PETE_Y	8

	struct s_anim pete =
	{
		0,

		(TEXT_CENTER_X - 2) * 8 - 10, 8 * 8 - 4,

		1,
		4,
		{ 2,1,2,3},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	struct s_anim flame0 =
	{
		1,

		(TEXT_CENTER_X - 3) * 8 + 2, 16 * 5 - 4,

		0,
		5,
		{ 32, 33, 34, 35, 36},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	struct s_anim flame1 =
	{
		2,

		(TEXT_CENTER_X - 3) * 8 -6, 16 * 5 - 4,

		1,
		5,
		{ 32, 33, 34, 35, 36},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};
	

	struct s_sprite icecube =
	{
		3,

		(TEXT_CENTER_X - 2) * 8 - 10, 12 * 8 - 4,
		ICEBLOCK_0
		
	};
	
	
	struct s_sprite solidrock =
	{
		4,

		(TEXT_CENTER_X - 2) * 8 - 10, 14 * 8 - 4,
		STONEBLOCK
		
	};

	struct s_sprite frozencoin =
	{
		5,

		(TEXT_CENTER_X - 2) * 8 - 10, 16 * 8 - 4,
		COINBLOCK_0
		
	};	

	counter = 0;
	frame_counter = 0;
	while (frame_counter < 3 * 128 && !Input_IsStartPressed())
	{
		if ((frame_counter & 127) < 100)
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");
		else
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "           ");


		if (frame_counter % 6 == 0)
		{
			Menu_LoadAnim(&pete);
			Menu_LoadAnim(&flame0);
			Menu_LoadAnim(&flame1);
			Menu_LoadSprite(&icecube);
			Menu_LoadSprite(&solidrock);
			Menu_LoadSprite(&frozencoin);
			counter ++;
		}

		//Actor_UpdateAll();
		//Actor_RenderAll(counter);

		VBlankIntrWait();
		frame_counter ++;
		

	}
}

void Menu_Loop2()
{
	int counter;
	int frame_counter;
	Menu_Clear();
	Menu_LoadLogo();
	
#define TEXT_CENTER_X ((240/8) / 2)

	Menu_LoadText(TEXT_CENTER_X - 7/2 , 6, "SCORING");
	Menu_LoadText(TEXT_CENTER_X, 8,	"5 POINTS");  // break ice
	Menu_LoadText(TEXT_CENTER_X, 10, "50 POINTS"); // putting out flame
	Menu_LoadText(TEXT_CENTER_X, 12, "100 POINTS"); // free coins
	Menu_LoadText(TEXT_CENTER_X, 14, "1000 POINTS"); // next level


	Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");

	struct s_anim pete =
	{
		0,

		(TEXT_CENTER_X - 2) * 8 - 10, 8 * 8 - 4,

		1,
		8,
		{ ICEBLOCK_0, ICEBLOCK_1, ICEBLOCK_2, ICEBLOCK_3, ICEBLOCK_4, ICEBLOCK_5, ICEBLOCK_6, ICEBLOCK_7},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	struct s_anim flame1 =
	{
		1,

		(TEXT_CENTER_X - 3) * 8 + 2, 16 * 5 - 4,

		0,
		5,
		{ 32, 33, 34, 35, 36},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	struct s_anim flame0 =
	{
		2,

		(TEXT_CENTER_X - 3) * 8 -6, 16 * 5 - 4,

		1,
		5,
		{ ICEBLOCK_0, ICEBLOCK_0, ICEBLOCK_0, ICEBLOCK_0, ICEBLOCK_0},	// spr
		{ -16, -12, -8, -4, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};
	
	struct s_anim coinblock =
	{
		3,

		(TEXT_CENTER_X - 2) * 8 - 10, 12 * 8 - 4,

		1,
		8,
		{ COINBLOCK_0, COINBLOCK_1, COINBLOCK_2, COINBLOCK_3, COINBLOCK_4, COINBLOCK_5, COINBLOCK_6, COINBLOCK_7},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};



	struct s_sprite win =
	{
		4,

		(TEXT_CENTER_X - 2) * 8 - 10, 14 * 8 - 4,
		39
		
	};
	
	counter = 0;
	frame_counter = 0;
	while (frame_counter < 3 * 128 && !Input_IsStartPressed())
	{
		if ((frame_counter & 127) < 100)
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");
		else
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "           ");


		if (frame_counter % 6 == 0)
		{
			Menu_LoadAnim(&pete);
			Menu_LoadAnim(&flame0);
			Menu_LoadAnim(&flame1);
			Menu_LoadAnim(&coinblock);			
			Menu_LoadSprite(&win);
			counter ++;
		}
		
#ifdef NEVER
		{
			int n = frame_counter / 60;
			char text[3];
			text[0] = '0' + (n / 10);
			text[1] = '0' + (n % 10);
			text[2] = '\0';
			Menu_LoadText(240/8 - 2, 0, text); 
		
			struct s_sprite test =
			{	
				5,

			(TEXT_CENTER_X - 2) * 8 - 10, 18 * 8 - 4,
				n
		
			};
			
			Menu_LoadText(0,0,"!!!");
			Menu_LoadSprite(&test);

		
		}
#endif		
		//Actor_UpdateAll();
		//Actor_RenderAll(counter);

		VBlankIntrWait();
		frame_counter ++;
		

	}
}

void Menu_LoadAnim(struct s_anim * anim)
{
	int idx = anim->animIdx = Math_Wrap(anim->animIdx + 1, 0, anim->animLen); 
	int spr = anim->anim[idx];
	int dx = anim->dx[idx];
	int dy = anim->dy[idx];
	int id = anim->id;
	int x = anim->x + dx;
	int y = anim->y + dy;

	dmaCopy(((void*)iceblox_data) + 16 * 16 * spr, OBJ_BASE_ADR + (5 + id) * 16 * 16, 16 * 16);

	// actor
	OAM[id].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
	OAM[id].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
	OAM[id].attr2 = 8 * (5 + id);
}

void Menu_LoadSprite(struct s_sprite * sprite)
{
	int id = sprite->id;
	int spr = sprite->spr;
	int x = sprite->x;
	int y = sprite->y;

	dmaCopy(((void*)iceblox_data) + 16 * 16 * spr, OBJ_BASE_ADR + (5 + id) * 16 * 16, 16 * 16);

	// actor
	OAM[id].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
	OAM[id].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
	OAM[id].attr2 = 8 * (5 + id);
}

void Menu_Loop1()
{
	int frame_counter;
	int i;
	int inc;

	for (i = 0; i < 128; i ++)
	{
		// actor
		OAM[i].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = 0;
	}


	Menu_Clear();
	Menu_LoadLogo();
	


	Menu_LoadText(TEXT_CENTER_X - 11/2 , 6, "HOW TO PLAY");
	Menu_LoadText(TEXT_CENTER_X, 8,			"MOVE WITH DPAD");
	Menu_LoadText(TEXT_CENTER_X, 10,		"PUSH A ICE CUBE");
	Menu_LoadText(TEXT_CENTER_X, 12,		"BREAK A CUBE");
	Menu_LoadText(TEXT_CENTER_X, 14,		"FREE A COIN");
	Menu_LoadText(TEXT_CENTER_X, 16,		"WATCH OUT");
	Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");
	//Menu_LoadSprite(50, 50, 32);

#define TEXT_01_PIXEL_PETE_X	8
#define ACTOR_01_PIXEL_PETE_Y	8


	struct s_anim walk =
	{
		0,

		(TEXT_CENTER_X - 3) * 8, 8 * 8 - 4,

		0,
		12,

		{ 1,2,3,	4,5,6,	7,8,9, 10,11,12},	// spr
		{ 0,0,0,	0,0,0,	0,0,0, 0,0,0},		// dx
		{ 0,0,0,	0,0,0,	0,0,0, 0,0,0},		// dy
	};

	struct s_anim push =
	{
		1,

		(TEXT_CENTER_X - 3) * 8  - 3 * 16, 10 * 8 - 4,

		0,
		12,

		{ 11, 12, 10,		11, 10, 10,		10, 10, 10,		10, 10, 10},	// spr
		{ -12, -8,-4,			0,0,0,			0,0,0,			0,0,0},		// dx
		{ 0,0,0,			0,0,0,			0,0,0,			0,0,0},		// dy
	};

	struct s_anim slide =
	{
		2,

		(TEXT_CENTER_X - 3) * 8  - 2 * 16, 10 * 8 - 4,
		//0,0,

		0,
		12,

		//		{ 0,0,0,			0,0,0,			0,0,0,			0,0,0},		// dy


		{ 16, 16, 16,		16, 16, 16,		16, 16, 16,		16, 16, 16},	// spr
		{ 0,0,0,			0,4,8,			16,20,24,			32, 32, 32},		// dx
		{ 0,0,0,			0,0,0,			0,0,0,			0,0,0},		// dy
	};

	struct s_anim breakcube =
	{
		3,

		(TEXT_CENTER_X - 3) * 8 - 16, 12 * 8 - 4,

		0,
		12,
		{ 16,16, 16,		17,18,19,	20,21,22,	23, 37, 37},	// spr
		{ 0,0,0,		0,0,0,		0,0,0,		0,0,0},		// dx
		{ 0,0,0,		0,0,0,		0,0,0,		0,0,0},		// dy
	};

	struct s_anim rock =
	{
		4,

		(TEXT_CENTER_X - 3) * 8, 12 * 8 - 4,

		0,
		1,
		{ 14},	// spr
		{ 0},		// dx
		{ 0},		// dy
	};

	struct s_anim prock =
	{
		5,

		(TEXT_CENTER_X - 3) * 8 - 2 *16, 12 * 8 - 4,

		0,
		1,
		{ 11},	// spr
		{ 0},		// dx
		{ 0},		// dy
	};

	struct s_anim breakcoincube =
	{
		6,

		(TEXT_CENTER_X - 3) * 8 - 16, 14 * 8 - 4,

		0,
		12,
		{ 24, 25, 26,	27,28,29,	30,31,15,	15, 15, 15},	// spr
		{ 0,0,0,		0,0,0,		0,0,0,		0,0,0},		// dx
		{ 0,0,0,		0,0,0,		0,0,0,		0,0,0},		// dy
	};

	struct s_anim pcubecoin =
	{
		8,

		//(TEXT_CENTER_X - 3) * 8 - 2 *16, 14 * 8 - 4,
		(TEXT_CENTER_X - 3) * 8, 14 * 8 - 4,


		0,
		1,
		{ 7},	// spr
		{ 0},		// dx
		{ 0},		// dy
	};


	struct s_anim pflames =
	{
		9,

		(TEXT_CENTER_X - 3) * 8 - 2 *16-5, 16 * 8 - 4,

		0,
		3,
		{ 7,8,9},	// spr
		{ 0,0, 0},		// dx
		{  0,0, 0},		// dy
	};

	struct s_anim flame0 =
	{
		10,

		(TEXT_CENTER_X - 3) * 8 - 2, 16 * 8 - 4,

		0,
		5,
		{ 32, 33, 34, 35, 36},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	struct s_anim flame1 =
	{
		11,

		(TEXT_CENTER_X - 3) * 8 - 10, 16 * 8 - 4,

		1,
		5,
		{ 32, 33, 34, 35, 36},	// spr
		{ 0, 0, 0, 0, 0 },		// dx
		{ 0, 0, 0, 0, 0 },		// dy
	};

	
	inc = 0;
	frame_counter = 0;
	
	while (frame_counter < 3 * 128 && !Input_IsStartPressed())
	{
		if ((frame_counter & 127) < 100)
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "PRESS START");
		else
			Menu_LoadText(TEXT_CENTER_X - 11/2, 18, "           ");

		if (frame_counter % 6 == 0)
		{
			Menu_LoadAnim(&walk);
			
			Menu_LoadAnim(&push);
			Menu_LoadAnim(&slide);
			
			Menu_LoadAnim(&prock);
			Menu_LoadAnim(&rock);
			Menu_LoadAnim(&breakcube);

			Menu_LoadAnim(&breakcoincube);
			Menu_LoadAnim(&pcubecoin);

			Menu_LoadAnim(&flame0);
			Menu_LoadAnim(&flame1);
			Menu_LoadAnim(&pflames);
		}

		if (Input_GetDirection() == LEFT)
		{
			inc--;
		}
		else if (Input_GetDirection() == RIGHT)
		{
			inc ++;
		}
		///Actor_UpdateAll(frame_counter);
		///Actor_RenderAll(frame_counter);

		VBlankIntrWait();
		frame_counter ++;
	}
}


void Menu_Loop()
{
	while (!Input_IsStartPressed())
	{
		Menu_Loop0();
		Menu_Loop1();
		Menu_Loop2();
	}
}
