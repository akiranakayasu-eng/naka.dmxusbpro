set_property(DIRECTORY PROPERTY max::external YES)
project(naka.dmxusbpro C)

target_sources(naka.dmxusbpro PRIVATE naka.dmxusbpro.c)
target_link_libraries(naka.dmxusbpro PRIVATE dmxusbpro_core)
