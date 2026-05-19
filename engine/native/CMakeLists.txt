include(Compiler)
include(Modules)

add_modules_library(core SHARED)
target_link_libraries(core PUBLIC definitions math io memory)

add_subdirectory(thirdparty SYSTEM)
add_subdirectory(platform)

add_modules_library(input)
target_link_libraries(input PRIVATE SDL3::SDL3-static)

add_modules_library(scene)
target_link_libraries(scene PUBLIC renderable)

add_subdirectory(rendering)