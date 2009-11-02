INCLUDE(InstallHelpers)
CPACK_ADD_COMPONENT(Samples DISPLAY_NAME "Sample files" DESCRIPTION "A collection of some basic samples such as drumkits" GROUP "Data")

INSTALL_DATA_SUBDIRS("samples" "*.wav;*.ogg;*.ds" Samples)

