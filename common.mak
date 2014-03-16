MLA_INSTALL_PATH ?= /opt/microchip_solutions
GRC ?= java -jar $(MLA_INSTALL_PATH)/Microchip/Graphics/bin/grc/grc.jar
CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc

ifeq ($(OS),Windows_NT)
# Windows native using cmd.exe or MinGW Shell
SDL2_PREFIX ?= $(ProgramFiles)/SDL2/i686-w64-mingw32
SDL2_CFLAGS := -I"$(SDL2_PREFIX)/include/SDL2" -Dmain=SDL_main
SDL2_LDFLAGS := -L"$(SDL2_PREFIX)/bin" -L"$(SDL2_PREFIX)/lib" -lmingw32 -lSDL2main -lSDL2 -mwindows
X = .exe

else ifneq (,$(findstring mingw,$(CROSS_COMPILE)))
# Windows cross compiling
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(patsubst %/lib,%/bin,$(shell $(CROSS_COMPILE)sdl2-config --libs))
X = .exe

else
# Linux etc.
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --libs)
endif

# no file extension for executable by default
X ?=

PROGRAM := $(TARGET)$(X)

SOURCES += \
	../sdl/DisplayDriver_sdl.c \
	../sdl/InputDriver_sdl.c

CFLAGS += \
	-include MLA_compat.h \
	-Wall \
	-I. \
	-I"$(GENERATED_DIR)" \
	-I"../MLA_glue" \
	-I"../sdl" \
	-I"$(MLA_INSTALL_PATH)/Microchip/Include" \
	-I"$(MLA_INSTALL_PATH)/Board Support Package" \
	$(SDL2_CFLAGS)

LDFLAGS += \
	$(SDL2_LDFLAGS)

OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.o))
DEPS := $(OBJECTS:.o=.d)

Q :=

.PHONY: all clean distclean prepare prepare_project

all: $(PROGRAM)

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(Q)mkdir -p $(patsubst %/,%,$(dir $@))
	$(Q)$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(PROGRAM): $(OBJECTS)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(Q)-rm -f $(OBJECTS)
	$(Q)-rm -f $(DEPS)

distclean: clean
	$(Q)-rm -f $(PROGRAM)
	$(Q)-rm -f -R $(GENERATED_DIR) $(BUILD_DIR) sdl

prepare: prepare_project
# workarounds for unsupported weak attribute, e.g. when building for Windows:
# - rename unused weak function to avoid runtime problems due to missing invocation
# - remove weak attribute from remaining used functions to avoid linkage errors
	$(Q)sed -e 's/__attribute__((weak)) \(Bar\|SetClipRgn\|SetClip\)\>/UNUSED_\1/' \
		-e 's/__attribute__((weak))//' \
		$(GENERATED_DIR)/Primitive.c > $(GENERATED_DIR)/Primitive.c.tmp
	$(Q)mv $(GENERATED_DIR)/Primitive.c.tmp $(GENERATED_DIR)/Primitive.c
