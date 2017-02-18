#ifndef MAP_H
#define MAP_H

#include <gba.h>
#include "enum.h"
#include "actor.h"
#include "font.h"
#include "direction.h"
#include "iceblox_data.h"


extern enum BLOCK map_data[];

void Map_Init(void);
void Map_Update(void);
void Map_LoadLevel(int level);
void Map_Clear();
void Map_Show(void);
void Map_Hide(void);
int Map_GetNbCoins();
enum BLOCK Map_GetBlock(int x, int y, enum DIRECTION dir);
int Map_CanBreakBlock(int x, int y, enum DIRECTION dir);
int Penguin_canMove(int x, int y, enum DIRECTION dir);
int Map_BreakBlock(int x, int y, enum DIRECTION dir);
int Map_CanSlideBlock(int x, int y, enum DIRECTION dir);
void Map_SlideBlock(int x, int y, enum DIRECTION dir);
int Map_GetCoin(int x, int y, enum DIRECTION dir);
#endif

