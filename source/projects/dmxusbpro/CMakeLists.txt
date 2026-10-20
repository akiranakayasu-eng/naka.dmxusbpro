set_property(DIRECTORY PROPERTY max::external YES)
project(dmxusbpro C)

target_sources(dmxusbpro PRIVATE dmxusbpro.c)
target_link_libraries(dmxusbpro PRIVATE dmxusbpro_core)
