CROSS_COMPILE ?=

ifeq ($(OS),Windows_NT)
# Windows native using cmd.exe or MinGW Shell
MLA_INSTALL_PATH ?= C:/Program Files (x86)/Microchip/microchip_solutions_v2013-06-15
SDL2_PREFIX ?= $(ProgramFiles)/SDL2/i686-w64-mingw32
SDL2_CFLAGS := -I"$(SDL2_PREFIX)/include/SDL2" -Dmain=SDL_main
SDL2_LDFLAGS := -L"$(SDL2_PREFIX)/bin" -L"$(SDL2_PREFIX)/lib" -lmingw32 -lSDL2main -lSDL2 -mwindows
X = .exe

else ifneq (,$(findstring mingw,$(CROSS_COMPILE)))
# Windows cross compiling
MLA_INSTALL_PATH ?= /opt/microchip_solutions
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(patsubst %/lib,%/bin,$(shell $(CROSS_COMPILE)sdl2-config --libs))
X = .exe

else
# Linux etc.
MLA_INSTALL_PATH ?= /opt/microchip_solutions
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --libs)
endif

GRC ?= java -jar "$(MLA_INSTALL_PATH)/Microchip/Graphics/bin/grc/grc.jar"
CC := $(CROSS_COMPILE)gcc

# no file extension for executable by default
X ?=

PROGRAM := $(TARGET)$(X)

SOURCES += \
	MLA_glue/ExternalMemory.c \
	sdl/DisplayDriver_sdl.c \
	sdl/InputDriver_sdl.c

CFLAGS += \
	-include MLA_compat.h \
	-Wall \
	-g \
	-I. \
	-I"$(GENERATED_DIR)" \
	-I"../MLA_glue" \
	-I"../sdl" \
	-I"$(MLA_INSTALL_PATH)/Microchip/Include" \
	-I"$(MLA_INSTALL_PATH)/Microchip/Include/Graphics" \
	-I"$(MLA_INSTALL_PATH)/Board Support Package" \
	-DMLATOR_EXTRAS \
	-DMLATOR_SCREENSHOT_PREFIX="$(TARGET)" \
	$(SDL2_CFLAGS)

LDFLAGS += \
	$(SDL2_LDFLAGS)

OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.o))
DEPS := $(OBJECTS:.o=.d)

Q :=

.PHONY: all clean distclean prepare prepare_project

all: $(PROGRAM)

-include $(DEPS)

# to find the common SOURCES
vpath %.c ..

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
	$(Q)-rm -f -R $(GENERATED_DIR) $(BUILD_DIR)

prepare: prepare_project
# workarounds for unsupported weak attribute, e.g. when building for Windows:
# - rename unused weak function to avoid runtime problems due to missing invocation
# - remove weak attribute from remaining used functions to avoid linkage errors
	$(Q)sed -e 's/__attribute__((weak)) \(Bar\|SetClipRgn\|SetClip\)\>/UNUSED_\1/' \
		-e 's/__attribute__((weak))//' \
		"$(MLA_INSTALL_PATH)/Microchip/Graphics/Primitive.c" \
		> $(GENERATED_DIR)/Primitive.c
# fix 32-bit integer typedefs, the used "signed long" and "unsigned long" base types may take 64 bits
	$(Q)sed -e '/#include/a #include <stdint.h>' \
		-e 's/\(typedef\).*\<\(INT32\|LONG\)\>/\1 int32_t \2/' \
		-e 's/\(typedef\).*\<\(UINT32\|DWORD\)\>/\1 uint32_t \2/' \
		"$(MLA_INSTALL_PATH)/Microchip/Include/GenericTypeDefs.h" \
		> $(GENERATED_DIR)/GenericTypeDefs.h
# Fix wrong inline definition, causes link errors with USE_DOUBLE_BUFFERING.
# GraphicsConfig.h needs to be copied to avoid including the faulty
# DisplayDriver.h from MLA installation in the same directory
	$(Q)mkdir -p $(GENERATED_DIR)/Graphics
	$(Q)sed -e 's/extern inline/static inline/' \
		"$(MLA_INSTALL_PATH)/Microchip/Include/Graphics/DisplayDriver.h" \
		> $(GENERATED_DIR)/Graphics/DisplayDriver.h
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Include/Graphics/Graphics.h" $(GENERATED_DIR)/Graphics
