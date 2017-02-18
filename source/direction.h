
#ifndef DIRECTION_H
#define DIRECTION_H

#include <gba.h>
#include "enum.h"

void Direction_GetDelta(enum DIRECTION dir, int* dx, int* dy);
int Direction_GetDx(enum DIRECTION dir);
int Direction_GetDy(enum DIRECTION dir);
#endif