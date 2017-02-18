#include "map.h"

enum BLOCK map_data[15 * 10];

void Map_Init(void)
{
	dmaCopy(iceblox_palette + 0, BG_PALETTE + 0, 256);
	
	REG_BG0CNT = TEXTBG_SIZE_256x256 | CHAR_BASE(0)|SCREEN_BASE(8)|BG_256_COLOR;
	REG_DISPCNT |= BG0_ENABLE;
	
	dmaCopy(((u8*)iceblox_data) + 0, TILE_BASE_ADR(0) + 0, 16 * 16 * 16);
	dmaCopy(NULL, TILE_BASE_ADR(0) + 0, 1 *  16*16);

	Font_LoadLetters(1,TILE_BASE_ADR(0));

	Map_LoadLevel(0); 
	Map_Update();
}
			
void Map_Update(void)
{
	int i;
	int j;
	u16* map;

	map = MAP_BASE_ADR(8);
	dmaCopy(NULL, map + 0, sizeof(u16) * 32 * 32);
	for (j = 0; j < 10; j++)
	{
		for (i = 0; i < 15; i++)
		{
			int blockid;
			int x;
			int y;
			
			blockid = map_data[j *15 + i] * 4;
			x = i * 2;
			y = j * 2;
						
			map[32*(y+0) + x + 0] = blockid + 0;
			map[32*(y+0) + x + 1] = blockid + 1;
			map[32*(y+1) + x + 0] = blockid + 2;
			map[32*(y+1) + x + 1] = blockid + 3;
		}
	}
}

void Map_LoadLevel(int level)
{		
	Map_Clear();
	map_data[0] = STONEBLOCK;
	map_data[1] = STONEBLOCK;
	map_data[15-2] = STONEBLOCK;
	map_data[15-1] = STONEBLOCK;
	srand(level);
	int nbStoneblock = 5;
	while (nbStoneblock > 0)
	{
		int x = rand() % 15;
		int y = rand() % 10;
		if (map_data[y * 15 + x] == EMPTYBLOCK)
		{
			map_data[y * 15 + x] = STONEBLOCK;
			nbStoneblock--;
		}
	}
	int nbCoinblock = 10 + level * 2;
	while (nbCoinblock > 0)
	{
		int x = rand() % 15;
		int y = rand() % 10;
		if (map_data[y * 15 + x] == EMPTYBLOCK)
		{
			map_data[y * 15 + x] = COINBLOCK_0;
			nbCoinblock--;
		}
	}
	
	int nbIceblock = 5 + (level * 5)/2;
	while (nbIceblock > 0)
	{
		int x = rand() % 15;
		int y = rand() % 10;
		if (map_data[y * 15 + x] == EMPTYBLOCK)
		{
			map_data[y * 15 + x] = ICEBLOCK_0;
			nbIceblock--;
		}
	}

	map_data[1 * 15 + 1] = EMPTYBLOCK;
	
	map_data[5 * 15 + 7] = EMPTYBLOCK;
	map_data[5 * 15 + 8] = EMPTYBLOCK;
	map_data[5 * 15 + 9] = EMPTYBLOCK;
	map_data[4 * 15 + 7] = EMPTYBLOCK;
	map_data[6 * 15 + 7] = EMPTYBLOCK;
	
	Map_Update();
}

void Map_Clear()
{
	int i;

	for (i = 0; i < 15 * 10; i++)
	{
		map_data[i] = EMPTYBLOCK;
	}
	Map_Update();
}

int Map_GetNbCoins()
{
	int i;
	int nb;

	nb = 0;
	for (i = 0; i < 15 * 10; i++)
	{
		enum BLOCK  block = map_data[i];
		
		if (block == COIN || (block >= COINBLOCK_0 && block <= COINBLOCK_7))
		{
			nb++;
		} 
	}
	return nb;
}

enum BLOCK Map_GetBlock(int x, int y, enum DIRECTION dir)
{
	int blockX;
	int blockY;

	blockX  = x >> 4;
	blockY  = y >> 4;
	blockX  += Direction_GetDx(dir);
	blockY  += Direction_GetDy(dir);
	
	if (blockX >=0 && blockX < SCREEN_WIDTH/16 && blockY >=0 && blockY < SCREEN_HEIGHT/16)
	{
		return map_data[blockY*15 + blockX];
	}
	return STONEBLOCK;
}

int Map_CanBreakBlock(int x, int y, enum DIRECTION dir)
{
	enum BLOCK currentBlock;
	enum BLOCK nextBlock;
	
	currentBlock = Map_GetBlock(x, y, dir);
	nextBlock = Map_GetBlock(x + 16 * Direction_GetDx(dir), y + 16 * Direction_GetDy(dir), dir);
	
	return ((currentBlock  >= ICEBLOCK_0 && currentBlock <= ICEBLOCK_7)
		||(currentBlock  >= COINBLOCK_0 && currentBlock <= COINBLOCK_7)) && nextBlock != EMPTYBLOCK;
}

int Penguin_canMove(int x, int y, enum DIRECTION dir)
{
	return Map_GetBlock(x, y, dir) == EMPTYBLOCK;
}

int Map_BreakBlock(int x, int y, enum DIRECTION dir)
{
	int blockX;
	int blockY;

	blockX  = x >> 4;
	blockY  = y >> 4;
	blockX  += Direction_GetDx(dir);
	blockY  += Direction_GetDy(dir);
	
	if (blockX >=0 && blockX < SCREEN_WIDTH/16 && blockY >=0 && blockY < SCREEN_HEIGHT/16)
	{
		enum BLOCK block = map_data[blockY*15 + blockX];
		//bool ret = false;
	
		if (block == ICEBLOCK_7)
		{
			map_data[blockY*15 + blockX] = EMPTYBLOCK;
			return 5; // 5 points
		}
		else if (block == COINBLOCK_7)
		{
			map_data[blockY*15 + blockX] = COIN;
			return 100; // 100 points
		}
		else if (block >= ICEBLOCK_0 && block < ICEBLOCK_7)
		{
			map_data[blockY*15 + blockX]++;
			return 0;
		}
		else if (block >= COINBLOCK_0 && block < COINBLOCK_7)
		{
			map_data[blockY*15 + blockX]++;
			return 0;
		}
	}
	return 0;
}



int Map_CanSlideBlock(int x, int y, enum DIRECTION dir)
{
	enum BLOCK currentBlock;
	enum BLOCK nextBlock;
	
	currentBlock = Map_GetBlock(x, y, dir);
	nextBlock = Map_GetBlock(x + 16 * Direction_GetDx(dir), y + 16 * Direction_GetDy(dir), dir);
	
	return ((currentBlock  >= ICEBLOCK_0 && currentBlock <= ICEBLOCK_7)
		||(currentBlock  >= COINBLOCK_0 && currentBlock <= COINBLOCK_7)) && nextBlock == EMPTYBLOCK;
}

void Map_SlideBlock(int x, int y, enum DIRECTION dir)
{
	int blockX;
	int blockY;

	blockX  = x >> 4;
	blockY  = y >> 4;
	blockX  += Direction_GetDx(dir);
	blockY  += Direction_GetDy(dir);
	
	if (blockX >=0 && blockX < SCREEN_WIDTH/16 && blockY >=0 && blockY < SCREEN_HEIGHT/16)
	{
		enum BLOCK block = map_data[blockY*15 + blockX];
		
		if (block >= ICEBLOCK_0 && block <= ICEBLOCK_7)
		{
			map_data[blockY*15 + blockX] = EMPTYBLOCK;
		}
		else if (block >= COINBLOCK_0 && block <= COINBLOCK_7)
		{
			map_data[blockY*15 + blockX] = EMPTYBLOCK;
		}
		
		Actor_Create(blockX <<4, blockY << 4, dir, 8, BLOC, block); 
		//Actor_Create(blockX <<4, blockY << 4, dir, 4, BLOC, block, 0, &FSM_BLOCK); 
	}
}


int Map_GetCoin(int x, int y, enum DIRECTION dir)
{
	int blockX;
	int blockY;

	blockX  = x >> 4;
	blockY  = y >> 4;
	blockX  += Direction_GetDx(dir);
	blockY  += Direction_GetDy(dir);
	
	if (blockX >=0 && blockX < SCREEN_WIDTH/16 && blockY >=0 && blockY < SCREEN_HEIGHT/16)
	{
		if (COIN == map_data[blockY*15 + blockX])
		{
			map_data[blockY*15 + blockX] = EMPTYBLOCK;
			return 1;
		}
	}
	return 0;
}
