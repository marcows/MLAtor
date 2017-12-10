#include "InputDriver.h"
#include "DisplayDriver_sdl.h"

#include "Graphics/Graphics.h"
#include <SDL.h>

/* dummy function, to be implemented by the application if needed */
void __attribute__((weak)) PeriodicHook(void)
{
}

#ifdef USE_TOUCHSCREEN
void TouchGetMsg(GOL_MSG *pMsg)
{
	SDL_Event event;
	Uint8 scaleFactor;

	scaleFactor = MLAtor_GetScaleFactor();

	pMsg->type = TYPE_TOUCHSCREEN;
	pMsg->uiEvent = EVENT_INVALID;

	// Touch screen simulation with a mouse using the left button as finger
	// touch/release, there is no "hover".
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				pMsg->uiEvent = EVENT_PRESS;
				pMsg->param1 = event.button.x / scaleFactor;
				pMsg->param2 = event.button.y / scaleFactor;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				pMsg->uiEvent = EVENT_RELEASE;
				pMsg->param1 = event.button.x / scaleFactor;
				pMsg->param2 = event.button.y / scaleFactor;
			}
			break;

		case SDL_MOUSEMOTION:
			if (event.motion.state & SDL_BUTTON_LMASK) {
				pMsg->uiEvent = EVENT_MOVE;
				pMsg->param1 = event.motion.x / scaleFactor;
				pMsg->param2 = event.motion.y / scaleFactor;
			}
			break;

		default:
			HandleGeneralEvent(&event);
			break;
		}
	} else {
		int x, y; // temporary variables to avoid compile warning "incompatible pointer type"

		if (SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK) {
			pMsg->uiEvent = EVENT_STILLPRESS;
			pMsg->param1 = x / scaleFactor;
			pMsg->param2 = y / scaleFactor;
		}

		// do not eat 100% CPU
		SDL_Delay(10);
	}

	PeriodicHook();
}
#endif
