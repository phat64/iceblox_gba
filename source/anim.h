#ifndef ANIM_H
#define ANIM_H

#include "enum.h"

int Fire_GetAnim(int counter);
int Penguin_GetAnim(enum DIRECTION orient, int motion, int counter);
int Penguin_GetDieAnim(int counter);
#endif