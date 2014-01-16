#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"
#include <SDL.h>

#include "TouchScreen.h"

char _time_str[16] = "                ";
char _date_str[16] = "                ";

/* do dummy event handling, necessary for querying the input state */
static void TouchGetDummyMsg(void)
{
#ifdef COMPAT_DUMMY_EVENT_POLL
	GOL_MSG pDummyMsg;
	TouchGetMsg(&pDummyMsg);
#endif
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

BYTE GetHWButtonScanDown(void)
{
	TouchGetDummyMsg();
	return !SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN];
}

BYTE GetHWButtonScanUp(void)
{
	TouchGetDummyMsg();
	return !SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP];
}

SHORT TouchGetX(void)
{
	return 0; // -1 if not pressed
}

/* this hook is invoked periodically at nonconstant intervals */
void PeriodicHook(void)
{
	extern DWORD tick;

	// tick counter
	tick = SDL_GetTicks() * 8;
}
