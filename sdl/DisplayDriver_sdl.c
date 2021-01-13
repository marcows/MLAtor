/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  SDL Display driver
 *****************************************************************************/

#include "DisplayDriver_sdl.h"

#include <stdio.h>
#include <time.h>
#include "Graphics/Graphics.h"

#ifndef MLATOR_WINDOW_TITLE
#define MLATOR_WINDOW_TITLE MLAtor
#endif

#ifndef MLATOR_SCALE_FACTOR_DEFAULT
#define MLATOR_SCALE_FACTOR_DEFAULT 1
#endif

#ifndef MLATOR_SCALE_FACTOR_MAX
#define MLATOR_SCALE_FACTOR_MAX 3
#endif

#define xstr(s) str(s)
#define str(s) #s

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

#ifdef USE_DOUBLE_BUFFERING
BYTE blInvalidateAll;
volatile BYTE blDisplayUpdatePending;
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

// window/render scaling
static Uint8 scaleFactor = MLATOR_SCALE_FACTOR_DEFAULT;

/* local helper functions */
static void Cleanup(void);
static void ActivateCurrentColor(void);
static void ScheduleScreenUpdate(void);
static SDL_Rect GetRectFromNativeData(DWORD offset, WORD width, WORD height);
static SDL_Rect GetApplicationRect(SDL_Rect nativeRect);

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

/*
 * Get the rectangle from HW display memory address offset and width/height.
 *
 * These three parameters always relate to the HW display with native
 * orientation (not rotated) because the display buffer on HW and its pitch
 * (for pixel arrangement) is not affected by the rotated application.
 *
 * DISP_ORIENTATION is not applied here, the resulting rectangle is oriented
 * like the native orientation of the HW display.
 */
static SDL_Rect GetRectFromNativeData(DWORD offset, WORD width, WORD height)
{
	SDL_Rect r;

	r.x = offset % DISP_HOR_RESOLUTION;
	r.y = offset / DISP_HOR_RESOLUTION;
	r.w = width;
	r.h = height;

	return r;
}

/*
 * Transform rectangle coordinates/dimensions from native orientation of the HW
 * display to orientation of the SW application window.
 *
 * The SW application content is rotated counterclockwise, the HW display
 * clockwise by DISP_ORIENTATION degrees. In below graphics the screen is
 * always drawn like the HW display with native orientation (not rotated).
 * The single letters A/B/C should also be rotated counterclockwise of course,
 * but that's not possible with simple ASCII.
 *
 * Symbols for origin (left/top) of the rectangle and screen:
 *   o: HW display
 *   x: SW application window
 *
 *     DISP_ORIENTATION=0          DISP_ORIENTATION=90        DISP_ORIENTATION=180        DISP_ORIENTATION=270
 * o------------------------+  o------------------------+  o------------------------+  o------------------------x
 * |                        |  |                        |  |                        |  |    +---x               |
 * |                        |  |                        |  |               o-----+  |  |    | A |               |
 * |                        |  |                        |  |               | CBA |  |  |    | B |               |
 * |                        |  |               o---+    |  |               +-----x  |  |    | C |               |
 * |  o-----+               |  |               | C |    |  |                        |  |    +---o               |
 * |  | ABC |               |  |               | B |    |  |                        |  |                        |
 * |  +-----+               |  |               | A |    |  |                        |  |                        |
 * |                        |  |               x---+    |  |                        |  |                        |
 * +------------------------+  x------------------------+  +------------------------x  +------------------------+
 */
static SDL_Rect GetApplicationRect(SDL_Rect nativeRect)
{
	SDL_Rect appRect;

	#if (DISP_ORIENTATION == 90)
	appRect.x = DISP_VER_RESOLUTION - nativeRect.y - nativeRect.h;
	appRect.y = nativeRect.x;
	#elif (DISP_ORIENTATION == 180)
	appRect.x = DISP_HOR_RESOLUTION - nativeRect.x - nativeRect.w;
	appRect.y = DISP_VER_RESOLUTION - nativeRect.y - nativeRect.h;
	#elif (DISP_ORIENTATION == 270)
	appRect.x = nativeRect.y;
	appRect.y = DISP_HOR_RESOLUTION - nativeRect.x - nativeRect.w;
	#else
	appRect.x = nativeRect.x;
	appRect.y = nativeRect.y;
	#endif

	#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
	appRect.w = nativeRect.h;
	appRect.h = nativeRect.w;
	#else
	appRect.w = nativeRect.w;
	appRect.h = nativeRect.h;
	#endif

	return appRect;
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
			xstr(MLATOR_WINDOW_TITLE),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			(GetMaxX() + 1) * scaleFactor, (GetMaxY() + 1) * scaleFactor,
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

	#ifndef MLATOR_EXTRAS
	SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	#endif
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

	if (SDL_RenderSetScale(renderer, scaleFactor, scaleFactor)) {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not scale rendering: %s\n", SDL_GetError());
		scaleFactor = 1;
		// TODO: change window size and redraw content
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
	SDL_Rect rect;
	Uint32 pixel, w_pixfmtVal;

	SDL_PixelFormat *w_pixfmt;
	Uint8 r, g, b;

	rect.x = x * scaleFactor;
	rect.y = y * scaleFactor;
	rect.w = 1;
	rect.h = 1;

	w_pixfmtVal = SDL_GetWindowPixelFormat(window);
	if (w_pixfmtVal == SDL_PIXELFORMAT_UNKNOWN) {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not determine window pixel format, fall back to \"RGB 888\": %s\n", SDL_GetError());
		w_pixfmtVal = SDL_PIXELFORMAT_RGB888;
	}

	if (SDL_RenderReadPixels(renderer, &rect, w_pixfmtVal, &pixel, rect.w * sizeof(pixel)) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not read pixel: %s\n", SDL_GetError());
		return 0;
	}

	w_pixfmt = SDL_AllocFormat(w_pixfmtVal);
	if (w_pixfmt == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate window pixel format: %s\n", SDL_GetError());
		return 0;
	}

	SDL_GetRGB(pixel, w_pixfmt, &r, &g, &b);
	SDL_FreeFormat(w_pixfmt);

	return SDL_MapRGB(pixfmt, r, g ,b);
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

#ifdef USE_DOUBLE_BUFFERING
void SwitchOnDoubleBuffering(void)
{
}

void SwitchOffDoubleBuffering(void)
{
}

void InvalidateRectangle(WORD left, WORD top, WORD right, WORD bottom)
{
}

void RequestDisplayUpdate(void)
{
}

void UpdateDisplayNow(void)
{
}
#endif

/* This function currently only works within a single display buffer, srcAddr
 * and dstAddr are unused. */
WORD CopyBlock(DWORD srcAddr, DWORD dstAddr, DWORD srcOffset, DWORD dstOffset, WORD width, WORD height)
{
	static Uint32 pixels[DISP_HOR_RESOLUTION * MLATOR_SCALE_FACTOR_MAX * DISP_VER_RESOLUTION * MLATOR_SCALE_FACTOR_MAX];
	SDL_Rect rect;
	Uint32 w_pixfmtVal;
	SDL_Texture *texture;

	/* set source block */

	rect = GetApplicationRect(GetRectFromNativeData(srcOffset, width, height));

	rect.x *= scaleFactor;
	rect.y *= scaleFactor;
	rect.w *= scaleFactor;
	rect.h *= scaleFactor;

	/* read pixel block from source into buffer */

	w_pixfmtVal = SDL_GetWindowPixelFormat(window);
	if (w_pixfmtVal == SDL_PIXELFORMAT_UNKNOWN) {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not determine window pixel format, fall back to \"RGB 888\": %s\n", SDL_GetError());
		w_pixfmtVal = SDL_PIXELFORMAT_RGB888;
	}

	if (SDL_RenderReadPixels(renderer, &rect, w_pixfmtVal, pixels, rect.w * sizeof(pixels[0])) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not read pixels: %s\n", SDL_GetError());
		return 1;
	}

	/* set destination block */

	rect = GetApplicationRect(GetRectFromNativeData(dstOffset, width, height));

	/* write pixel block from buffer into destination */

	texture = SDL_CreateTexture(renderer, w_pixfmtVal, SDL_TEXTUREACCESS_STREAMING, rect.w * scaleFactor, rect.h * scaleFactor);
	if (texture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create texture: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_UpdateTexture(texture, NULL, pixels, rect.w * scaleFactor * sizeof(pixels[0])) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not update texture: %s\n", SDL_GetError());
		SDL_DestroyTexture(texture);
		return 1;
	}

	if (SDL_RenderCopy(renderer, texture, NULL, &rect) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not copy the texture to the renderer: %s\n", SDL_GetError());
		SDL_DestroyTexture(texture);
		return 1;
	}

	SDL_DestroyTexture(texture);

	ScheduleScreenUpdate();
	return 1;
}

/* accelerated functions to avoid primitives using PutPixel() */

WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
	SDL_Rect rect;

	#ifdef USE_ALPHABLEND_LITE
	if (GetAlpha() != 100)
		return BarAlpha(left, top, right, bottom);
	#endif

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

	#ifdef MLATOR_EXTRAS
	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_F1) {
			MLAtor_ShowHelp();
		} else if (event->key.keysym.sym == SDLK_F2) {
			MLAtor_TakeScreenshot();
		}
		break;
	#endif

	default:
		if (event->type == redrawEvent) {
			SDL_RenderPresent(renderer);
		} else {
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled SDL event: 0x%x\n", event->type);
		}
		break;
	}
}

/* Extras */

#ifndef MLATOR_SCREENSHOT_PREFIX
#define MLATOR_SCREENSHOT_PREFIX MLAtor
#endif

void MLAtor_ShowHelp(void)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Help",
				"F1 - Show help\n"
				"F2 - Save screenshot\n"
				, NULL);
}

void MLAtor_TakeScreenshot(void)
{
	time_t now;
	struct tm *nowlocal;
	char nowstring[15 + 1];

	char sshotFilename[sizeof(xstr(MLATOR_SCREENSHOT_PREFIX)) + 1 + 15 + 4 + 1] = xstr(MLATOR_SCREENSHOT_PREFIX) "_";
	FILE *sshotFile;

	Uint32 w_pixfmtVal;
	SDL_PixelFormat *w_pixfmt;
	SDL_Surface *sshot, *sshotNormalized;

	/* create filename and abort if file already exists, format: <MLATOR_SCREENSHOT_PREFIX>_yyyymmddThhmmss.bmp */

	if (time(&now) == (time_t)-1) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not determine current time.\n");
		return;
	}

	nowlocal = localtime(&now);
	if (nowlocal == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not convert to local time.\n");
		return;
	}

	if (strftime(nowstring, 15 + 1, "%Y%m%dT%H%M%S", nowlocal) != (sizeof(nowstring) - 1)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not format the filename.\n");
		return;
	}

	strcat(sshotFilename, nowstring);
	strcat(sshotFilename, ".bmp");

	sshotFile = fopen(sshotFilename, "r");
	if (sshotFile) {
		fclose(sshotFile);
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "File already exists, only 1 screenshot per second possible.\n");
		return;
	}

	/* read pixels (possibly scaled) into SDL surface, normalize the dimensions and save to bitmap file */

	w_pixfmtVal = SDL_GetWindowPixelFormat(window);
	if (w_pixfmtVal == SDL_PIXELFORMAT_UNKNOWN) {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not determine window pixel format, fall back to \"RGB 888\": %s\n", SDL_GetError());
		w_pixfmtVal = SDL_PIXELFORMAT_RGB888;
	}

	w_pixfmt = SDL_AllocFormat(w_pixfmtVal);
	if (w_pixfmt == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not allocate window pixel format: %s\n", SDL_GetError());
		return;
	}

	sshot = SDL_CreateRGBSurface(0, (GetMaxX() + 1) * scaleFactor, (GetMaxY() + 1) * scaleFactor,
			w_pixfmt->BitsPerPixel, w_pixfmt->Rmask, w_pixfmt->Gmask, w_pixfmt->Bmask, w_pixfmt->Amask);
	if (sshot == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create screenshot surface: %s\n", SDL_GetError());
		SDL_FreeFormat(w_pixfmt);
		return;
	}

	if (SDL_RenderReadPixels(renderer, NULL, w_pixfmtVal, sshot->pixels, sshot->pitch)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not read screenshot pixels: %s\n", SDL_GetError());
		SDL_FreeSurface(sshot);
		SDL_FreeFormat(w_pixfmt);
		return;
	}

	sshotNormalized = SDL_CreateRGBSurface(0, GetMaxX() + 1, GetMaxY() + 1,
			w_pixfmt->BitsPerPixel, w_pixfmt->Rmask, w_pixfmt->Gmask, w_pixfmt->Bmask, w_pixfmt->Amask);
	if (sshotNormalized == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create screenshot surface for normalization: %s\n", SDL_GetError());
		SDL_FreeSurface(sshot);
		SDL_FreeFormat(w_pixfmt);
		return;
	}

	if (SDL_BlitScaled(sshot, NULL, sshotNormalized, NULL)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not normalize screenshot surface: %s\n", SDL_GetError());
		SDL_FreeSurface(sshotNormalized);
		SDL_FreeSurface(sshot);
		SDL_FreeFormat(w_pixfmt);
		return;
	}

	if (SDL_SaveBMP(sshotNormalized, sshotFilename)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not save screenshot to .bmp file: %s\n", SDL_GetError());
	}

	SDL_FreeSurface(sshotNormalized);
	SDL_FreeSurface(sshot);
	SDL_FreeFormat(w_pixfmt);
}

Uint8 MLAtor_GetScaleFactor(void)
{
	return scaleFactor;
}
