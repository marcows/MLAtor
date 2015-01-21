#ifndef _DEMO_PROJECTS_COMPAT_H
#define _DEMO_PROJECTS_COMPAT_H

// display layout, normally defined in HardwareProfile.h
#define DISP_ORIENTATION    0
#define DISP_HOR_RESOLUTION 320
#define DISP_VER_RESOLUTION 240

#if defined(TARGET_AppNotes)
  #define COMPAT_DUMMY_EVENT_POLL
  #define COMPAT_TICK_COUNTER

#elif defined(TARGET_ColorDepth)
  #define CFG_INCLUDE_MPLAB_X
  #define Screen1_1BPP_Palette_SIZE Screen_1BPP_Palette_SIZE
  #define Screen2_1BPP_Palette_SIZE Screen_1BPP_Palette_SIZE
  #ifdef EXTERNAL_MEMORY
    #define USE_PALETTE_EXTERNAL
    #define USE_FONT_EXTERNAL
    #define USE_BITMAP_EXTERNAL
  #endif
  #define COMPAT_TICK_COUNTER

#elif defined(TARGET_ExternalMemory)
  #define COMPAT_GENERAL_EVENTS_HOOK

#elif defined(TARGET_ObjectLayer)
  #define MEB_BOARD
  #define COMPAT_TICK_COUNTER
  #define USE_DOUBLE_BUFFERING

#elif defined(TARGET_ObjectLayerPalette)
  #define GFX_PICTAIL_LCC
  #define COMPAT_TICK_COUNTER

#elif defined(TARGET_PrimitiveLayer)
  #define COMPAT_GENERAL_EVENTS_HOOK
#endif

#endif
