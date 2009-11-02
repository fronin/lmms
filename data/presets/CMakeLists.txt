INCLUDE(InstallHelpers)
CPACK_ADD_COMPONENT(Presets DISPLAY_NAME "Plugin presets" DESCRIPTION "A collection of some useful presets for LMMS' plugins" GROUP "Data")
INSTALL_DATA_SUBDIRS("presets" "*.xpf" Presets)
INSTALL_DATA_SUBDIRS("presets" "*.xiz" ZynAddSubFx)

