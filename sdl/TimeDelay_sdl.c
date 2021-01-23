#include "GenericTypeDefs.h"
#include <SDL.h>

#ifdef COMPAT_GENERAL_EVENTS_HOOK
#include "DisplayDriver_sdl.h"
#endif

void DelayMs(UINT16 ms)
{
	Uint32 timeout;

	#ifdef COMPAT_GENERAL_EVENTS_HOOK
	SDL_Event event;
	#endif

	// Already set the timeout here, then the duration of the event polling
	// is not added additionally.
	timeout = SDL_GetTicks() + ms;

	#ifdef COMPAT_GENERAL_EVENTS_HOOK
	while (SDL_PollEvent(&event))
		HandleGeneralEvent(&event);
	#endif

	// perform the delay
	if (ms == 0) {
		// no delay
	} else if (ms <= 30) {
		// active waiting to improve accuracy with short delays
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) { }
	} else {
		// passive waiting, inaccuracy is system dependent
		SDL_Delay(ms);
	}
}
