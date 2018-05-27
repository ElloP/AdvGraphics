cmake_minimum_required ( VERSION 3.0.2 )

project ( haze)

# Find *all* shaders.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)
# Separate filter for shaders.
source_group("Shaders" FILES ${SHADERS})

# Build and link executable.
add_executable ( haze
    main.cpp
    fbo.cpp
    hdr.cpp
    particlesystem.cpp
    particlegalaxy.cpp
    ${SHADERS}
    )

target_link_libraries ( haze labhelper )
config_build_output()
