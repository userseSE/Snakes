cmake_minimum_required(VERSION 3.16.0)

project(test VERSION 0.1.0  LANGUAGES C CXX)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "${CMAKE_CURRENT_SOURCE_DIR}" FORCE)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()




aux_source_directory(${PROJECT_SOURCE_DIR}/src/ source_files)


add_executable(test   ${source_files})

target_include_directories(
    test
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/
 )
target_link_libraries(test PRIVATE)
install (TARGETS test DESTINATION bin)


