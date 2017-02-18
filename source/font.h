#ifndef FONT_H
#define FONT_H

#include <gba.h>
#include <stdlib.h>
#include "iceblox_data.h"

void Font_Init();
void Font_LoadLetters(int inc, void * data);
void Font_LoadChar(int inc, void * data, char c);


#endif

