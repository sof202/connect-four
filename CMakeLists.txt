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

if (CMAKE_BUILD_TYPE STREQUAL "Dev") 
add_compile_options(-ggdb -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror)
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
# Build network lib
# ------------------
set(NETWORK_SOURCES
    src/network/socket.cpp
    src/network/clientSocket.cpp
    src/network/serverSocket.cpp
)
set(NETWORK_HEADERS
    src/network/socket.hpp
    src/network/clientSocket.hpp
    src/network/serverSocket.hpp
    src/network/address.hpp
)
add_library(network_lib STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

target_include_directories(network_lib
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(network_lib PRIVATE Threads::Threads)

# ------------------
# Build server
# ------------------
add_executable(
    server
    src/server.cpp
)
target_link_libraries(server PRIVATE network_lib)

# ------------------
# Build client
# ------------------
add_executable(
    client
    src/client.cpp
)
target_link_libraries(client PRIVATE network_lib)
