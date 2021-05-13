add_subdirectory(cglm)


add_library(cute_headers
    cute_headers/cute_impl.c
)


add_library(glad
    glad/glad.c
)   

add_library(nuklear
    nuklear/nuklear_impl.c
    nuklear/nuklear_sdl_gl3.c
    nuklear/demos.c
)

add_library(stb
    stb/stb_impl.c
)

