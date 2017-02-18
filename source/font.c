#include "font.h"

extern unsigned char font_8x8[2048];

void Font_Init()
{
	int idx;
	char c;
	//extern u16 iceblox_letters[/* 26 * 8 * 8 / 2 */];
	//extern u8 iceblox_font[/*2048*/]; // TODO : move to u16

	for (c = 'A'; c <= 'Z'; c++)
	{
		idx = c - 'A';

		idx ++;


	}
}

void Font_LoadLetters(int inc, void * data)
{
	char c;
	u16 * out;


	out = data;
	for (c = 'A'; c <= 'Z'; c ++)
	{
		Font_LoadChar(inc, out, c);
		out += 8 * 8 / 2;
	}
	
	for (c = '0'; c <= '9'; c ++)
	{
		Font_LoadChar(inc, out, c);
		out += 8 * 8 / 2;
	}
}


void Font_LoadChar(int inc, void * data, char c)
{
	int idx;
	int off;
	int i;
	int j;
	int k;
	u8 line8;
	u16 * out;
	u16 * tmp;

	inc = 1;
	if (c >= 'A' && c <= 'Z')
	{
		idx = /*1*/ inc+ (c - 'A');
	}
	else if (c >= 'a' && c <= 'z')
	{
		idx = /*1*/inc + (c - 'a');
	}
	else if (c >= '0' && c <= '9')
	{
		idx = /*1*/inc + 26 + (c - '0');
	}
	else
	{
		idx = 27;
	}
	
	

	// uncompress an 8x8 bitmaps letter 
	off = c * 8;;
	out = data;
	
	
	for (i = 0; i < 8; i ++)
	{
		line8 = font_8x8[off + i];
		tmp = out;
		for (j = 0, k = 3; j < 8; j+=2)
		{
		int b0 = line8 & 1? 6: 0;
		int b1 = line8 & 2? 6: 0;
		
		tmp[k--] = (b0<<8) | b1;
		out++;	
		
		line8 >>=2;
		}
	}
}
