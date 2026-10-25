set_property(DIRECTORY PROPERTY max::external YES)
project(naka.miseru C)

target_sources(naka.miseru PRIVATE naka.miseru.c)
target_link_libraries(naka.miseru PRIVATE miseru_core)
