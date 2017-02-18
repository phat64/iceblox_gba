/*#include "actor.h"

int nbActors = 0;
struct Actor actors[32];

struct FSM FSM_FIFTY =
{
	"FSM_FIFTY", NULL, NULL
};

void GBA_print(char *s)
{
	// THUMB code
	asm volatile("mov r0, %0;"
                 "swi 0xff;"
                 : // no ouput
                 : "r" (s)
                 : "r0");

	// ARM code
	//asm volatile("mov r0, %0;"
    //             "swi 0xff0000;"
    //             : // no ouput
    //             : "r" (s)
    //             : "r0");
}


void GBA_printi(int i)
{
	const int BUFFER_LEN = 80;
	char buffer[BUFFER_LEN];
	char * str;
	int neg;
	char c;

	buffer[BUFFER_LEN - 1] = '\0';

	str = &buffer[BUFFER_LEN - 1];
	neg = (i < 0);
	i = abs(i);

	if (i == 0)
	{
		str --;
		*str = '0';		
	}
	else while (i > 0)
	{
		c = '0' + (i % 10);
		str --;
		*str = c;
		i /= 10;
	}

	if (neg)
	{
		str --;
		*str = '-';	
	}

	GBA_print(str);
}


int Actor_Create(int x, int y, enum DIRECTION dir, int speed, enum TYPE type, int sprite, int framecounter_offset, struct FSM * fsm)
{
	int idx = nbActors;
	actors[idx].x = x;
	actors[idx].y = y;
	actors[idx].dir = dir;
	actors[idx].speed = speed;
	actors[idx].type = type;
	actors[idx].sprite = sprite;
	actors[idx].framecounter_offset = framecounter_offset;
	actors[idx].timer = 0;
	actors[idx].fsm = *fsm;
	nbActors ++;
	return idx;
}

void Actor_Delete(int idx)
{
	if (nbActors >= 2)
	{
		actors[idx] = actors[nbActors-1];
	}
	nbActors --;
}

void Actor_ChangeSprite(int idx, int sprite)
{
	actors[idx].sprite = sprite;
}

void Actor_ChangePosition(int idx, int x, int y)
{
	actors[idx].x = x;
	actors[idx].y = y;
}

enum DIRECTION RandomDirectionDifferentThan(enum DIRECTION dir)
{
	static enum DIRECTION dirs[4] = {LEFT, RIGHT, UP, DOWN};
	enum DIRECTION newDir;
	int i;
	
	i = rand() & 3;
	newDir = dirs[i];

	if (newDir == dir)
	{
		i = (i + 1) & 3;	
		newDir = dirs[i];
	}

	return newDir;
}

enum DIRECTION GetOppositeDirection(enum DIRECTION dir)
{
	enum DIRECTION newDir;

	newDir = NONE;
	switch(dir)
	{
		case LEFT:	newDir = RIGHT;	break;
		case RIGHT: newDir = LEFT;	break;
		case UP:	newDir = DOWN;	break;
		case DOWN:	newDir = UP;	break;
		case NONE:	newDir = NONE;	break;
	}

	return newDir;
}

int Actor_GetDistanceManatthan(int idx0, int idx1)
{
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;

	return abs(x1 -x0) + abs(y1 - y0);
}

int Actor_GetNbObstaclesBetween(int idx0, int idx1)
{
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;
	int dx = x1 - x0;
	int dy = y1 - y0;
	int sdx = dx >= 0? 1 : -1;
	int sdy = dy >= 0? 1 : -1;
	int counter = 0;

	if ((x0 == x1))
	{
		while (y0 < y1)
		{
			if (Map_GetBlock(x0, y0, NONE) != EMPTYBLOCK)
			{
				counter ++;	
			}
			y0 += 16 * sdy;
		}
	}
	else if ((y0 == y1))
	{
		while (x0 < x1)
		{
			if (Map_GetBlock(x0, y0, NONE) != EMPTYBLOCK)
			{
				counter ++;
			}
			x0 += 16 * sdx;
		}
	}
	else
	{
		counter = 99;
	}

	return counter;
}

enum DIRECTION Actor_GetDirectionBetween(int idx0, int idx1)
{
	enum DIRECTION dir = NONE;
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (dy == 0)
	{
		if (dx < 0)
		{
			dir = LEFT; 
		}
		else if (dx > 0)
		{
			dir = RIGHT;
		}
	}
	else if (dx == 0)
	{
		if (dy < 0)
		{
			dir = UP;
			//dir = DOWN;
		}
		else if (dy > 0)
		{
			dir = DOWN;
			//dir = UP;
		}	
	}

	return dir;
}

void Actor_UpdatePosition(int idx)
{
	int offX;
	int offY;

	// update
	if (actors[idx].type == FIFTY)
	{
		actors[idx].x += (actors[idx].speed & 1) * Direction_GetDx(actors[idx].dir);
		actors[idx].y += (actors[idx].speed & 1) * Direction_GetDy(actors[idx].dir);

		if (actors[idx].speed > 0)
		{
			actors[idx].speed --;
		}
		return;
	}

	offX = actors[idx].x & 15;
	offY = actors[idx].y & 15;

	switch(actors[idx].speed)
	{
		case 3:
		{
			// 16 = 3 + 3 + 4 + 3 + 3
			if (offX == 0 || offY == 0)
			{
				actors[idx].x += Direction_GetDx(actors[idx].dir);
				actors[idx].y += Direction_GetDy(actors[idx].dir);				
			}
		}
		break;

		case 5:
		case 6:
		{
			// 16 = 5 + 6 + 5
			if (offX == 0 || offY == 0)
			{
				actors[idx].x += Direction_GetDx(actors[idx].dir);
				actors[idx].y += Direction_GetDy(actors[idx].dir);				
			}
		}
	}

	actors[idx].x += actors[idx].speed * Direction_GetDx(actors[idx].dir);
	actors[idx].y += actors[idx].speed * Direction_GetDy(actors[idx].dir);
}

void Actor_UpdateCollision(int i)
{
		// Collision Checking 
		switch (actors[i].type)
		{
			case BLOC:
			{
				int j;
				for (j = 0; j < nbActors; j++)
				{
					if (actors[j].type == FIRE && Actor_IsCollide(i, j))
					{
						Actor_Create(actors[j].x, actors[j].y, UP, 7, FIFTY, 38, 0, &FSM_FIFTY);
						Actor_Delete(j);
						j--;
					}
				}
			}
			break;
					
			case FIRE:
			{
			}
			break;
					
			case FIFTY:
			{
				if (0 == --actors[i].speed)
				{
					Actor_Delete(i);
					i--;
				}
			}	
			break;
		}	
}


void Actor_UpdateAI(int i)
{
	return;

		// UpdateAI
		if ((actors[i].x & 15) == 0 && (actors[i].y & 15) == 0 && actors[i].type == FIRE)
		{




			if(actors[i].timer > 0 && Actor_GetDistanceManatthan(31, i) < 7 * 16)
			{
				actors[i].timer --;





				int j;
				int isfollowing = 0;

				for (j = 31; (j >= (31 - 8)) && !isfollowing; j--)
				{
					//GBA_printi(j); GBA_print(" (");
					//GBA_printi(actors[j].x/16); GBA_print(",");
					//GBA_printi(actors[j].y/16); GBA_print(") ");
			
					//switch (Actor_GetDirectionBetween(i, j))
					//{
					//	case UP:		GBA_print("UP\n");		break;	// TOP
					//	case DOWN:		GBA_print("DOWN\n");	break;	// BOTTOM
					//	case LEFT:		GBA_print("LEFT\n");	break;	// LEFT
					//	case RIGHT:		GBA_print("RIGHT\n");	break;	// RIGHT
					//	default:		GBA_print("NONE\n");	break;	// NONE
					//}

					{
						if (Actor_GetNbObstaclesBetween(i, j) == 0)
						{

							int distanceNow = Actor_GetDistanceManatthan(31, i);
							int distancePrev = Actor_GetDistanceManatthan(30, i);

							//if ()
							{
								if (distanceNow <= distancePrev)
								{
									//if (distanceNow 
								}
								else
								{
			
								}
							}
						}
					}
											
					if (Actor_GetNbObstaclesBetween(i, j) == 0)
					{
						enum DIRECTION newDir;
						int oldSpeed;

						newDir = Actor_GetDirectionBetween(i, j);
						if (newDir != NONE && Map_GetBlock(actors[i].x, actors[i].y, newDir) == EMPTYBLOCK)
						{
							actors[i].dir = newDir;
							isfollowing = 1;
							oldSpeed = actors[i].speed;
							actors[i].speed = 2;//2-4;
							if (Actor_GetDistanceManatthan(31, i) <= (4.5 * 16) && actors[i].timer > 0)
							{
								actors[i].timer --;
								actors[i].speed ++;
							}
							if (Actor_GetDistanceManatthan(31, i) >= (1.8 * 16) && j == 31 && actors[i].dir == actors[31].dir && actors[i].timer > 0)
							{
								actors[i].timer /= 2;
								actors[i].speed ++;
							}
							

							// actor
							OAM[126].attr0 = OBJ_Y(actors[j].y) | OBJ_256_COLOR;
							OAM[126].attr1 = OBJ_X(actors[j].x) | OBJ_SIZE(Sprite_16x16);
							OAM[126].attr2 = 0;
							
						}
					}

				}

				if (!isfollowing)
				{
					OAM[126].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
					OAM[126].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
					OAM[126].attr2 = 0;
				}
			}
		}
		else if (actors[i].timer < 10)
		{
			GBA_print("respawn\n");
			actors[i].speed = 2;//2-4;
			actors[i].timer = 25; // nb cases of following
		}


		if ((actors[i].x & 15) == 0 && (actors[i].y & 15) == 0)
		{
			enum BLOCK block = Map_GetBlock(actors[i].x,actors[i].y,actors[i].dir);
			if (block != EMPTYBLOCK)
			{
				int blockX;
				int blockY;
			
				blockX = actors[i].x >> 4;
				blockY = actors[i].y >> 4;
				
				switch (actors[i].type)
				{
					case BLOC:
					{
						map_data[15 * blockY + blockX] = actors[i].sprite;
						Actor_Delete(i);
						i--;
						Map_Update();
					}
					break;
					
					case FIRE:
					{
						if (Map_GetBlock(actors[i].x, actors[i].y, actors[i].dir) != EMPTYBLOCK || actors[i].dir == NONE)
						{
							enum DIRECTION oldDir;
							enum DIRECTION dir0;
							enum DIRECTION dir1;
							enum DIRECTION dir2;

							oldDir = actors[i].dir;
							dir0 = RandomDirectionDifferentThan(oldDir);
							dir1 = GetOppositeDirection(dir0);
							dir2 = GetOppositeDirection(oldDir);

							if (Map_GetBlock(actors[i].x, actors[i].y, dir0) == EMPTYBLOCK)
							{
								actors[i].dir = dir0;							
							}
							else if (Map_GetBlock(actors[i].x, actors[i].y, dir1) == EMPTYBLOCK)
							{
								actors[i].dir = dir1;
							}
							else if (Map_GetBlock(actors[i].x, actors[i].y, dir2) == EMPTYBLOCK)
							{
								actors[i].dir = dir2;
							}
							else 
							{
								actors[i].dir = NONE;
							}
						}
					}
					break;
				}	
			}
		}
}


void Actor_UpdateAnimation(int i, int frame_count)
{


}

void Actor_UpdateAll(int frame_count)
{
	int i;

	for (i = 0; i < nbActors; i++)
	{
		Actor_UpdatePosition(i);
		Actor_UpdateAI(i);		
		Actor_UpdateCollision(i);	
	}
}

void Actor_RenderAll(int frame_counter)
{
	int  i;

	for (i = 0; i < nbActors; i++)
	{
		// actor
		OAM[i].attr0 = OBJ_Y(actors[i].y) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(actors[i].x) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = (5 + i) * 8;
	
		switch (actors[i].type)
		{
			case BLOC:		
				dmaCopy(((void*)iceblox_data) + 16 * 16 * actors[i].sprite , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;
			
			case FIRE:
				dmaCopy(((void*)iceblox_data) + 16 * 16 * Fire_GetAnim(frame_counter + actors[i].framecounter_offset) , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;
			
			case FIFTY:
				dmaCopy(((void*)iceblox_data) + 16 * 16 * actors[i].sprite , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;

			case GFX:
				dmaCopy(((void*)iceblox_data) + 16 * 16 * actors[i].sprite , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;
			
		}
	}
	for (i = nbActors; i < 128-1 - 1; i ++)
	{
		// actor
		OAM[i].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = 0;
	}
}

int Actor_IsCollideWitdh(int x0, int y0, int idx0)
{
	int x1;
	int y1;
	
	x1 = actors[idx0].x;
	y1 = actors[idx0].y;
	
	return (x0 >= x1 && x0 < (x1 + 16) && y0 >= y1 && y0 < (y1 + 16)) 
		|| (x1 >= x0 && x1 < (x0 + 16) && y1 >= y0 && y1 < (y0 + 16));

}

int Actor_IsCollide(int idx0, int idx1)
{
	int x0;
	int y0;
	
	x0 = actors[idx0].x;
	y0 = actors[idx0].y;
	
	return Actor_IsCollideWitdh(x0, y0, idx1);
}
*/

#include "actor.h"

int nbActors = 0;
struct Actor actors[32];

void Actor_Create(int x, int y, enum DIRECTION dir, int speed, enum TYPE type, int sprite)
{
	actors[nbActors].x = x;
	actors[nbActors].y = y;
	actors[nbActors].dir = dir;
	actors[nbActors].speed = speed;
	actors[nbActors].type = type;
	actors[nbActors].sprite = sprite;
	nbActors ++;
}

void Actor_Delete(int idx)
{
	if (nbActors >= 2)
	{
		actors[idx] = actors[nbActors-1];
	}
	nbActors --;
}
enum DIRECTION Actor_RandomDirectionDifferentThan(enum DIRECTION dir)
{
	enum DIRECTION newDir;
	
	newDir = dir;

	while(newDir == dir)
	{
		newDir = (enum DIRECTION) (1 +  (rand() & 3));
	}
	return newDir;
}

int Actor_UpdateAll()
{
	int i;
	int scoreInc;
	
	scoreInc = 0;
	
	for (i = 0; i < nbActors; i++)
	{
		// update
		if (actors[i].type == FIFTY)
		{
			actors[i].x += (actors[i].speed & 1) * Direction_GetDx(actors[i].dir);
			actors[i].y += (actors[i].speed & 1) * Direction_GetDy(actors[i].dir);
		}
		else
		{
			actors[i].x += actors[i].speed * Direction_GetDx(actors[i].dir);
			actors[i].y += actors[i].speed * Direction_GetDy(actors[i].dir);
		}
				
		// 
		if ((actors[i].x & 15) == 0 && (actors[i].y & 15) == 0)
		{
			enum BLOCK block = Map_GetBlock(actors[i].x,actors[i].y,actors[i].dir);
			if (block != EMPTYBLOCK)
			{
				int blockX;
				int blockY;
			
				blockX = actors[i].x >> 4;
				blockY = actors[i].y >> 4;
				
				switch (actors[i].type)
				{
					case BLOC:
					{
						map_data[15 * blockY + blockX] = actors[i].sprite;
						Actor_Delete(i);
						i--;
						Map_Update();
					}
					break;
					
					case FIRE:
					{

						while (Map_GetBlock(actors[i].x, actors[i].y, actors[i].dir) != EMPTYBLOCK)
						{
							actors[i].dir = Actor_RandomDirectionDifferentThan(actors[i].dir);
						}
					}
					break;
				}	
			}
			else
			{
				if (actors[i].type == FIRE)
				{	
					if (Actor_GetDistanceManatthan(i, 31) < 9 * 16)// && actors[i].dir == Actor_GetDirectionBetween(i, 31))
					{
						//actors[i].speed = 4;
					}
					else
					{
						//actors[i].speed = 2;
					}
					
					if (Actor_GetDistanceManatthan(i, 31) < 200)
					{
						if(Actor_GetNbObstaclesBetween(i, 31) == 0)
						{
							actors[i].dir = Actor_GetDirectionBetween(i, 31);
							while (Map_GetBlock(actors[i].x, actors[i].y, actors[i].dir) != EMPTYBLOCK)
							{
								actors[i].dir = Actor_RandomDirectionDifferentThan(actors[i].dir);
							}
						}
					}	
				}	
			}
		}
		
		// Collision Checking 
		switch (actors[i].type)
		{
			case BLOC:
			{
				int j;
				for (j = 0; j < nbActors; j++)
				{
					if (actors[j].type == FIRE && Actor_IsCollide(i, j))
					{
						Actor_Create(actors[j].x, actors[j].y, UP, 7, FIFTY, 38);
						Actor_Delete(j);
						j--;
					}
				}
			}
			break;
					
			case FIRE:
			{
			}
			break;
					
			case FIFTY:
			{
				if (0 == --actors[i].speed)
				{
					Actor_Delete(i);
					i--;
					scoreInc = 50;
				}
			}	
			break;
		}	
			
	}
	return scoreInc;
}

void Actor_RenderAll(int frame_counter)
{
	int  i;

	for (i = 0; i < nbActors; i++)
	{
		// actor
		OAM[i].attr0 = OBJ_Y(actors[i].y) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(actors[i].x) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = (5 + i) * 8;
	
		switch (actors[i].type)
		{
			case BLOC:		
				dmaCopy(((void*)iceblox_data) + 16 * 16 * actors[i].sprite , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;
			
			case FIRE:
				dmaCopy(((void*)iceblox_data) + 16 * 16 * Fire_GetAnim(frame_counter) , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;
			
			case FIFTY:
				dmaCopy(((void*)iceblox_data) + 16 * 16 * actors[i].sprite , OBJ_BASE_ADR + (5 + i) * 16 * 16, 16 * 16);
			break;

			
		}
	}
	for (i = nbActors; i < 128-1; i ++)
	{
		// actor
		OAM[i].attr0 = OBJ_Y(160) | OBJ_256_COLOR;
		OAM[i].attr1 = OBJ_X(240) | OBJ_SIZE(Sprite_16x16);
		OAM[i].attr2 = 0;
	}
}

int Actor_IsCollideWitdh(int x0, int y0, int idx0)
{
	int x1;
	int y1;
	
	x1 = actors[idx0].x;
	y1 = actors[idx0].y;
	
	return ((x0 >= x1) && x0 < (x1 + 16) && (y0 >= y1) && y0 < (y1 + 16)) 
		|| ((x1 >= x0) && x1 < (x0 + 16) && (y1 >= y0) && y1 < (y0 + 16));

}

int Actor_IsCollide(int idx0, int idx1)
{
	int x0;
	int y0;
	
	x0 = actors[idx0].x;
	y0 = actors[idx0].y;
	
	return Actor_IsCollideWitdh(x0, y0, idx1);
}


int Actor_GetDistanceManatthan(int idx0, int idx1)
{
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;

	return abs(x1 -x0) + abs(y1 - y0);
}

int Actor_GetNbObstaclesBetween(int idx0, int idx1)
{
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;
	int dx = x1 - x0;
	int dy = y1 - y0;
	int sdx = dx >= 0? 1 : -1;
	int sdy = dy >= 0? 1 : -1;
	int counter = 0;

	if ((x0 == x1))
	{
		while (y0 < y1)
		{
			if (Map_GetBlock(x0, y0, NONE) != EMPTYBLOCK)
			{
				counter ++;	
			}
			y0 += 16 * sdy;
		}
	}
	else if ((y0 == y1))
	{
		while (x0 < x1)
		{
			if (Map_GetBlock(x0, y0, NONE) != EMPTYBLOCK)
			{
				counter ++;
			}
			x0 += 16 * sdx;
		}
	}
	else
	{
		counter = 99;
	}

	return counter;
}

enum DIRECTION Actor_GetDirectionBetween(int idx0, int idx1)
{
	enum DIRECTION dir = NONE;
	int x0 = actors[idx0].x;
	int y0 = actors[idx0].y;
	int x1 = actors[idx1].x;
	int y1 = actors[idx1].y;
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (dy == 0)
	{
		if (dx < 0)
		{
			dir = LEFT; 
		}
		else if (dx > 0)
		{
			dir = RIGHT;
		}
	}
	else if (dx == 0)
	{
		if (dy < 0)
		{
			dir = UP;
			//dir = DOWN;
		}
		else if (dy > 0)
		{
			dir = DOWN;
			//dir = UP;
		}	
	}

	return dir;
}
