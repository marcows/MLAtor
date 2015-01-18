#include "MLAtorTest.h"

#include "Graphics/Graphics.h"
#include <SDL.h>

/*
 * Write a pixel onto the screen and immediately read it back again each 100
 * ms. Print both values to stdout with extra noise if they differ. Use the
 * coordinate range (x,y) of (0,0)..(xmax,0) and then start over.
 */
void MLAtor_PixelReadWriteTest(void)
{
	static Uint32 prevTicks;
	Uint32 ticks = SDL_GetTicks();

	/* do stuff every 100 ms */
	if (ticks - prevTicks >= 100) {
		GFX_COLOR colPut, colGet;
		static int x;

		prevTicks = ticks;

		srand(SDL_GetTicks());

		#if COLOR_DEPTH == 24
		colPut = rand() & 0xFFFFFF;
		#elif COLOR_DEPTH == 16
		colPut = rand() & 0xFFFF;
		#elif COLOR_DEPTH == 8
		colPut = rand() & 0xFF;
		#elif COLOR_DEPTH == 4
		colPut = rand() & 0xF;
		#elif COLOR_DEPTH == 1
		colPut = rand() & 0x1;
		#endif

		SetColor(colPut);
		PutPixel(x, 0);
		colGet = GetPixel(x, 0);

		#if COLOR_DEPTH == 24
		printf("%06X -> %06X", colPut, colGet);
		#elif COLOR_DEPTH == 16
		printf("%04X -> %04X", colPut, colGet);
		#elif COLOR_DEPTH == 8
		printf("%02X -> %02X", colPut, colGet);
		#elif COLOR_DEPTH == 4
		printf("%01X -> %01X", colPut, colGet);
		#elif COLOR_DEPTH == 1
		printf("%01X -> %01X", colPut, colGet);
		#endif

		if (colPut != colGet)
			printf(" (DIFFERENT!)\n");
		else
			printf("\n");

		if (x < GetMaxX())
			x++;
		else
			x = 0;
	}
}
