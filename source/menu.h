#ifndef MENU_H
#define MENU_H

void Menu_Init();
void Menu_Clear();
void Menu_Loop();

void Menu_LoadLogo();
void Menu_LoadText(int x, int y, char * str);

int Math_Clamp(int x, int min, int max);
int Math_Wrap(int x, int min, int max);

#endif