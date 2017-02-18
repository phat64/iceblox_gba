#include "anim.h"

int Fire_GetAnim(int counter)
{
	static const int animF[8] =  {32, 33, 34, 35, 36, 35, 34, 33};
	return animF[counter & 7];
}

int Penguin_GetAnim(enum DIRECTION orient, int motion, int counter)
{
	static const int animP[5 * 4] = {0,0,0,0, 8,7,8,9, 11,10,11,12, 5,4,5,6, 2,1,2,3};
	return animP[4 * orient + motion * (counter & 3)];
}

int Penguin_GetDieAnim(int counter)
{
	static const int animDie[] = {40, 41, 42, 43, 44, 45, 46, 47};
	return animDie[counter];
}