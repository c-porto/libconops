cmake_minimum_required(VERSION 3.10)
project(libconops VERSION 0.2.2 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -Wshadow -Wcast-align -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-value -Wno-missing-field-initializers -Wno-narrowing -Wno-pointer-arith")

set(SRC_FILES ${CMAKE_SOURCE_DIR}/src/conops.c ${CMAKE_SOURCE_DIR}/src/util/cmd_queue.c)
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

enable_testing()

# Static library
add_library(conops_static STATIC ${SRC_FILES})
set_target_properties(conops_static PROPERTIES OUTPUT_NAME conops)

# Shared library
add_library(conops SHARED ${SRC_FILES})
set_target_properties(conops PROPERTIES VERSION ${PROJECT_VERSION})

# Build tests
add_subdirectory(test)
