# Target
add_compile_definitions("CFXS_CPU_CLOCK_FREQUENCY=${CFXS_CORE_FREQUENCY}")

# Project
add_compile_definitions("CFXS_PROJECT_NAME=\"Rihards Veips RTU BD 2023\"")
add_compile_definitions("CFXS_PROJECT_VERSION_STRING=\"1.0\"")

add_compile_definitions(
    "SEGGER_DONT_REDEFINE_PRINTF" # custom printf lib, dont let segger redirect directly
    "SEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_NO_BLOCK_SKIP"
    # "SEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL"
    "CFXS_DEBUG_TERMINAL_USE_COLORS=1"

    # Task Groups
    "LOW_PRIORITY=0"
    "HIGH_PRIORITY=1"
    "SYSTEM_PRIORITY=2"
) 

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions("DEBUG")
    add_compile_definitions("CFXS_DEBUG")
    add_compile_definitions("CFXS_DEBUG_ASSERT")
    add_compile_definitions("EASTL_ASSERT_ENABLED=1")
endif()
    
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    add_compile_definitions("RELEASE")
    add_compile_definitions("NDEBUG")
    add_compile_definitions("CFXS_DEBUG_ASSERT") # CFXS_ASSERT/CFXS_ERROR
endif()