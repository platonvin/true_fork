cmake_minimum_required(VERSION 4.0.2)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
# set(CMAKE_EXPERIMENTAL_EXPORT_BUILD_DATABASE "73194a1d-c0b5-41b9-9190-a4512925e192")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_MODULE_STD 1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,100000000")

project(lum LANGUAGES C CXX)
add_executable(demo)

# i have no fucking idea how to build using its cmake it so i build it myself (literally just include files)
# add_subdirectory(src/deps/glm)

add_subdirectory(src/deps/glfw)

# target_link_libraries(demo PRIVATE glm)

include_directories(demo PRIVATE 
    src/deps
    src/deps/glm
    src/deps/RmlUi/Include
)

# freetype - font lib for rmlui
set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
add_subdirectory(src/deps/freetype)

# rmlui - library for perfomant ui rendering
set(RMLUI_FONT_ENGINE none)
set(RMLUI_STATIC_LIBRARIES ON)
set(RMLUI_BUILD_SHARED_LIBRARIES OFF)
set(RMLUI_CUSTOM_RTTI ON)
add_subdirectory(src/deps/RmlUi)

target_link_libraries(demo PRIVATE glfw freetype)

target_sources(demo
    PUBLIC
    FILE_SET CXX_MODULES TYPE CXX_MODULES FILES
    src/math.cppm
    
    src/containers/shared_vector.cppm
    src/containers/ecm.cppm
    src/containers/thread_pool.cppm
    src/containers/ring.cppm
    src/containers/table3d.cppm
    src/containers/arena.cppm
    src/containers/fixed_map.cppm

    src/engine/input.cppm
    
    src/render/ao_lut.cppm

    src/render/lumal/lumal.cppm
        src/render/lumal/types.cppm
        src/render/lumal/init.cppm
        src/render/lumal/setup.cppm

    src/render/lum.cppm
        src/render/render.cppm
        src/render/load.cppm
        src/render/setup.cppm
        src/render/wrapper.cppm

    src/deps/glm/glm/glm.cppm
    src/deps/glfw.cppm
    src/deps/rmlui.cppm
    src/deps/ogt_voxel_meshify.cppm
    src/deps/ogt_vox.cppm
)

target_sources(demo
    PRIVATE

    src/main.cpp
    
    src/deps/volk/volk.c
    src/deps/vma/vma.cpp
    src/deps/meshopt.cpp
)
