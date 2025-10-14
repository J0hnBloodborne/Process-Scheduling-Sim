cmake_minimum_required(VERSION 3.10)
project(ProcessSchedulingSim C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SRC_FILES "src/*.c")
# Headers moved to include/
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

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

# Statically link the MSVC runtime to avoid requiring the VC++ Redistributable
# Works for CMake < 3.15 by rewriting /MD to /MT
if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL)
        if(DEFINED ${flag_var})
            string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Output exe to release/
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set_target_properties(ProcessSchedulingSim PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_SOURCE_DIR}/release")
endforeach()