
#include <gba.h>
#include <stdio.h>
#include <stdlib.h>

#include "iceblox_data.h"
#include "enum.h"
#include "anim.h"
#include "map.h"
#include "input.h"
#include "direction.h"
#include "actor.h"
#include "menu.h"



void Intro_Show()
{
	int i;

	SetMode(MODE_0 | BG0_ENABLE| OBJ_ENABLE| OBJ_1D_MAP| WIN0_ENABLE);
	
#define WIN_RIGHT(n)    (n)
#define WIN_LEFT(n)     (n) << 8
#define WIN_BOTTOM(n)   (n)
#define WIN_TOP(n)      (n) << 8

#define WIN0_BG0      1
#define WIN0_BG1      2
#define WIN0_BG2      4
#define WIN0_BG3      8
#define WIN0_SPRITES  16
#define WIN0_BLENDS   32

	// 1rst window
	REG_WININ = WIN0_SPRITES | WIN0_BG0 | WIN0_BLENDS;
	REG_WINOUT = 0;
	
	// Draw Intro
	for (i = 0; i < 240; i++)
	{
		int x0;
		int y0;
		int x1;
		int y1;
	
		x0 = 240/2 - i/2;
		x1 = 240/2 + i/2;
		y0 = 160/2 - (i*2/3)/2;
		y1 = 160/2 + (i*2/3)/2;

		if (x0 < 0) x0 = 0;
		if (x1 >= 240) x1 = 239;
		
		if (y0 < 0) y0 = 0;
		if (y1 >= 160) y1 = 159;

		REG_WIN0H = WIN_LEFT(x0) | WIN_RIGHT(x1);
		REG_WIN0V = WIN_TOP(y0)  | WIN_BOTTOM(y1);
		
		//Actor_Create
		Actor_RenderAll(0);

		VBlankIntrWait();
	}
}


void Intro_Hide()
{
	int i;

	SetMode(MODE_0 | BG0_ENABLE| OBJ_ENABLE| OBJ_1D_MAP| WIN0_ENABLE);
	
#define WIN_RIGHT(n)    (n)
#define WIN_LEFT(n)     (n) << 8
#define WIN_BOTTOM(n)   (n)
#define WIN_TOP(n)      (n) << 8

#define WIN0_BG0      1
#define WIN0_BG1      2
#define WIN0_BG2      4
#define WIN0_BG3      8
#define WIN0_SPRITES  16
#define WIN0_BLENDS   32

	// 1rst window
	REG_WININ = WIN0_SPRITES | WIN0_BG0 | WIN0_BLENDS;
	REG_WINOUT = 0;
	
	// Draw Intro
	for (i = 240; i >=0 ; i--)
	{
		int x0;
		int y0;
		int x1;
		int y1;
	
		x0 = 240/2 - i/2;
		x1 = 240/2 + i/2;
		y0 = 160/2 - (i*2/3)/2;
		y1 = 160/2 + (i*2/3)/2;

		if (x0 < 0) x0 = 0;
		if (x1 >= 240) x1 = 239;
		
		if (y0 < 0) y0 = 0;
		if (y1 >= 160) y1 = 159;

		REG_WIN0H = WIN_LEFT(x0) | WIN_RIGHT(x1);
		REG_WIN0V = WIN_TOP(y0)  | WIN_BOTTOM(y1);
		
		//Actor_Create
		Actor_RenderAll(0);
		//Actor_UpdateAll(0);

		VBlankIntrWait();
	}
}


void Intro_Wait(int frame_counter, int n)
{
	int counter = 0;
					
	// wait n/60 seconds
	while(--n > 0)
	{
		if (frame_counter++ % 4 == 0)
		{
			Actor_UpdateAll();
			Actor_RenderAll(counter);
			counter ++;
		}
		VBlankIntrWait();
	}						
}


void FootPrint_Init(int x, int y)
{
	int i;

	for (i = 31 - 8; i <= 31; i ++)
	{
		actors[i].x = x;
		actors[i].y = y;
		actors[i].dir = NONE;
		actors[i].type = PLAYER;
	}
}

void FootPrint_Update(int x, int y)
{
	if (((x & 15) == 0) && ((y & 15) == 0)) //&& (actors[31].x != x || actors[31].y != y))
	{
		actors[23] = actors[24];
		actors[24] = actors[25];
		actors[25] = actors[26];
		actors[26] = actors[27];
		actors[27] = actors[28];
		actors[28] = actors[29];
		actors[29] = actors[30];
		actors[30] = actors[31];

		actors[31].x = x;
		actors[31].y = y;
		actors[31].dir = NONE;
		actors[31].type = PLAYER;	
	}
}

void HUD_DrawLives(int nbLives)
{
	int i;
	
	for (i = 0; i < nbLives; i++)
	{
		Menu_LoadText(i,0,"!");
	} 
}

void HUD_DrawScore(int score)
{
	int tmp = score > 9999 ? 9999: score;
	char text[5] = "0000";
	
	text[4] = '\0';
	text[3] = '0' + (tmp % 10); tmp /= 10;
	text[2] = '0' + (tmp % 10); tmp /= 10;
	text[1] = '0' + (tmp % 10); tmp /= 10;
	text[0] = '0' + (tmp % 10); 				
				
	Menu_LoadText(240/8 - 4, 0, text);	
}


//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	int i;
	int frame_counter;
	int counter;
	int x; 
	int y; 
	enum DIRECTION dir;
	enum DIRECTION orient;
	int motion;
	int isDie;
	int dieFrameCounter;
	int nbLives;
	int nbTotalCoins;
	int score;
	int level;

	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);



	while (true)
	{
		Map_Clear();

		SetMode(MODE_0);
	
		// copy palette and data
		dmaCopy(iceblox_palette + 0, SPRITE_PALETTE + 0, 256);
		dmaCopy(((void*)iceblox_data) + 0, OBJ_BASE_ADR + 0, 13 * 16 * 16);

		VBlankIntrWait();
		VBlankIntrWait();
		VBlankIntrWait();
		VBlankIntrWait();
		VBlankIntrWait();
		

		SetMode(MODE_0 | BG0_ENABLE| OBJ_ENABLE| OBJ_1D_MAP| WIN0_ENABLE);
	
	
		Menu_Init();
		Menu_Loop();
		Menu_Clear();

		Map_Init();
		

		for(i = 0; i < 128; i++)
		{
			OAM[i].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
			OAM[i].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
			OAM[i].attr2 = 0;
		}
		
		
		// Penguin position
		x = 16;
		y = 16;
		nbLives = 3;

		// Penguin
		OAM[127].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
		OAM[127].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
		OAM[127].attr2 = 0;

				
		Actor_Create(8*16, 5*16, LEFT, 2, FIRE, 32); 
		Actor_Create(8*16, 5*16, RIGHT, 2, FIRE, 32); 
		Actor_Create(8*16, 5*16, UP, 2, FIRE, 32); 
		Actor_Create(8*16, 5*16, DOWN, 2, FIRE, 32);

		
		FootPrint_Init(x, y);
		
		Map_Init();
		Map_LoadLevel(0);
		Map_Update();

		Intro_Show();
	
		level = 0;
		dir = orient = NONE;
		motion = 0;
		isDie = 0;
		dieFrameCounter = 0;
		counter = frame_counter = 0;
		nbTotalCoins = Map_GetNbCoins();
		score = 0;
		while (1) 
		{
			if ((frame_counter ++ & 3) == 0)
			{
				if ((x & 15) == 0 && (y & 15) == 0 && (counter&1))
				{
					enum DIRECTION oldDir;

					oldDir = dir;
					dir = Input_GetDirection();

					if (dir != NONE) orient = dir;
					motion = (oldDir == dir) && (dir != NONE) && !isDie && Penguin_canMove(x,y, dir);
				}	
				
				// motion
				if (motion)
				{
					x += 4 * Direction_GetDx(dir);
					y += 4 * Direction_GetDy(dir);
					
					FootPrint_Update(x, y);

					OAM[127].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
					OAM[127].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
				}

				// actions
				else if (dir != NONE && !Penguin_canMove(x,y, dir))
				{
					// Get a coin
					if (Map_GetBlock(x,y,dir) == COIN)
					{
						Map_GetCoin(x,y,dir);
						Map_Update();
						
						nbTotalCoins--;
						if (nbTotalCoins == 0)
						{
							// win
							score += 1000;
							HUD_DrawScore(score);
							
							dmaCopy(((void*)iceblox_data) + 16 * 16 * 39, OBJ_BASE_ADR + 0 * 16 * 16, 16 * 16);
							
					
							// wait 2 seconds
							Intro_Wait(frame_counter, 2 * 60);
						
						
							Intro_Hide();
							if (level == 5)
							{
								Map_Clear();
								SetMode(MODE_0 | BG0_ENABLE | OBJ_1D_MAP);

								// game win
								Menu_LoadText(240/8/2- 9/2, 160/8/2,"game win");
								
								// wait 3 seconds
								Intro_Wait(frame_counter, 3 * 60);
								break;
							}
							else
							{
								level++;
								Map_LoadLevel(level);
								nbTotalCoins = Map_GetNbCoins();
								nbActors = 0;
								Actor_Create(8*16, 5*16, LEFT, 2, FIRE, 32); 
								Actor_Create(8*16, 5*16, RIGHT, 2, FIRE, 32); 
								Actor_Create(8*16, 5*16, UP, 2, FIRE, 32); 
								Actor_Create(8*16, 5*16, DOWN, 2, FIRE, 32);
								x = 16;
								y = 16;
								FootPrint_Init(x, y);
								OAM[127].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
								OAM[127].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
								dmaCopy(((void*)iceblox_data) + 16 * 16 * Penguin_GetAnim(NONE, 0, 0), OBJ_BASE_ADR + 0 * 16 * 16, 16 * 16);
								dir = orient = NONE;
								isDie = 0;
								dieFrameCounter = 0;
								Intro_Show();
							}
							
						}
					}
					// Break a block
					else if (Map_CanBreakBlock(x, y, dir))
					{
						score += Map_BreakBlock(x, y, dir);
						Map_Update();
					}
					// Slide a block
					else if(Map_CanSlideBlock(x, y, dir))
					{
						Map_SlideBlock(x, y, dir);
						Map_Update();			
					}
				}
				
				// Penguin-Fire collision checking			
				for (i = 0; i < nbActors; i ++)
				{
					if (actors[i].type == FIRE && Actor_IsCollideWitdh(x,y, i))
					{
						if (!isDie)
						{
							dieFrameCounter = 0;
						}
						isDie = 1;
						dir = NONE;
					}
				}
				
				
				// Penguin
				if (isDie)
				{
					dmaCopy(((void*)iceblox_data) + 16 * 16 * Penguin_GetDieAnim(dieFrameCounter), OBJ_BASE_ADR + 0 * 16 * 16, 16 * 16);
				
					if (dieFrameCounter < 7)
					{
						dieFrameCounter ++;
					}
					else if (dieFrameCounter == 7)
					{
						for (i = 0; i < 6; i++)
						{
							Map_Update();
							HUD_DrawLives(nbLives - (i&1));						
						
							Intro_Wait(frame_counter, 2 * 60 / 6); 
						}
						
						Intro_Hide();
											
						nbLives--;
						if (nbLives == 0)
						{
							// game over
							nbActors = 0;
							Map_Clear();
							SetMode(MODE_0 | BG0_ENABLE | OBJ_1D_MAP);
							Menu_LoadText(240/8/2- 9/2, 160/8/2,"game over");
							
							// wait 3 seconds
							Intro_Wait(frame_counter, 3 * 60);
							break;
						}
						else
						{						
							Map_LoadLevel(level);
							nbTotalCoins = Map_GetNbCoins();
							nbActors = 0;
							Actor_Create(8*16, 5*16, LEFT, 2, FIRE, 32); 
							Actor_Create(8*16, 5*16, RIGHT, 2, FIRE, 32); 
							Actor_Create(8*16, 5*16, UP, 2, FIRE, 32); 
							Actor_Create(8*16, 5*16, DOWN, 2, FIRE, 32);
							x = 16;
							y = 16;
							FootPrint_Init(x, y);						
							OAM[127].attr0 = OBJ_Y(y) | OBJ_256_COLOR;
							OAM[127].attr1 = OBJ_X(x) | OBJ_SIZE(Sprite_16x16);
							dmaCopy(((void*)iceblox_data) + 16 * 16 * Penguin_GetAnim(NONE, 0, 0), OBJ_BASE_ADR + 0 * 16 * 16, 16 * 16);
							dir = orient = NONE;
							isDie = 0;
							dieFrameCounter = 0;
							Intro_Show();
						}

					
					}
				}
				else
				{
					dmaCopy(((void*)iceblox_data) + 16 * 16 * Penguin_GetAnim(orient, motion, counter), OBJ_BASE_ADR + 0 * 16 * 16, 16 * 16);
				}

				// Other
				score += Actor_UpdateAll();
				Actor_RenderAll(counter);
				counter++;
			}

			Map_Update();
			
			HUD_DrawLives(nbLives);
			HUD_DrawScore(score);
			
			
			// Pause Management
			if (Input_IsStartPressed())
			{
				SetMode(MODE_0 | BG0_ENABLE | OBJ_1D_MAP);
				Menu_LoadText(240/8/2- 5/2, 160/8/2,"pause");
			
				Intro_Wait(frame_counter, 0.25 * 60);
			
				while (!Input_IsStartPressed())
				{
					VBlankIntrWait();
				}
				
				Intro_Wait(frame_counter, 0.25 * 60);
				
				SetMode(MODE_0 | BG0_ENABLE| OBJ_ENABLE| OBJ_1D_MAP| WIN0_ENABLE);
			}
			
			VBlankIntrWait();
		}
	}
}




