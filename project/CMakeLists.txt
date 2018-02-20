cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

# Find *all* shaders.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)
# Separate filter for shaders.
source_group("Shaders" FILES ${SHADERS})

# Build and link executable.
add_executable ( project
    main.cpp
    fbo.cpp
    hdr.cpp
    heightfield.cpp
    particlesystem.cpp
    particlegalaxy.cpp
    ${SHADERS}
    )

target_link_libraries ( project labhelper )
config_build_output()
