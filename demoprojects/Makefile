MLA_INSTALL_PATH ?= /opt/microchip_solutions

TARGET := AppNotes

SOURCES := \
	DemoProjects_compat.c \
	../sdl/DisplayDriver_sdl.c \
	../sdl/InputDriver_sdl.c \
	$(wildcard generated/*.c)

CC := gcc

CFLAGS := \
	-include DemoProjects_compat.h \
	-include MLA_compat.h \
	-Wall \
	-I. \
	-I"generated" \
	-I"../MLA_glue" \
	-I"../sdl" \
	-I"$(MLA_INSTALL_PATH)/Microchip/Include" \
	-I"$(MLA_INSTALL_PATH)/Board Support Package" \
	$(shell sdl2-config --cflags)

LDFLAGS := \
	$(shell sdl2-config --libs)

OBJECTS := $(addprefix build/,$(SOURCES:.c=.o))
DEPS := $(OBJECTS:.o=.d)

Q :=

all: $(TARGET)

-include $(DEPS)

build/%.o: %.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(Q)-rm $(OBJECTS)
	$(Q)-rm $(DEPS)

distclean: clean
	$(Q)-rm $(TARGET)
	$(Q)-rm -R generated build sdl

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
