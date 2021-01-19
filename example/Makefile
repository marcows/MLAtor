TARGET := example

# only double buffering
#USE_DOUBLE_BUFFERING = x
# transition effects with double buffering
USE_TRANSITION_EFFECTS = x

GENERATED_DIR = generated
BUILD_DIR = build

SOURCES := \
	Main.c \
	Demo.c \
	$(wildcard $(GENERATED_DIR)/*.c)

CFLAGS := \
	-DMLATOR_WINDOW_TITLE="MLAtor - Example"

ifdef USE_TRANSITION_EFFECTS
CFLAGS += \
	-DUSE_DOUBLE_BUFFERING \
	-DUSE_TRANSITION_EFFECTS
else ifdef USE_DOUBLE_BUFFERING
CFLAGS += -DUSE_DOUBLE_BUFFERING
else
# dummy macro definition for non-existent function
CFLAGS += -D"GetDrawBufferAddress()=0"
endif

LDFLAGS := \

include ../common.mak

prepare_project:
	$(Q)mkdir -p $(GENERATED_DIR)
# unchanged
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOL.c"                       $(GENERATED_DIR)
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLSchemeDefault.c"          $(GENERATED_DIR)
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLFontDefault.c"            $(GENERATED_DIR)
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Button.c"                    $(GENERATED_DIR)
ifdef USE_TRANSITION_EFFECTS
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Transitions.c"               $(GENERATED_DIR)
#modified
	$(Q)sed -e 's/__attribute__((weak))//' \
		"$(MLA_INSTALL_PATH)/Microchip/Graphics/Transitions_weak.c"         > $(GENERATED_DIR)/Transitions_weak.c
endif
