
#include "include/gba.h"
#include "include/screenmode.h"
#include "include/keypad.h"
#include "include/gfx.h"

#include "gbatext.h"

#include "resources/mmaster.h"
#include "resources/ccar.h"
#include "resources/ssand.h"
#include "resources/rrock.h"
#include "resources/ssky.h"

u16* FrontBuffer = (u16*)0x6000000;
u16* BackBuffer  = (u16*)0x600A000;
u16* videoBuffer;   // video memory
u16* dontbreakpls= (u16*)0x4000016;
volatile u16* ScanlineCounter = (volatile u16*)0x4000006;
u16* paletteMem = (u16*)0x5000000;	//PalMemory is 256 16 bit BGR values

void plotPixel(int x, int y, unsigned short int c)
{
	videoBuffer[(y) * 120 + (x)] = (c);
}

void plotPixelTransparent(int x, int y, unsigned short int c)
{
	unsigned short int temp;
	if ((c & 0x00FF) == 0) // bottom is transparent
	{
		if ((c & 0xFF00) == 0) // top is also transparent
			return;
		// so bottom transparent, top is not so....
		temp = ((videoBuffer[(y)* 120 + (x)]) & 0x00FF);
		temp |= c; // or it with c (to get the highpart of c in
		videoBuffer[(y)* 120 + (x)] = (temp);
	}
	else
		if ((c & 0xFF00) == 0) // only the top is transprent
		{
			temp = ((videoBuffer[(y)* 120 + (x)]) & 0xFF00);
			temp |= c;
			videoBuffer[(y)* 120 + (x)] = (temp);
		}
		else // no part is transparent
			videoBuffer[(y)* 120 + (x)] = (c);
}

void waitForVblank();
void sleep(int);
void takeInput();
void gameLogic();
void drawSky();
void drawTerrain();
void drawCar();
void drawUI();
void flip();
void setPalette();
void setPaletteSky();
void setPaletteSand();

int jumping = 0;
int score = 0;
int carVY = 0;
int carX = 10;
int carY = 121 - car_HEIGHT;
int carSpeed = 0;
int floorX = 10000;
int floorSpeed = -2;
int jumpHeight = 7;

int main()
{	

	SetMode(MODE_4 | BG2_ENABLE);

	int x, y;
	for (x = 0; x < 120; x++)
	{
		for (y = 0; y < 160; y++)
		{
			plotPixel(x, y, 0x77B3);
		}
	}
	waitForVblank();
	flip();
	setPalette();

	while(1)
	{
		takeInput();
		gameLogic();
		drawSky();
		drawTerrain();
		drawCar();
		drawNumberLong(score, 115, 15);
		drawString("MY cAt]is2*GiNgEr4u lol", 20, 20);
		waitForVblank();
		flip();
	}
	return 0;
}

void takeInput()
{
		if		(!((*KEYS) & KEY_LEFT))
			carSpeed = -1;  
		else if (!((*KEYS) & KEY_RIGHT))
			carSpeed = 1;
		else
			carSpeed = 0;
		if(!((*KEYS) & KEY_A) && !jumping)
		{
			jumping = 1;
			carVY = -jumpHeight;
		}
}

void gameLogic()
{
	score++;
	floorX += floorSpeed;
	if ((carSpeed < 0 && carX >= -1 * carSpeed) || (carSpeed > 0 && carX < 120 - car_WIDTH / 2 - carSpeed / 2))
		carX += carSpeed;
	
	if (jumping)
	{
		carY += carVY;
		carVY++;
		
		if (carVY > jumpHeight)
		{
			carVY = 0;
			jumping = 0;
		}
	}
	
}

void drawSky()
{
	//setPaletteSky();
	int x, y;

	for (y = 0; y < sky_HEIGHT; y++)
	{
		for(x = 0; x < sky_WIDTH / 1; x++)
		{
			plotPixelTransparent(x, y, skyData[y * (sky_WIDTH / 2) + x]);
		}
	}
}

void drawTerrain()
{
	//draw floor
	int x, y;
	

	for (y = 0; y < sand_HEIGHT; y++)
	{
		for(x = floorX % 120; x < 120; x++)
		{
			plotPixelTransparent(x, y + 120, sandData[y * (sand_WIDTH / 2) + x - (floorX % 120)]);
		}
	}
	
	for (y = 0; y < sand_HEIGHT; y++)
	{
		for (x = (120 - floorX%120); x < 120; x++)
		{
			plotPixelTransparent(x + (floorX%120) - 120, y + 120, sandData[y * (sand_WIDTH / 2) + x]);
		}
	}
	
	//draw rocks
}

void drawCar()
{
	int x, y;

	for (y = 0; y < car_HEIGHT; y++)
	{
		for(x = 0; x < car_WIDTH / 2; x++)
		{
			plotPixelTransparent(x + carX, y + carY, carData[y * (car_WIDTH / 2) + x]); 
		}
	}
}

void drawUI()
{
	//draw "SCORE"
	
	//draw numeric score
}

void sleep(int i)
{
	int x, y;
	int c;
	for (y = 0; y < i; y++)
	{
		for (x = 0; x < 30000; x++)
			c = c + 2; // do something to slow things down
	}
}

void flip()			// flips between the back/front buffer
{
	if(REG_DISPCNT & BACKBUFFER) //back buffer is the current buffer so we need to switch it to the font buffer
	{ 
		REG_DISPCNT &= ~BACKBUFFER; //flip active buffer to front buffer by clearing back buffer bit
		videoBuffer = BackBuffer; //now we point our drawing buffer to the back buffer
    }
    else //front buffer is active so switch it to backbuffer
    { 
		REG_DISPCNT |= BACKBUFFER; //flip active buffer to back buffer by setting back buffer bit
		videoBuffer = FrontBuffer; //now we point our drawing buffer to the front buffer
	}
}

void waitForVblank()	// waits for the drawer to get to end before flip
{
	while (*ScanlineCounter < 160) {	// do nothing
	}//now we are in the vblank period
}

void setPalette()
{
	int z;
	for (z = 0; z < 256; z++)
		paletteMem[z] = masterPalette[z];
}

void setPaletteSky()
{
	int z;
	for (z = 0; z < 256; z++)
		paletteMem[z] = skyPalette[z];
}
void setPaletteSand()
{
	int z;
	for (z = 0; z < 256; z++)
		paletteMem[z] = sandPalette[z];
}
