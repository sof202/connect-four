cmake_minimum_required(VERSION 3.17)
project(connect-four)

# ------------------
# Options
# ------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

add_compile_definitions(BUILD_TYPE="${CMAKE_BUILD_TYPE}")

# ------------------
# Git
# ------------------
if(NOT PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  # Auto-create a .gitignore in the specified "build" directory.
  file(GENERATE OUTPUT .gitignore CONTENT "*")
endif()

# ------------------
# Build
# ------------------
add_executable(
    connect-four
    src/main.cpp
)
target_include_directories(
    connect-four
    PUBLIC
        src/
)

# ------------------
# Install
# ------------------
install(TARGETS connect-four
RUNTIME DESTINATION bin
)
