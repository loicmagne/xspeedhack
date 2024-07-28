cmake_minimum_required(VERSION 3.10)
project(xspeedhack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Injector executable

set(EXE_SOURCES
    cpp/injector.cpp
)

add_executable(injector ${EXE_SOURCES})

# DLL Speedhack

set(LIB_SOURCES
    cpp/dll_main.cpp
)

include_directories("${CMAKE_SOURCE_DIR}/deps/include")
add_library(speedhack SHARED ${LIB_SOURCES})
# Check if the platform is 64-bit or 32-bit
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    set(DETOURS_LIB_PATH "${CMAKE_SOURCE_DIR}/deps/lib/detoursx64.lib")
else()
    # 32-bit
    set(DETOURS_LIB_PATH "${CMAKE_SOURCE_DIR}/deps/lib/detoursx86.lib")
endif()

target_link_libraries(speedhack PRIVATE ${DETOURS_LIB_PATH})