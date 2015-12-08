# http://francesco-cek.com/cmake-and-gtk-3-the-easy-way/

cmake_minimum_required(VERSION 2.8.6)
project(othello C)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources.h)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/resources.h)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources.c)
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/resources.c)
endif()

# képek, UI beágyazása
add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/resources.h
            ${CMAKE_BINARY_DIR}/resources.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND glib-compile-resources --target ${CMAKE_BINARY_DIR}/resources.h --generate  ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/resources
    COMMAND glib-compile-resources --target ${CMAKE_BINARY_DIR}/resources.c --generate  ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/resources
    COMMENT "building resources"
    VERBATIM
)



# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})


set(SOURCE_FILES
        graphics.c
        othello.c
        game.c
        ${CMAKE_BINARY_DIR}/resources.c)

add_executable(othello ${SOURCE_FILES})

# Link the target to the GTK+ libraries
target_link_libraries(othello ${GTK3_LIBRARIES})