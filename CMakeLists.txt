cmake_minimum_required(VERSION 3.5)

project(demo)

# cmake_policy(SET CMP0072 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Why are these needed? 
# (otherwise we get: "error: libGLEW.so.2.1.0 not found at runtime")
set(CMAKE_BUILD_TYPE NONE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/usr/lib64")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Target cpp 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/build)

# find sdl2 and glew
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# add the include (header) directories for sdl2 and glew
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(src include)

# sld2 has some weird extra whitespace that breaks things?
string(STRIP ${SDL2_INCLUDE_DIRS} SDL2_INCLUDE_DIRS)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)

add_compile_options(-Wall -Wextra -Wpedantic)

# set the sources for the executable
set(SOURCES 
    src/gl/shaderUtils.cpp
    src/gl/glObject.cpp
    src/camera.cpp
    src/light/light.cpp
    src/light/directionalLight.cpp
    src/light/pointLight.cpp
    src/light/spotLight.cpp
    src/material/deferredMaterial.cpp
    src/material/material.cpp
    src/mesh.cpp
    src/model.cpp
    src/renderer.cpp
    src/renderTarget.cpp
)

# Add the executable
add_executable(demo 
    src/main.cpp
    ${SOURCES}
)
# Link libraries as necessary
target_link_libraries(demo ${SDL2_LIBRARIES})
target_link_libraries(demo ${FREETYPE_LIBRARIES})
target_link_libraries(demo ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
