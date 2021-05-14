cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(vcsub)

add_executable(${PROJECT_NAME} src/events.c src/actors.c src/services.c src/utils.c src/chunks.c src/indexed_chunk.c
  src/example/main.c
  src/example/services/transform.c
  src/example/services/renderer.c
  src/example/actors/player.c
  src/example/actors/missile.c
  )
include_directories(${PROJECT_NAME} include/)

#sdl2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
find_package(sdl2-image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2_image)
