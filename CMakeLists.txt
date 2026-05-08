cmake_minimum_required(VERSION 4.0)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
endif()

find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    message(STATUS "Updating submodules...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "Failed to update submodules. Please run manually.")
    endif()
endif()

project(DraconicEngine LANGUAGES C CXX)

# Global Rules
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Force Clang/libc++ to avoid ABI mismatch
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-mavx2 -mfma -stdlib=libc++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Subdirectories for engine libs
add_subdirectory(engine/native)

# Main Executable
add_executable(draconic engine/native/main/main.cpp)

enable_modules(draconic)

set_target_properties(draconic PROPERTIES CXX_SCAN_FOR_MODULES ON)

target_link_libraries(draconic
    PRIVATE
    core
    io
    memory
    image_loader
    platform
    input
    camera # Camera controller
    transform
    rhi
    renderer
    rendergraph
    mesh
    bgfx
    bx
    bimg
)

find_package(Threads REQUIRED)

if(UNIX AND NOT APPLE)
    target_link_libraries(draconic PRIVATE SDL3::SDL3-static Threads::Threads dl)
    target_link_options(draconic PRIVATE -Wl,--whole-archive $<TARGET_FILE:SDL3::SDL3-static> -Wl,--no-whole-archive)
elseif(APPLE)
    target_link_libraries(draconic PRIVATE SDL3::SDL3-static)
    target_link_options(draconic PRIVATE -Wl,-force_load,$<TARGET_FILE:SDL3::SDL3-static>)
else()
    target_link_libraries(draconic PRIVATE SDL3::SDL3-static)
endif()

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/engine/native/rendering/shaders")

set(BGFX_INCLUDE "${CMAKE_SOURCE_DIR}/engine/native/thirdparty/bgfx/src")

if(WIN32)

    set(SHADERC
        "${CMAKE_SOURCE_DIR}/engine/native/thirdparty/bgfx/.build/win64_vs2022/bin/shadercRelease.exe"
    )

    set(SHADER_PLATFORM windows)
    set(SHADER_PROFILE spirv)

elseif(APPLE)

    set(SHADERC
        "${CMAKE_SOURCE_DIR}/engine/native/thirdparty/bgfx/.build/osx-arm64_clang/bin/shadercRelease"
    )

    set(SHADER_PLATFORM osx)
    set(SHADER_PROFILE metal)

elseif(UNIX)

    set(SHADERC
        "${CMAKE_SOURCE_DIR}/engine/native/thirdparty/bgfx/.build/linux64_gcc/bin/shadercRelease"
    )

    set(SHADER_PLATFORM linux)
    set(SHADER_PROFILE spirv)

endif()

# Shader source files
set(VERTEX_SHADER "${SHADER_DIR}/vs.sc")
set(FRAGMENT_SHADER "${SHADER_DIR}/fs.sc")
set(VARYING_DEF   "${SHADER_DIR}/varying.def.sc")

# Output binaries
set(VERTEX_OUTPUT "${CMAKE_BINARY_DIR}/vs.bin")
set(FRAGMENT_OUTPUT "${CMAKE_BINARY_DIR}/fs.bin")

# Compile vertex shader
add_custom_command(
    OUTPUT ${VERTEX_OUTPUT}

    COMMAND ${SHADERC}
        -f ${VERTEX_SHADER}
        -o ${VERTEX_OUTPUT}
        --type vertex
        --platform ${SHADER_PLATFORM}
        -p ${SHADER_PROFILE}
        --varyingdef ${VARYING_DEF}
        -i ${BGFX_INCLUDE}

    DEPENDS
        ${VERTEX_SHADER}
        ${VARYING_DEF}

    COMMENT "Compiling vertex shader..."
)

# Compile fragment shader
add_custom_command(
    OUTPUT ${FRAGMENT_OUTPUT}

    COMMAND ${SHADERC}
        -f ${FRAGMENT_SHADER}
        -o ${FRAGMENT_OUTPUT}
        --type fragment
        --platform ${SHADER_PLATFORM}
        -p ${SHADER_PROFILE}
        --varyingdef ${VARYING_DEF}
        -i ${BGFX_INCLUDE}

    DEPENDS
        ${FRAGMENT_SHADER}
        ${VARYING_DEF}

    COMMENT "Compiling fragment shader..."
)

# Shader target
add_custom_target(shaders ALL
    DEPENDS
        ${VERTEX_OUTPUT}
        ${FRAGMENT_OUTPUT}
)

# Make executable depend on shaders
add_dependencies(draconic shaders)

# Copy shader binaries to executable dir
add_custom_command(
    TARGET draconic POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VERTEX_OUTPUT}
        $<TARGET_FILE_DIR:draconic>

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FRAGMENT_OUTPUT}
        $<TARGET_FILE_DIR:draconic>

    COMMENT "Copying compiled shaders..."
)