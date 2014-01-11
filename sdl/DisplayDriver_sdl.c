/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  SDL Display driver
 *****************************************************************************/

#include "DisplayDriver_sdl.h"

#include "Graphics/Graphics.h"

// Clipping region control
SHORT       _clipRgn;

// Clipping region borders
SHORT       _clipLeft;
SHORT       _clipTop;
SHORT       _clipRight;
SHORT       _clipBottom;

// Color
GFX_COLOR    _color;

// SDL
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_PixelFormat *pixfmt;
static Uint32 redrawEvent;

/* local helper functions */
static void Cleanup(void);
static void ActivateCurrentColor(void);
static void ScheduleScreenUpdate(void);

static void Cleanup(void)
{
	if (pixfmt != NULL)
		SDL_FreeFormat(pixfmt);

	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);

	if (window != NULL)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

static void ActivateCurrentColor(void)
{
	Uint8 r, g, b;

	SDL_GetRGB(GetColor(), pixfmt, &r, &g, &b);
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

/* For performance reasons, the screen should not be updated after each drawing
 * of even a single pixel, but scheduled for the next event handling.
 * It would cause very slow drawing when updated directly in PutPixel(). */
static void ScheduleScreenUpdate(void)
{
	if (!SDL_HasEvent(redrawEvent)) {
		SDL_Event e;
		e.type = e.user.type = redrawEvent;
		SDL_PushEvent(&e);
	}
}

/* mandatory functions for Microchip Graphics Library */

void ResetDevice(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	atexit(Cleanup);

	window = SDL_CreateWindow(
			"MLAtor",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			#if (DISP_ORIENTATION == 90 || DISP_ORIENTATION == 270)
			DISP_VER_RESOLUTION, DISP_HOR_RESOLUTION,
			#else
			DISP_HOR_RESOLUTION, DISP_VER_RESOLUTION,
			#endif
			0);
	if (window == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	#if (COLOR_DEPTH == 8)
	pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGB332);
	#elif (COLOR_DEPTH == 16)
	pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGB565);
	#elif (COLOR_DEPTH == 24)
	pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);
	#else
	#error color depth unsupported
	#endif

	if (pixfmt == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate pixel format: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	redrawEvent = SDL_RegisterEvents(1);
	if (redrawEvent == ((Uint32)-1)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not register user events: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SetColor(BLACK);
	ActivateCurrentColor();
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

void PutPixel(SHORT x, SHORT y)
{
	ActivateCurrentColor();
	SDL_RenderDrawPoint(renderer, x, y);
	ScheduleScreenUpdate();
}

GFX_COLOR GetPixel(SHORT x, SHORT y)
{
	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GetPixel() NOT IMPLEMENTED YET\n");
	return 0;
}

WORD IsDeviceBusy(void)
{
	return 0;
}

/* accelerated functions to avoid primitives using PutPixel() */

WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
	SDL_Rect rect;

	rect.x = left;
	rect.y = top;
	rect.w = right - left + 1;
	rect.h = bottom - top + 1;

	if (left > right || top > bottom)
		// wrong parameters, draw nothing
		return 1;

	ActivateCurrentColor();
	SDL_RenderFillRect(renderer, &rect);
	ScheduleScreenUpdate();
	return 1;
}

/* SDL event handling */

void HandleGeneralEvent(SDL_Event *event)
{
	switch (event->type) {
	case SDL_WINDOWEVENT:
		if (event->window.event == SDL_WINDOWEVENT_EXPOSED) {
			SDL_RenderPresent(renderer);
		}
		break;

	case SDL_QUIT:
		exit(EXIT_SUCCESS);
		break;

	case SDL_KEYDOWN:
	case SDL_KEYUP:
	case SDL_TEXTINPUT:
		// event ignored
		break;

	default:
		if (event->type == redrawEvent) {
			SDL_RenderPresent(renderer);
		} else {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled SDL event: 0x%x\n", event->type);
		}
		break;
	}
}
