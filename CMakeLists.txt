cmake_minimum_required(VERSION 3.24)
project(Szachy C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GTK4_LDFLAGS_OTHER}")

add_executable(Szachy main.c)

target_link_libraries(Szachy ${GTK4_LIBRARIES})