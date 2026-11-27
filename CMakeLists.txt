cmake_minimum_required(VERSION 3.22)
project(underwater_treasure_hunt C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(underwater_treasure_hunt src/main.c)
target_link_libraries(underwater_treasure_hunt PRIVATE raylib)

if(EMSCRIPTEN)
    set_target_properties(underwater_treasure_hunt PROPERTIES SUFFIX ".html")
    target_link_options(underwater_treasure_hunt PRIVATE
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/src/web_shell.html"
    )
endif()
