MLA_INSTALL_PATH ?= /opt/microchip_solutions
CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc

ifeq ($(OS),Windows_NT)
# Windows native using cmd.exe or MinGW Shell
SDL2_PREFIX ?= $(ProgramFiles)/SDL2/i686-w64-mingw32
SDL2_CFLAGS := -I"$(SDL2_PREFIX)/include/SDL2" -Dmain=SDL_main
SDL2_LDFLAGS := -L"$(SDL2_PREFIX)/bin" -L"$(SDL2_PREFIX)/lib" -lmingw32 -lSDL2main -lSDL2 -mwindows

else ifneq (,$(findstring mingw,$(CROSS_COMPILE)))
# Windows cross compiling
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(patsubst %/lib,%/bin,$(shell $(CROSS_COMPILE)sdl2-config --libs))

else
# Linux etc.
SDL2_CFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --cflags)
SDL2_LDFLAGS := $(shell $(CROSS_COMPILE)sdl2-config --libs)
endif

SOURCES += \
	../sdl/DisplayDriver_sdl.c \
	../sdl/InputDriver_sdl.c

CFLAGS += \
	-include MLA_compat.h \
	-Wall \
	-I. \
	-I"generated" \
	-I"../MLA_glue" \
	-I"../sdl" \
	-I"$(MLA_INSTALL_PATH)/Microchip/Include" \
	-I"$(MLA_INSTALL_PATH)/Board Support Package" \
	$(SDL2_CFLAGS)

LDFLAGS := \
	$(SDL2_LDFLAGS)

OBJECTS := $(addprefix build/,$(SOURCES:.c=.o))
DEPS := $(OBJECTS:.o=.d)

Q :=

all: $(TARGET)

-include $(DEPS)

build/%.o: %.c
	$(Q)mkdir -p $(patsubst %/,%,$(dir $@))
	$(Q)$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(Q)-rm -f $(OBJECTS)
	$(Q)-rm -f $(DEPS)

distclean: clean
	$(Q)-rm -f $(TARGET)
	$(Q)-rm -f -R generated build sdl
