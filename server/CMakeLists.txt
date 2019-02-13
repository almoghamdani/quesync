cmake_minimum_required(VERSION 3.0)
project(Quesync)

# Add the include folder
include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)

# If on windows include the openssl headers files for windows
IF (WIN32)
    include_directories(C:/OpenSSL-Win64/include)
ENDIF()

# Preprocessor defines
add_definitions(-DASIO_STANDALONE) # Build ASIO without boost
add_definitions(-D_WIN32_WINNT=0x0501) # Set Windows version

# Set C++14 standard
set (CMAKE_CXX_STANDARD 14)

# Get all the cpp files
file(GLOB SOURCES "*.cpp")

# Get all c files that are associated with the libraries used
file(GLOB_RECURSE INCLUDE_CPP RELATIVE ${CMAKE_CURRENT_LIST_DIR} "../include/*.c")

# Add the server as an executable to be compiled
add_executable(server ${SOURCES} ${INCLUDE_CPP})