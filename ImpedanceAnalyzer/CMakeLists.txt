include("${PROJECT_SOURCE_DIR}/CMake/CFXS.cmake")

execute_process(COMMAND lua "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Scripts/LinkerGen/LinkerGen.lua"
                            "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Scripts/LinkerGen"
                            "${CMAKE_CURRENT_SOURCE_DIR}/cfg/${LINKERSCRIPT}.lua"
                            "${CMAKE_CURRENT_SOURCE_DIR}/cfg/LinkerScript.ld")

set(LINKER_SCRIPT "\"${CMAKE_CURRENT_SOURCE_DIR}/cfg/LinkerScript.ld\"")

add_compile_options(
    ${CPU_OPTIONS}
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-old-style-cast>

    -fsingle-precision-constant # 2.3/2.3f are both floats
    # -Werror=double-promotion

    -Wl,--gc-sections # remove unused sections
    -Wl,--print-gc-sections

    #-Wstack-usage=x
    #-fstack-usage

    -Wall
    -Wextra
    -Wcast-align
    -Wsign-conversion
    -Wshadow
    -Wlogical-op
    -Wsuggest-final-types
    -Wsuggest-final-methods
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-sign-conversion
    -Wno-narrowing
    -Wno-sign-compare
    -Wno-shift-negative-value
    -Wno-cast-align
    -Wno-write-strings
    -Wno-tautological-compare
    -Wno-unused-variable

    $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    
    #$<$<CONFIG:Release>:-flto=auto>
)

add_executable(${PROJECT_NAME})

include("_IncludeDirectories.cmake")
include("_Defines.cmake")
include("_Modules.cmake")
include("_Sources.cmake")

target_sources(${PROJECT_NAME} PRIVATE ${sources})

target_precompile_headers(
    ${PROJECT_NAME}
    PUBLIC
    <stddef.h>
    <stdint.h>
    <stdio.h>
    <stdlib.h>
    <stdbool.h>
    <string.h>
    <new>
    <Compiler.hpp>
)

target_link_options(
    ${PROJECT_NAME}
    PRIVATE
    ${CPU_OPTIONS}
    SHELL:-T${LINKER_SCRIPT}
    -Wl,-Map=output.map,--cref
    
    #$<$<CONFIG:Release>:-flto>
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${OBJCOPY} -S -O binary ${CMAKE_CURRENT_SOURCE_DIR}/../build/${PROJECT_NAME}/${PROJECT_NAME}.elf ${CMAKE_CURRENT_SOURCE_DIR}/../build/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${NM} -C -l --print-size --size-sort --format=bsd --radix=x ${CMAKE_CURRENT_SOURCE_DIR}/../build/${PROJECT_NAME}/${PROJECT_NAME}.elf > ${CMAKE_CURRENT_SOURCE_DIR}/../build/${PROJECT_NAME}_Symbols.txt)