cmake_minimum_required(VERSION 3.10)
project(ProcessSchedulingSim C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

# Put all source files in src/
file(GLOB SRC_FILES "src/*.c")
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(ProcessSchedulingSim main.c ${SRC_FILES} src/quantum.rc)

# Windows resource file handling
if(MSVC)
    set_source_files_properties(src/quantum.rc PROPERTIES LANGUAGE RC)
else()
    # MinGW/GCC needs windres
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set_source_files_properties(src/quantum.rc PROPERTIES OBJECT_DEPENDENCIES src/quantum.rc)
endif()

# Link Windows GUI library
#target_link_libraries(ProcessSchedulingSim PRIVATE comdlg32)

# If you want OpenGL, add:
# target_link_libraries(ProcessSchedulingSim PRIVATE opengl32)