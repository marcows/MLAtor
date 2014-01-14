MLA_INSTALL_PATH ?= /opt/microchip_solutions
CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc

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
	$(shell $(CROSS_COMPILE)sdl2-config --cflags)

LDFLAGS := \
	$(shell $(CROSS_COMPILE)sdl2-config --libs)

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
	$(Q)-rm -f $(OBJECTS)
	$(Q)-rm -f $(DEPS)

distclean: clean
	$(Q)-rm -f $(TARGET)
	$(Q)-rm -f -R generated build sdl
