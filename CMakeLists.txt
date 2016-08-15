cmake_minimum_required(VERSION 2.8.4)
project(voxelizer)

#
# Compiler Strings
#
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-int-to-void-pointer-cast")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/render/render_files.cmake)

#
# Source Files
#
set(SOURCE_FILES ${RENDER_FILES}
        src/Main.cpp
        src/IsoVoxels.cpp
        src/VBOCube.cpp
        src/VoxelBase.cpp
        src/VoxelizerApp.cpp
        src/VoxelizerApp.h
        src/VoxelAsset.h
        src/VoxelAsset.cpp)

#
# Link Directories
#
if(APPLE)
    link_directories(ext/libs-lldb)
elseif(WIN32 AND MINGW)
    link_directories(ext/libs-ming64)
endif()


find_package(OpenGL REQUIRED)

# OSX required libraries
if(APPLE)
    find_package(zlib   REQUIRED)
    find_library(COCOA_LIBRARY   Cocoa     REQUIRED)
    find_library(IOKIT_LIBRARY   IOKit     REQUIRED)
    find_library(COREVID_LIBRARY CoreVideo REQUIRED)
endif()

include_directories(render ext/include)
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(/usr/local/opt/opencv3/include)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_SEARCH_END_STATIC 1)

target_link_libraries(${PROJECT_NAME} ${COREVID_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${COCOA_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${IOKIT_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})

target_link_libraries(${PROJECT_NAME} libglfw3.a)
target_link_libraries(${PROJECT_NAME} libassimp.a)
if(WIN32 AND MINGW)
    target_link_libraries(${PROJECT_NAME} libopengl32.a)
    target_link_libraries(${PROJECT_NAME} libimm32.a)
    target_link_libraries(${PROJECT_NAME} libzlibstatic.a)
endif()