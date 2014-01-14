TARGET := AppNotes

SOURCES := \
	DemoProjects_compat.c \
	$(wildcard generated/*.c)

CFLAGS := \
	-include DemoProjects_compat.h

include ../common.mak

prepare:
	$(Q)mkdir -p generated
# unchanged
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1136Demo.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1136Demo.h"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1136 Pictures XC32.h"       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1182Demo.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1182Demo.h"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1182 Pictures XC32.h"       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1227Demo.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1227Demo.h"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1246Demo.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1246Demo.h"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AppNotesDemoResources XC32.h" generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/GraphicsConfig.h"             generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/HardwareProfile.h"            generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/HelloWorldFonts.h"            generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/HelloWorldFonts XC32.h"       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Primitive.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOL.c"                       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLSchemeDefault.c"          generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLFontDefault.c"            generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Button.c"                    generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Slider.c"                    generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/StaticText.c"                generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/TextEntry.c"                 generated
# renamed
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1136 Pictures XC32.c"       generated/AN1136_Pictures_XC32.c
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AN1182 Pictures XC32.c"       generated/AN1182_Pictures_XC32.c
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AppNotesDemoResources XC32.c" generated/AppNotesDemoResources_XC32.c
	$(Q)cp "$(MLA_INSTALL_PATH)/Graphics/AppNotes/HelloWorldFonts XC32.c"       generated/HelloWorldFonts_XC32.c
# modified
	$(Q)cat "$(MLA_INSTALL_PATH)/Graphics/AppNotes/AppNotesDemo.c" \
		| awk '/_T3Interrupt/ { exit } { print }' \
		| sed 's/InitializeBoard()/GOLInit()/' \
		> generated/AppNotesDemo.c
