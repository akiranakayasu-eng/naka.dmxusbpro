set_property(DIRECTORY PROPERTY max::external YES)
project(naka.artnet C)

target_sources(naka.artnet PRIVATE naka.artnet.c)
target_link_libraries(naka.artnet PRIVATE netdmx_core)
