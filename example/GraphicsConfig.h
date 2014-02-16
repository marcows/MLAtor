#ifndef _GRAPHICSCONFIG_H
#define _GRAPHICSCONFIG_H

#define USE_NONBLOCKING_CONFIG

//#define USE_PALETTE

#ifdef USE_PALETTE
//#define USE_PALETTE_EXTERNAL
#endif

//#define USE_FOCUS

#define USE_TOUCHSCREEN
//#define USE_KEYBOARD

#define USE_GOL

#define USE_BUTTON
//#define USE_WINDOW
//#define USE_CHECKBOX
//#define USE_RADIOBUTTON
//#define USE_EDITBOX
//#define USE_LISTBOX
//#define USE_SLIDER
//#define USE_PROGRESSBAR
//#define USE_STATICTEXT
//#define USE_PICTURE
//#define USE_GROUPBOX
//#define USE_ROUNDDIAL
//#define USE_METER
//#define USE_TEXTENTRY
//#define USE_CUSTOM

//#define USE_MULTIBYTECHAR

#define USE_FONT_FLASH
//#define USE_FONT_EXTERNAL

//#define USE_BITMAP_FLASH
//#define USE_BITMAP_EXTERNAL

#define GFX_malloc(size) malloc(size)
#define GFX_free(pObj)   free(pObj)

//#define COLOR_DEPTH 8
#define COLOR_DEPTH 16
//#define COLOR_DEPTH 24

#endif
