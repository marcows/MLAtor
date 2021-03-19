#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"
#include <SDL.h>

#include "TouchScreen.h"

char _time_str[16] = "                ";
char _date_str[16] = "                ";

#ifdef COMPAT_ADC_POTENTIOMETER
volatile SHORT adcPot;
#endif

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

#ifdef USE_TOUCHSCREEN
SHORT TouchGetX(void)
{
	int x, y;

	GOL_MSG pDummyMsg;
	TouchGetMsg(&pDummyMsg);

	if (SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK)
		return x;
	else
		return -1; // not pressed
}
#endif

/* this hook is invoked periodically at nonconstant intervals */
void PeriodicHook(void)
{
#ifdef COMPAT_TICK_COUNTER
	extern DWORD tick;

	// tick counter
	tick = SDL_GetTicks() * COMPAT_TICK_COUNTER_FACTOR;
#endif

#ifdef COMPAT_ADC_POTENTIOMETER
	adcPot = tick & 0x03FF; // 10-bit ADC
#endif
}
