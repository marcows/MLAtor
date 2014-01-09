TARGET := example

SOURCES := \
	Main.c \
	Demo.c \
	$(wildcard generated/*.c)

CFLAGS := \


include ../common.mak

prepare_project:
	$(Q)mkdir -p generated
# unchanged
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Primitive.c"                 generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOL.c"                       generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLSchemeDefault.c"          generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/GOLFontDefault.c"            generated
	$(Q)cp "$(MLA_INSTALL_PATH)/Microchip/Graphics/Button.c"                    generated
