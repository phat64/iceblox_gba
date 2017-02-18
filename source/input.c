#include "input.h"

int Input_IsStartPressed()
{
	u16 keys; 
	
	scanKeys();
	keys = keysHeld();
	
	return (keys & KEY_START);
}

enum DIRECTION Input_GetDirection()
{
	u16 keys = REG_KEYCNT; 
	
	scanKeys();
	keys = keysHeld();
	
	if (keys & KEY_LEFT)
	{
		return LEFT;
	}
	else if (keys & KEY_RIGHT)
	{
		return RIGHT;
	}
	else if (keys & KEY_UP)
	{
		return UP;
	}
	else if (keys & KEY_DOWN)
	{
		return DOWN;
	}
	
	return NONE;
}

