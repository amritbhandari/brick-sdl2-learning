cmake_minimum_required(VERSION 4.0)
project(brick_sdl2 LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
        src/main.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(brick_sdl2 ${SOURCE_FILES})

target_link_libraries(brick_sdl2 PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        sdl_ttf::sdl_ttf)

add_custom_command(TARGET brick_sdl2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/images
        $<TARGET_FILE_DIR:brick_sdl2>/images
)
