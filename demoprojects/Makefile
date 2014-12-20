.PHONY: all clean distclean prepare

all:
	@echo -- Target all is currently unsupported with the single Makefile because of the
	@echo -- shared sdl build directory. You have to build each demo project separately:
	@echo $(MAKE) -f Makefile_... all

clean distclean prepare:
	$(MAKE) -f Makefile_AppNotes $@
	$(MAKE) -f Makefile_ColorDepth $@
	$(MAKE) -f Makefile_ExternalMemory $@
	$(MAKE) -f Makefile_ObjectLayer $@
	$(MAKE) -f Makefile_ObjectLayerPalette $@
	$(MAKE) -f Makefile_PrimitiveLayer $@
