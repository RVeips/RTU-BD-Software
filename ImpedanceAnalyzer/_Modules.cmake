include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# CFXS-Base - Types and Math
FetchContent_Declare(lib_CFXS_Base GIT_REPOSITORY https://github.com/CFXS/CFXS-Base.git GIT_SHALLOW TRUE GIT_TAG master)
FetchContent_MakeAvailable(lib_CFXS_Base)
target_link_libraries(CFXS_Base PUBLIC CFXS_Toolchain)
target_link_libraries(${PROJECT_NAME} PUBLIC CFXS_Base)
target_precompile_headers(CFXS_Base PUBLIC <Compiler.hpp>)

# CFXS-Platform - System Startup and Utility Functions
FetchContent_Declare(lib_CFXS_Platform GIT_REPOSITORY https://github.com/CFXS/CFXS-Platform.git GIT_SHALLOW TRUE GIT_TAG master)
FetchContent_MakeAvailable(lib_CFXS_Platform)
target_include_directories(CFXS_Platform PUBLIC "${lib_cfxs_base_SOURCE_DIR}/include")
target_link_libraries(CFXS_Platform PUBLIC CFXS_Toolchain)
target_link_libraries(${PROJECT_NAME} PUBLIC CFXS_Platform)
target_precompile_headers(CFXS_Platform PUBLIC <Compiler.hpp>)

# CFXS-HW - Hardware Abstraction and Drivers
FetchContent_Declare(lib_CFXS_Hardware GIT_REPOSITORY https://github.com/CFXS/CFXS-HW.git GIT_SHALLOW TRUE GIT_TAG master)
FetchContent_MakeAvailable(lib_CFXS_Hardware)
target_include_directories(CFXS_HW PUBLIC "${lib_cfxs_base_SOURCE_DIR}/include")
target_include_directories(CFXS_HW PUBLIC "${lib_cfxs_platform_SOURCE_DIR}/include")
target_link_libraries(CFXS_HW PUBLIC CFXS_Toolchain)
target_link_libraries(${PROJECT_NAME} PUBLIC CFXS_HW)
target_precompile_headers(CFXS_HW PUBLIC <Compiler.hpp>)

# SeggerRTT - Terminal Output
FetchContent_Declare(lib_printf_impl_SeggerRTT GIT_REPOSITORY https://github.com/CFXS/SeggerRTT-printf.git GIT_SHALLOW TRUE GIT_TAG master)
FetchContent_MakeAvailable(lib_printf_impl_SeggerRTT)
target_link_libraries(${PROJECT_NAME} PUBLIC printf_impl_SeggerRTT)

# STM32 Generated Files
target_include_directories(CFXS_Platform PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}")
