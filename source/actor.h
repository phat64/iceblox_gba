#ifndef ACTOR_H
#define ACTOR_H

#include <gba.h>
#include <stdlib.h>
#include "enum.h"
#include "anim.h"
#include "map.h"
#include "direction.h"
#include "iceblox_data.h"

struct Actor
{
	int x;
	int y;
	enum DIRECTION dir;
	int speed;
	int type;
	int sprite;
};

extern int nbActors;
extern struct Actor actors[32];

void Actor_Create(int x, int y, enum DIRECTION dir, int speed, enum TYPE type, int sprite);
void Actor_Delete(int idx);
enum DIRECTION Actor_RandomDirectionDifferentThan(enum DIRECTION dir);
int Actor_UpdateAll();
void Actor_RenderAll(int frame_counter);
int Actor_IsCollideWitdh(int x0, int y0, int idx0);
int Actor_IsCollide(int idx0, int idx1);

int Actor_GetDistanceManatthan(int idx0, int idx1);
int Actor_GetNbObstaclesBetween(int idx0, int idx1);
enum DIRECTION Actor_GetDirectionBetween(int idx0, int idx1);

#endif
