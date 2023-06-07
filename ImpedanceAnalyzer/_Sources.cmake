file(GLOB_RECURSE sources CONFIGURE_DEPENDS
  "src/*.c"
  "src/*.cc"
  "src/*.cpp"
  "src/*.cxx"
  "res/*.c"
  "res/*.cc"
  "res/*.cpp"
  "res/*.cxx"
)

if("${CFXS_PLATFORM}" STREQUAL "STM32")
    target_include_directories(CFXS_Platform PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}")
    file(GLOB_RECURSE stm_sources CONFIGURE_DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/gen/${CFXS_PART}/*.cxx"
    )
    list(APPEND sources ${stm_sources})
endif()

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)