#
# Copyright (C) 2023  Come Raczy
#
# This file is part of gcfrct:
# Guided Colimation For Ritchey-Chretien Telescopes.
#
# Gcfrct is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Gcfrct is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with gcfrct. If not, see <https://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.18)
enable_testing()
project(gcfrct LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # force C++17 on each target
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
set(GRESOURCE_XML ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml)
set(GRESOURCE_C ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp)
file(GLOB UI_SOURCES CONFIGURE_DEPENDS ui/*.ui)
add_custom_command(
    OUTPUT ${GRESOURCE_C}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS --target=${GRESOURCE_C} --generate-source ${GRESOURCE_XML}
    DEPENDS ${GRESOURCE_XML} ${UI_SOURCES}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
)

find_program(GLIB_COMPILE_SCHEMAS NAMES glib-compile-schemas REQUIRED)
set(GSCHEMA_XML ${CMAKE_CURRENT_SOURCE_DIR}/gcfrct.gschema.xml)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gschema.compiled
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_SCHEMAS}
    ARGS --strict --targetdir=${CMAKE_CURRENT_BINARY_DIR} --schema-file=${GSCHEMA_XML}
    DEPENDS ${GSCHEMA_XML}
    VERBATIM
    MAIN_DEPENDENCY ${GSCHEMA_XML}
)
add_custom_target(gcfrct_gschema ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gschema.compiled)

add_subdirectory(src)

add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}.cpp ${GRESOURCE_C}  $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}_OBJECTS>)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${${CMAKE_PROJECT_NAME}_SRC_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE PkgConfig::GTK4 ${GTKMM_LIBRARIES})

