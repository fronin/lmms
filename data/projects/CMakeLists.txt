INCLUDE(InstallHelpers)
CPACK_ADD_COMPONENT(Projects DISPLAY_NAME "Demo projects" DESCRIPTION "Some demo projects helping you to get started with LMMS" GROUP "Data")

INSTALL_DATA_SUBDIRS("projects" "*.mmp;*.mmpz" Projects)
#INSTALL_DATA_SUBDIRS("projects" "*.mmpz" Projects)

