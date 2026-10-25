cmake_minimum_required(VERSION 3.25)

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS architecture")
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
# Max externals use the static CRT on Windows; keep core and tests consistent
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(DMXUSBPRO_BUILD_EXTERNAL "Build the Max external (needs max-sdk-base)" ON)
option(DMXUSBPRO_BUILD_TESTS "Build protocol unit tests" ON)

if(DMXUSBPRO_BUILD_EXTERNAL)
  include(FetchContent)
  FetchContent_Declare(
    max-sdk-base
    GIT_REPOSITORY https://github.com/Cycling74/max-sdk-base.git
    GIT_TAG c03a2922a2a8ff149165a1e2ea134e9321d6e202
  )
  FetchContent_MakeAvailable(max-sdk-base)
endif()

project(dmxusbpro-max C)

# Portable protocol + serial code, shared by the external and the tests
add_library(dmxusbpro_core STATIC
  source/core/dmxusbpro_protocol.c
  source/core/serial_port.c
  source/core/dmxusbpro_device.c)
target_include_directories(dmxusbpro_core PUBLIC source/core)
set_target_properties(dmxusbpro_core PROPERTIES POSITION_INDEPENDENT_CODE ON C_STANDARD 99)

# Art-Net / sACN packets and UDP sockets, used by naka.artnet
add_library(netdmx_core STATIC
  source/core/netdmx_protocol.c
  source/core/udp_socket.c)
target_include_directories(netdmx_core PUBLIC source/core)
set_target_properties(netdmx_core PROPERTIES POSITION_INDEPENDENT_CODE ON C_STANDARD 99)
if(WIN32)
  target_link_libraries(netdmx_core PUBLIC ws2_32 iphlpapi)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # getaddrinfo / inet_pton need POSIX declarations under -std=c99
  target_compile_definitions(netdmx_core PRIVATE _DEFAULT_SOURCE)
endif()

if(DMXUSBPRO_BUILD_EXTERNAL)
  add_subdirectory(source/projects/naka.dmxusbpro)
  add_subdirectory(source/projects/naka.artnet)
endif()

if(DMXUSBPRO_BUILD_TESTS)
  enable_testing()
  add_executable(test_protocol tests/test_protocol.c)
  target_link_libraries(test_protocol PRIVATE dmxusbpro_core)
  add_test(NAME protocol COMMAND test_protocol)

  find_package(Threads REQUIRED)
  add_executable(test_device tests/test_device.c)
  target_link_libraries(test_device PRIVATE dmxusbpro_core Threads::Threads)
  add_test(NAME device COMMAND test_device)

  add_executable(test_netdmx tests/test_netdmx.c)
  target_link_libraries(test_netdmx PRIVATE netdmx_core)
  add_test(NAME netdmx COMMAND test_netdmx)
endif()
