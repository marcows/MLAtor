MLA_INSTALL_PATH ?= /opt/microchip_solutions

TARGET := example

SOURCES := \
	Main.c \
	Demo.c \
	../sdl/DisplayDriver_sdl.c \
	../sdl/InputDriver_sdl.c \
	$(wildcard generated/*.c)

CC := gcc

CFLAGS := \
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
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Primitive.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOL.c"                       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLSchemeDefault.c"          generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLFontDefault.c"            generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Button.c"                    generated
