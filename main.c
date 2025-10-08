// TEST
#include <stdlib.h>
#include <stdio.h>
#include "include/gba.h"
#include "gbatext.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern const unsigned char fontdata_6x8[];
int seed = 0;

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u16 setColor(u8 red, u8 green, u8 blue);

typedef struct
{
	int row;
	int col;
	int rd;
	int cd;
	int color;
} MOVOBJ;

MOVOBJ car;
MOVOBJ enemy;

void initCar()
{
	car.row = 100;
	car.col = 100;
	car.rd = 0;
	car.cd = 0;
	car.color = setColor(31,0,0);
}

void initEnemy()
{
	enemy.row = 10;
	enemy.col = 10;
	enemy.rd = 0;
	enemy.cd = 0;
	enemy.color = setColor(0,0,31);
}

void drawCar()
{
	DMANow(3, &car.color, &videoBuffer[OFFSET(car.row, car.col, 240)], DMA_SOURCE_FIXED | 1);
}

void undrawCar()
{
	DMANow(3, &BLACK, &videoBuffer[OFFSET(car.row, car.col, 240)], DMA_SOURCE_FIXED | 1);
}

void drawEnemy()
{
	DMANow(3, &enemy.color, &videoBuffer[OFFSET(enemy.row, enemy.col, 240)], DMA_SOURCE_FIXED | 1);
}

void undrawEnemy()
{
	DMANow(3, &BLACK, &videoBuffer[OFFSET(enemy.row, enemy.col, 240)], DMA_SOURCE_FIXED | 1);
}

u16 setColor(u8 red, u8 green, u8 blue)
{
	return (red & 0x1F) | (green & 0x1F) << 5 | (blue & 0x1F) << 10;
}

void drawRect(int row, int col, int height, int width, u16 color)
{
	for(int r=0; r<height; r++)
	{
		DMANow(3, &color, &videoBuffer[OFFSET(row+r, col, 240)], DMA_SOURCE_FIXED | width);
	}
}

int main()
{
	REG_DISPCNT = MODE3 | BG2_ENABLE;
	initCar();
	initEnemy();
	drawCar();
	drawEnemy();

	while(1)
	{
		waitForVblank();
		undrawCar();
		undrawEnemy();

		if(KEY_DOWN(BUTTON_UP))
		{
			car.row -= 1;
		}
		if(KEY_DOWN(BUTTON_DOWN))
		{
			car.row += 1;
		}
		if(KEY_DOWN(BUTTON_LEFT))
		{
			car.col -= 1;
		}
		if(KEY_DOWN(BUTTON_RIGHT))
		{
			car.col += 1;
		}

		enemy.row += 1;
		if(enemy.row > 160)
		{
			enemy.row = 0;
			enemy.col = rand() % 240;
		}

		if(car.row < 0) car.row = 0;
		if(car.row > 159) car.row = 159;
		if(car.col < 0) car.col = 0;
		if(car.col > 239) car.col = 239;

		drawCar();
		drawEnemy();
	}

	return 0;
}