# Copyright (c) 2019, QuantStack and Mamba Contributors
#
# Distributed under the terms of the BSD 3-Clause License.
#
# The full license is in the file LICENSE, distributed with this software.

cmake_minimum_required (VERSION 2.8.11)

project(monstructor)

set(MONSTRUCTOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MONSTRUCTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

set(MONSTRUCTOR_SOURCES
    ${MONSTRUCTOR_SOURCE_DIR}/installer.cpp
)

if (WIN32)
	include_directories($ENV{CONDA_PREFIX}/Library/include)
endif()

add_executable(monstructor ${MONSTRUCTOR_SOURCES})
set_property(TARGET monstructor PROPERTY CXX_STANDARD 17)
if (WIN32)
	find_library(MAMBA_LIBRARY NAMES libmamba_static)
	target_link_libraries(monstructor ${MAMBA_LIBRARY})
else()
	target_link_libraries(monstructor mamba)
endif()
target_link_libraries(monstructor nlohmann_json)

find_library(LIBSOLV_LIBRARIES NAMES solv)
find_library(LIBSOLVEXT_LIBRARIES NAMES solvext)
find_package(CURL REQUIRED)
find_package(LibArchive REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(monstructor
    ${LIBSOLV_LIBRARIES}
    ${LIBSOLVEXT_LIBRARIES}
    ${LibArchive_LIBRARIES}
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${YAML_CPP_LIBRARIES}
    Threads::Threads
)
