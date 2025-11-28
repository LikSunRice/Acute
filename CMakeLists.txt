cmake_minimum_required(VERSION 3.15)
project(AcuteDrawing VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Find packages
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Window.cpp
    src/Canvas.cpp
    src/InputManager.cpp
    src/BrushEngine.cpp
    src/Renderer.cpp
    src/Shader.cpp
)

# Windows-specific sources
if(WIN32)
    list(APPEND SOURCES src/WindowsInkInput.cpp)
endif()

set(HEADERS
    include/Application.h
    include/Window.h
    include/Canvas.h
    include/InputManager.h
    include/BrushEngine.h
    include/BrushDab.h
    include/Renderer.h
    include/Shader.h
    include/InputTypes.h
    include/BrushMapping.h
)

# Windows-specific headers
if(WIN32)
    list(APPEND HEADERS include/WindowsInkInput.h)
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Link libraries using modern CMake targets
# SDL2 (handles both shared and static, with optional SDL2main)
if(TARGET SDL2::SDL2main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()
if(TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static)
endif()

# GLEW and OpenGL
target_link_libraries(${PROJECT_NAME} PRIVATE
    GLEW::GLEW
    OpenGL::GL
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        dl
        pthread
    )
endif()

# Shaders are embedded in Canvas.cpp, no need to copy files

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()


