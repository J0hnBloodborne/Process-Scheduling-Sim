cmake_minimum_required(VERSION 3.10)
project(ProcessSchedulingSim C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SRC_FILES "src/*.c")
include_directories(${CMAKE_SOURCE_DIR}/src)

# Resource file (if you want it)
set(RC_FILE src/quantum.rc)

# Build as GUI app, no console pop-up
add_executable(ProcessSchedulingSim WIN32 main.c ${SRC_FILES} ${RC_FILE})

# Link Windows GUI libraries
target_link_libraries(ProcessSchedulingSim PRIVATE comdlg32 gdi32)

# Try to statically link everything (works with MinGW-w64, not MSVC)
if(MINGW)
    set_target_properties(ProcessSchedulingSim PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++ -mwindows"
    )
endif()

# Output exe to release/
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set_target_properties(ProcessSchedulingSim PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_SOURCE_DIR}/release")
endforeach()