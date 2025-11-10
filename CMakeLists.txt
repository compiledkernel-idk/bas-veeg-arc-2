cmake_minimum_required(VERSION 3.20)
project(BasVeegArc3D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /O2 /arch:AVX2)
    # Disable specific warnings for external headers
    add_compile_options(/wd4251 /wd4275)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native)
endif()

# Find required packages
find_package(OGRE REQUIRED COMPONENTS Bites RTShaderSystem Overlay)
find_package(Bullet REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ENet REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${OGRE_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIR}
    ${ENET_INCLUDE_DIR}
)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "include/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OGRE_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${ENET_LIBRARIES}
    pthread
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 winmm)
    # Set working directory for Visual Studio
    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} pthread X11 dl)
endif()

# Copy shaders to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY shaders DESTINATION share/${PROJECT_NAME})

# Print build info
message(STATUS "===========================================")
message(STATUS "Bas Veeg Arc 3D - Build Configuration")
message(STATUS "===========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "===========================================")
message(STATUS "Game Features:")
message(STATUS "  ✓ 13 Playable Characters (Procedural)")
message(STATUS "  ✓ 6 Epic Bosses (Procedural)")
message(STATUS "  ✓ PBR Graphics & Shaders")
message(STATUS "  ✓ Procedural Audio Synthesis")
message(STATUS "  ✓ Zero External Asset Files")
message(STATUS "  ✓ Apache License 2.0")
message(STATUS "===========================================")
