#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"
#include "TimeDelay.h"

#ifdef USE_TRANSITION_EFFECTS
#include "Graphics/Transitions.h"
#endif

#include "Demo.h"

#define ID_BUTTON 0

static BYTE drawDelayCnt;
static BOOL showDecoration, showDecorationPrev;

static WORD DrawOrHideDecoration(void)
{
	static int drawState;

	OBJ_HEADER *pObj;
	SHORT l, t, r, b;

	if (showDecoration != showDecorationPrev) {
		showDecorationPrev = showDecoration;
		drawState = 0;
	}

	switch (drawState) {
	case 0:
		// prepare style
		SetLineThickness(THICK_LINE);
		SetLineType(DASHED_LINE);

		if (showDecoration)
			SetColor(GFX_SCHEMEDEFAULT.Color1);
		else
			SetColor(GFX_SCHEMEDEFAULT.CommonBkColor);

		drawState++;
		/* fall through */

	case 1:
		// draw/hide decoration
		pObj = GOLFindObject(ID_BUTTON);
		if (pObj == NULL)
			break;

		l = pObj->left - 5;
		t = pObj->top - 5;
		r = pObj->right + 5;
		b = pObj->bottom + 5;

		if (!Rectangle(l, t, r, b))
			return 0;

		drawState++;
		/* fall through */

	default:
		break;
	}

	return 1;
}

static WORD CopyButtonText(void)
{
	OBJ_HEADER *pObj;

	DWORD srcOffset, dstOffset;

	WORD appSrcX, appSrcY, appDstX, appDstY, appWidth, appHeight;
	WORD srcX, srcY, dstX, dstY, width, height;

	pObj = GOLFindObject(ID_BUTTON);
	if (pObj == NULL)
		return 1;

	/* Set size and position with orientation of the SW application. */

	// block to be copied, contains text "Button" from the button widget
	appWidth = 2 * 34;
	appHeight = 2 * 9;
	appSrcX = pObj->left + (pObj->right - pObj->left + 1) / 2 - 34;
	appSrcY = pObj->top + (pObj->bottom - pObj->top + 1) / 2 - 9;

	// destination block moved down with 1px space to the bottom
	appDstX = appSrcX;
	appDstY = GetMaxY() - appHeight;

	/* Transform size and position to native orientation of the HW display.
	 * Formulas adopted from GetApplicationRect() in DisplayDriver_sdl.c.
	 * See also CopyWindow() in MLA/Microchip/Graphics/Drivers/mchpGfxDrv.c */

	#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
	width = appHeight;
	height = appWidth;
	#else
	width = appWidth;
	height = appHeight;
	#endif

	#if (DISP_ORIENTATION == 90)
	srcX = appSrcY;
	srcY = DISP_VER_RESOLUTION - appSrcX - height;
	dstX = appDstY;
	dstY = DISP_VER_RESOLUTION - appDstX - height;
	#elif (DISP_ORIENTATION == 180)
	srcX = DISP_HOR_RESOLUTION - appSrcX - width;
	srcY = DISP_VER_RESOLUTION - appSrcY - height;
	dstX = DISP_HOR_RESOLUTION - appDstX - width;
	dstY = DISP_VER_RESOLUTION - appDstY - height;
	#elif (DISP_ORIENTATION == 270)
	srcX = DISP_HOR_RESOLUTION - appSrcY - width;
	srcY = appSrcX;
	dstX = DISP_HOR_RESOLUTION - appDstY - width;
	dstY = appDstX;
	#else
	srcX = appSrcX;
	srcY = appSrcY;
	dstX = appDstX;
	dstY = appDstY;
	#endif

	srcOffset = (DWORD)(srcY * (DWORD)DISP_HOR_RESOLUTION) + srcX;
	dstOffset = (DWORD)(dstY * (DWORD)DISP_HOR_RESOLUTION) + dstX;

	if (!CopyBlock(GetDrawBufferAddress(), GetDrawBufferAddress(), srcOffset, dstOffset, width, height))
		return 0;

	#ifdef USE_DOUBLE_BUFFERING
	InvalidateRectangle(appDstX, appDstY, appDstX + appWidth - 1, appDstY + appHeight - 1);
	RequestDisplayUpdate();
	#endif

	return 1;
}

void CreateDemo(void)
{
	GOLFree();
	SetColor(GFX_SCHEMEDEFAULT.CommonBkColor);
	ClearDevice();

	BtnCreate(ID_BUTTON,
		GetMaxX() * 1 / 8 + 1,
		GetMaxY() * 1 / 6 + 1,
		GetMaxX() * 5 / 8,
		GetMaxY() * 3 / 6,
		0, BTN_DRAW, NULL, "Button", NULL);

	drawDelayCnt = 0;
	showDecoration = FALSE;
	showDecorationPrev = TRUE; // force redraw
}

WORD DemoMsgCallback(WORD objMsg, OBJ_HEADER *pObj, GOL_MSG *pMsg)
{
	if (GetObjID(pObj) == ID_BUTTON) {
		if (objMsg == BTN_MSG_PRESSED) {
			showDecoration = TRUE;

			#ifdef USE_TRANSITION_EFFECTS
			// Expand the pressed button with the surrounding
			// decoration.
			GFXSetupTransition(
					pObj->left - 5 -1,
					pObj->top - 5 -1,
					pObj->right + 5 +1,
					pObj->bottom + 5 +1,
					EXPANDING_LINE,
					3 /* ms */,
					1 /* px */,
					HORIZONTAL);
			#endif
		} else if (objMsg == BTN_MSG_RELEASED || objMsg == BTN_MSG_CANCELPRESS) {
			showDecoration = FALSE;

			#ifdef USE_TRANSITION_EFFECTS
			// Push away the pressed button, only the inner face
			// without its 3D frame and without the decoration.
			GFXSetupTransition(
					pObj->left + GOL_EMBOSS_SIZE,
					pObj->top + GOL_EMBOSS_SIZE,
					pObj->right - GOL_EMBOSS_SIZE,
					pObj->bottom - GOL_EMBOSS_SIZE,
					PUSH,
					2 /* ms */,
					2 /* px */,
					TOP_TO_BOTTOM);
			#endif
		}
	}

	return 1;
}

WORD DemoDrawCallback(void)
{
	if (drawDelayCnt < 20) {
		drawDelayCnt++;
		DelayMs(50);
	} else if (drawDelayCnt == 20) {
		if (!CopyButtonText())
			return 0;
		drawDelayCnt++;
	}

	if (!DrawOrHideDecoration())
		return 0;

	return 1;
}
