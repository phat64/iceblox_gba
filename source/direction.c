#include "direction.h"

void Direction_GetDelta(enum DIRECTION dir, int* dx, int* dy)
{
	const int dirX[5] = {0, -1, 1, 0, 0};
	const int dirY[5] = {0, 0, 0 ,-1, 1};
	
	if (dx != NULL)
	{
		*dx = dirX[dir];
	}
	
	if (dy != NULL)
	{
		*dy = dirY[dir];
	}
}

int Direction_GetDx(enum DIRECTION dir)
{
	int dx;
	Direction_GetDelta(dir, &dx, NULL);
	return dx;
}

int Direction_GetDy(enum DIRECTION dir)
{
	int dy;
	Direction_GetDelta(dir, NULL, &dy);
	return dy;
}
