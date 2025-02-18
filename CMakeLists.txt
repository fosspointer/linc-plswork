cmake_minimum_required(VERSION 3.16)
project(linc DESCRIPTION "Linc Is Not C" VERSION 0.7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB LINC_CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include/linc/*/*.hpp)

file(GLOB LINC_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*/*.cpp)

file(GLOB LINC_STD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/std/*.linc)
file(GLOB LINC_STD_SOURCES_ASM ${CMAKE_CURRENT_SOURCE_DIR}/std/*.asm)

add_library(linc_core STATIC ${LINC_CORE_HEADERS} ${LINC_CORE_SOURCES})
target_compile_options(linc_core PUBLIC
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_include_directories(linc_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include)

target_compile_definitions(linc_core PUBLIC
    $<$<PLATFORM_ID:Windows>:LINC_WINDOWS>
    $<$<PLATFORM_ID:Linux>:LINC_LINUX>
    $<$<CONFIG:Release>:LINC_RELEASE>
    $<$<CONFIG:MinSizeRel>:LINC_RELEASE>
    $<$<CONFIG:Debug>:LINC_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:LINC_DEBUG>
    LINC_VERSION=\"${PROJECT_VERSION}\"
)

target_compile_definitions(linc_core PUBLIC LINC_INSTALL_PATH="${CMAKE_INSTALL_PREFIX}")
target_precompile_headers(linc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include/linc/Include.hpp)

option(doxygen "Use doxygen to generate documentation" OFF)

find_package(Doxygen)
if(doxygen)
    set(docs_dir ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(doxyfile_out ${docs_dir}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile_out} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_out}
        WORKING_DIRECTORY ${docs_dir}
        COMMENT "Started generating documentation")
endif()

add_executable(lincenv ${CMAKE_CURRENT_SOURCE_DIR}/src/lincenv.cpp)
add_executable(lincc ${CMAKE_CURRENT_SOURCE_DIR}/src/lincc.cpp)
target_link_libraries(lincenv linc_core)
target_link_libraries(lincc linc_core)
include(tests/testing.cmake)

install(TARGETS lincenv lincc linctest DESTINATION bin)

install(FILES ${LINC_STD_SOURCES} DESTINATION include)
set(STD_OBJECT_FILES)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach(FILENAME IN LISTS LINC_STD_SOURCES_ASM)
        get_filename_component(BASE_FILENAME "${FILENAME}" NAME_WE)
        set(OBJECT_FILE "${CMAKE_BINARY_DIR}/${BASE_FILENAME}.o")
        add_custom_command(
            OUTPUT "${OBJECT_FILE}"
            COMMAND nasm -f elf64 -o "${OBJECT_FILE}" "${FILENAME}"
            DEPENDS "${FILENAME}"
            COMMENT "Compiling standard object file `${FILENAME}`"
        )
        list(APPEND STD_OBJECT_FILES "${OBJECT_FILE}")
    endforeach()

    set(STD_BINARY_NAME "${CMAKE_BINARY_DIR}/liblinc.so")
    add_custom_command(
        OUTPUT "${STD_BINARY_NAME}"
        COMMAND ld --no-undefined -fPIC -shared -o "${STD_BINARY_NAME}" ${STD_OBJECT_FILES}
        DEPENDS ${STD_OBJECT_FILES}
        COMMENT "Linking standard library to create `${STD_BINARY_NAME}`"
    )
    add_custom_target(build_std ALL DEPENDS "${STD_BINARY_NAME}")
    install(FILES "${STD_BINARY_NAME}" DESTINATION lib)
endif()