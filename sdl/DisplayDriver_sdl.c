/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  SDL Display driver
 *****************************************************************************/

#include "DisplayDriver_sdl.h"

#include "Graphics/Graphics.h"

#ifdef USE_PALETTE
#if (COLOR_DEPTH == 1)
  #define MAX_NUM_PALETTE_ENTRIES 2
#elif (COLOR_DEPTH == 4)
  #define MAX_NUM_PALETTE_ENTRIES 16
#elif (COLOR_DEPTH == 8)
  #define MAX_NUM_PALETTE_ENTRIES 256
#else
  #error color depth unsupported with palette
#endif
#endif

// Clipping region control
SHORT       _clipRgn;

// Clipping region borders
SHORT       _clipLeft;
SHORT       _clipTop;
SHORT       _clipRight;
SHORT       _clipBottom;

// Color
GFX_COLOR    _color;

#ifdef USE_TRANSPARENT_COLOR
GFX_COLOR    _colorTransparent;
SHORT        _colorTransparentEnable;
#endif

#ifdef USE_PALETTE
extern void *_palette;
extern BYTE PaletteBpp;
extern BYTE blPaletteChangeError;
extern void *pPendingPalette;
extern WORD PendingStartEntry;
extern WORD PendingLength;
#endif

// SDL
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_PixelFormat *pixfmt;
static Uint32 redrawEvent;

#ifdef USE_PALETTE
static SDL_Palette *palette;
#if (COLOR_DEPTH == 8)
// pixel formats can be switched to toggle between indexed and RGB colors
static SDL_PixelFormat *pixfmt_idx, *pixfmt_rgb;
#endif
#endif

/* local helper functions */
static void Cleanup(void);
static void ActivateCurrentColor(void);
static void ScheduleScreenUpdate(void);

static void Cleanup(void)
{
	#ifdef USE_PALETTE
	if (palette != NULL)
		SDL_FreePalette(palette);

	#if (COLOR_DEPTH == 8)
	// prevent double freeing
	if (pixfmt_idx != pixfmt) {
		if (pixfmt_idx != NULL)
			SDL_FreeFormat(pixfmt_idx);
	} else {
		if (pixfmt_rgb != NULL)
			SDL_FreeFormat(pixfmt_rgb);
	}
	#endif
	#endif

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

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	#if (COLOR_DEPTH == 1)
	pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX1LSB);
	#elif (COLOR_DEPTH == 4)
	pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX4LSB);
	#elif (COLOR_DEPTH == 8)
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

	#ifdef USE_PALETTE
	#if (COLOR_DEPTH == 8)
	pixfmt_rgb = pixfmt;

	pixfmt_idx = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
	if (pixfmt_idx == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate indexed pixel format: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	#endif

	palette = SDL_AllocPalette(MAX_NUM_PALETTE_ENTRIES);
	if (palette == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate palette: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	PaletteBpp = COLOR_DEPTH;
	#endif

	redrawEvent = SDL_RegisterEvents(1);
	if (redrawEvent == ((Uint32)-1)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Could not register user events: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	SDL_EventState(SDL_TEXTEDITING, SDL_IGNORE);
	SDL_EventState(SDL_TEXTINPUT, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEWHEEL, SDL_IGNORE);
	SDL_EventState(SDL_CLIPBOARDUPDATE, SDL_IGNORE);

	#ifndef USE_TOUCHSCREEN
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	#endif

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

void SetClipRgn(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
	_clipLeft = left;
	_clipTop = top;
	_clipRight = right;
	_clipBottom = bottom;

	if (_clipRgn) {
		SetClip(_clipRgn);
	}
}

void SetClip(BYTE control)
{
	_clipRgn = control;

	if (_clipRgn) {
		SDL_Rect rect;

		rect.x = _clipLeft;
		rect.y = _clipTop;
		rect.w = _clipRight - _clipLeft + 1;
		rect.h = _clipBottom - _clipTop + 1;

		SDL_RenderSetClipRect(renderer, &rect);
	} else {
		SDL_RenderSetClipRect(renderer, NULL);
	}
}

#ifdef USE_TRANSPARENT_COLOR
void TransparentColorEnable(GFX_COLOR color)
{
	_colorTransparent = color;
	_colorTransparentEnable = TRANSPARENT_COLOR_ENABLE;

}
#endif

#ifdef USE_PALETTE
void EnablePalette(void)
{
	#if (COLOR_DEPTH == 8)
	pixfmt = pixfmt_idx;
	#endif

	SDL_SetPixelFormatPalette(pixfmt, palette);
}

void DisablePalette(void)
{
	SDL_SetPixelFormatPalette(pixfmt, NULL);

	#if (COLOR_DEPTH == 8)
	// fall back to RGB instead of an empty palette, which would lead to a white screen
	pixfmt = pixfmt_rgb;
	#endif
}

BYTE IsPaletteEnabled(void)
{
	return (pixfmt->palette != NULL);
}

BYTE SetPaletteBpp(BYTE bpp)
{
	return (bpp != COLOR_DEPTH);
}

BYTE SetPaletteFlash(PALETTE_ENTRY *pPaletteEntry, WORD startEntry, WORD length)
{
	int i;
	SDL_PixelFormat *pe_pixfmt; // used to transform palette entries to RGB
	static SDL_Color colors[MAX_NUM_PALETTE_ENTRIES];

	if ((pPaletteEntry == NULL) || ((startEntry + length) > MAX_NUM_PALETTE_ENTRIES)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Palette NULL or entry overflow: %u (maximum %u)\n",
								startEntry + length, MAX_NUM_PALETTE_ENTRIES);
		return 1;
	}

	pe_pixfmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGB565);
	if (pe_pixfmt == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate pixel format: %s\n", SDL_GetError());
		return 1;
	}

	for (i = 0; i < length; i++)
		SDL_GetRGB(pPaletteEntry[i].value, pe_pixfmt, &(colors[i].r), &(colors[i].g), &(colors[i].b));

	SDL_FreeFormat(pe_pixfmt);

	return !!SDL_SetPaletteColors(palette, colors, startEntry, length);
}

void StartVBlankInterrupt(void)
{
	if (pPendingPalette != NULL) {
		blPaletteChangeError = SetPalette(pPendingPalette, PendingStartEntry, PendingLength);

		if (!blPaletteChangeError) {
			_palette = pPendingPalette;
		}
	}
}
#endif

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

	default:
		if (event->type == redrawEvent) {
			SDL_RenderPresent(renderer);
		} else {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled SDL event: 0x%x\n", event->type);
		}
		break;
	}
}
