add_modules_library(camera)

target_link_libraries(camera PUBLIC input renderer bx)

add_modules_library(transform)

target_link_libraries(transform PUBLIC bx)

add_modules_library(renderable)

target_link_libraries(renderable PUBLIC mesh transform material)
