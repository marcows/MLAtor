#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"

#include "Demo.h"

#define ID_BUTTON 0

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

void CreateDemo(void)
{
	GOLFree();
	SetColor(GFX_SCHEMEDEFAULT.CommonBkColor);
	ClearDevice();

	BtnCreate(ID_BUTTON,
		GetMaxX() * 1 / 4,
		GetMaxY() * 1 / 3,
		GetMaxX() * 3 / 4,
		GetMaxY() * 2 / 3,
		0, BTN_DRAW, NULL, "Button", NULL);

	showDecoration = FALSE;
	showDecorationPrev = TRUE; // force redraw
}

WORD DemoMsgCallback(WORD objMsg, OBJ_HEADER *pObj, GOL_MSG *pMsg)
{
	if (GetObjID(pObj) == ID_BUTTON) {
		if (objMsg == BTN_MSG_PRESSED) {
			showDecoration = TRUE;
		} else if (objMsg == BTN_MSG_RELEASED || objMsg == BTN_MSG_CANCELPRESS) {
			showDecoration = FALSE;
		}
	}

	return 1;
}

WORD DemoDrawCallback(void)
{
	if (!DrawOrHideDecoration())
		return 0;

	return 1;
}
