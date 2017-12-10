/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  SDL Display driver
 *****************************************************************************/

#ifndef _DISPLAY_DRIVER_SDL_H
#define _DISPLAY_DRIVER_SDL_H

#include "HardwareProfile.h"

#include <SDL.h>

void HandleGeneralEvent(SDL_Event *event);
void MLAtor_ShowHelp(void);
void MLAtor_TakeScreenshot(void);
Uint8 MLAtor_GetScaleFactor(void);

#endif
