#include <Graphics/Graphics.h>

#if defined(USE_PALETTE_EXTERNAL) || defined(USE_FONT_EXTERNAL) || defined(USE_BITMAP_EXTERNAL)
extern const unsigned char externalMemoryData[];

WORD ExternalMemoryCallback(GFX_EXTDATA *memory, LONG offset, WORD nCount, void *buffer)
{
	DWORD address;
	WORD i;

	address = memory->address + offset;

	for (i = 0; i < nCount; i++) {
		*(BYTE *)buffer++ = externalMemoryData[address++];
	}

	return i;
}
#endif
