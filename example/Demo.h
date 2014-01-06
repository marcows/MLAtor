#ifndef _DEMO_H
#define _DEMO_H

#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"

void CreateDemo(void);
WORD DemoMsgCallback(WORD objMsg, OBJ_HEADER *pObj, GOL_MSG *pMsg);
WORD DemoDrawCallback(void);

#endif
