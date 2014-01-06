#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"
#include <SDL.h>

#include "TouchScreen.h"

/* do dummy event handling, necessary for querying the input state */
static void TouchGetDummyMsg(void)
{
	GOL_MSG pDummyMsg;
	TouchGetMsg(&pDummyMsg);
}

BYTE GetHWButtonCR(void)
{
	TouchGetDummyMsg();
	return !(SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RETURN]
		|| SDL_GetKeyboardState(NULL)[SDL_SCANCODE_KP_ENTER]);
}

BYTE GetHWButtonFocus(void)
{
	TouchGetDummyMsg();
	return !SDL_GetKeyboardState(NULL)[SDL_SCANCODE_TAB];
}

/* this hook is invoked periodically at nonconstant intervals */
void PeriodicHook(void)
{
	extern DWORD tick;

	// tick counter
	tick = SDL_GetTicks() * 8;
}
