#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Graphics/Graphics.h"

#include "InputDriver.h"

#include "Demo.h"

typedef enum
{
	CREATE_DEMO = 0,
	DISPLAY_DEMO,
} SCREEN_STATES;

SCREEN_STATES screenState = CREATE_DEMO;

int main(void)
{
	GOL_MSG msg;

	GOLInit();

	while (1) {
		if (GOLDraw()) {
			TouchGetMsg(&msg);
			GOLMsg(&msg);
		}
	}
}

WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER *pObj, GOL_MSG *pMsg)
{
	switch(screenState)
	{
	case DISPLAY_DEMO:
		return DemoMsgCallback(objMsg, pObj, pMsg);

	default:
		return 1;
	}
}

WORD GOLDrawCallback(void)
{
	switch(screenState)
	{
	case CREATE_DEMO:
		CreateDemo();
		screenState = DISPLAY_DEMO;
		return 1;

	case DISPLAY_DEMO:
		return DemoDrawCallback();

	default:
		return 1;
	}
}
