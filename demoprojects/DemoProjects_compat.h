#ifndef _DEMO_PROJECTS_COMPAT_H
#define _DEMO_PROJECTS_COMPAT_H

// display layout, normally defined in HardwareProfile.h
#define DISP_ORIENTATION    0
#define DISP_HOR_RESOLUTION 320
#define DISP_VER_RESOLUTION 240

#if defined(TARGET_AppNotes)
  #define COMPAT_DUMMY_EVENT_POLL

#elif defined(TARGET_ColorDepth)
  #define CFG_INCLUDE_MPLAB_X
  #define COLOR_DEPTH 8

#elif defined(TARGET_ObjectLayer)
  #define MEB_BOARD

#elif defined(TARGET_ObjectLayerPalette)
  #define GFX_PICTAIL_LCC
#endif

#endif
