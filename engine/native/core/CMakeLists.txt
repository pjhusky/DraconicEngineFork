add_modules_library(definitions PIC)
add_modules_library(math)
target_link_libraries(math PUBLIC definitions)

add_modules_library(io)
target_link_libraries(io PUBLIC definitions stb_image)

add_modules_library(memory)
target_link_libraries(memory PUBLIC definitions)