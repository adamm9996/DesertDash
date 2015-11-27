#include "gbatext.h"
#include "resources/nnumbers.h"
#include "resources/aalphabet.h"

void drawNumberLong(unsigned int n, int screenX, int screenY)
{
	if (n < 10)
	{
		drawNumber(n, screenX, screenY);
	}
	else if (n < 100)
	{
		drawNumber(n / 10, screenX, screenY);
		drawNumber(n % 10, screenX + 5, screenY);
	}
	else if (n < 1000)
	{
		drawNumber(n / 100, screenX, screenY);
		drawNumber((n / 10) % 10, screenX + 5, screenY);
		drawNumber(n % 10, screenX + 10, screenY);
	}
}

void drawNumber(unsigned int n, int screenX, int screenY)
{
	if (screenX >= 0 && screenX <= 115 && screenY >= 0 && screenY <= 160)
	{
		int x, y;
		for (y = 0; y < numbers_HEIGHT; y++)
		{
			for(x = 0; x < 5; x++)
			{
				plotPixelTransparent(screenX + x, screenY + y, numbersData[y * (numbers_WIDTH / 2) + x + 5 * n]);
			}
		}
	}
}

void drawChar(char c, int screenX, int screenY)
{
	if (screenX >= 0 && screenX <= 115 && screenY >= 0 && screenY <= 160)
	{
		int offset = c - 'a';
		int x, y;
		for (y = 0; y < alphabet_HEIGHT; y++)
		{
			for(x = 0; x < 5; x++)
			{
				plotPixelTransparent(screenX + x, screenY + y, alphabetData[y * (alphabet_WIDTH / 2) + x + 5 * offset]);
			}
		}
	}
}

void drawString(const char* s, int screenX, int screenY)
{
	int t = 0;
	while(s[t] != '\0')
	{
		if 		(s[t] >= 'A' && s[t] <= 'Z')//convert capitals to lower case
		{
			drawChar(s[t]+32, screenX + 5 * t, screenY);
		}
		else if (s[t] >= 'a' && s[t] <= 'z')
		{
			drawChar(s[t], screenX + 5 * t, screenY);
		}
		else if (s[t] >= '0' && s[t] <= '9')
		{
			drawNumber(s[t] - '0', screenX + 5 * t, screenY);
		}
		t++;
	}
}
