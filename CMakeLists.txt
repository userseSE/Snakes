cmake_minimum_required(VERSION 3.16.0)

project(test VERSION 0.1.0 LANGUAGES C CXX)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "${CMAKE_CURRENT_SOURCE_DIR}" FORCE)

include(CheckCXXCompilerFlag)
# CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)

# if(COMPILER_SUPPORTS_MARCH_NATIVE)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
# endif()

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/ source_files)
add_subdirectory(thirdparty/flecs EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/raylib EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/raylib-cpp EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/cppzmq EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/json EXCLUDE_FROM_ALL)
find_package(zstd CONFIG REQUIRED)

add_library(zip_hpp INTERFACE)
target_include_directories(zip_hpp INTERFACE thirdparty/bxzstr/include)
target_link_libraries(zip_hpp INTERFACE $<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>)

add_library(snake_mp STATIC ${source_files})

target_include_directories(
    snake_mp
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/
)
target_link_libraries(snake_mp PUBLIC raylib flecs::flecs_static raylib_cpp)
target_link_libraries(snake_mp PUBLIC cppzmq nlohmann_json::nlohmann_json zip_hpp )

add_executable(test_sp app/main.cpp)
add_executable(test_server app/server.cpp)
add_executable(test_client app/client.cpp)

target_link_libraries(test_sp snake_mp)
target_link_libraries(test_server snake_mp)
target_link_libraries(test_client snake_mp)
install(TARGETS test_sp DESTINATION bin)
install(TARGETS test_server DESTINATION bin)
install(TARGETS test_client DESTINATION bin)
