cmake_minimum_required(VERSION 3.0)
project(Quesync)

# Make the shared headers compile for the server
add_definitions (-DQUESYNC_SERVER)

# Add the include folder
include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)

# If on windows set the openssl folder
IF (WIN32)
    set (OPENSLL_DIR C:/OpenSSL-Win64)

    set (OPENSSL_LIBS "VC/libeay32MT VC/ssleay32MT")
ENDIF()

# If on macos set the openssl dir
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (OPENSLL_DIR /usr/local/opt/openssl)

    set (OPENSSL_LIBS "crypto")
ENDIF()

# If there is an openssl dir, set the include and lib directory
if (OPENSLL_DIR)
    include_directories(${OPENSLL_DIR}/include)
    link_directories(${OPENSLL_DIR}/lib)
ENDIF()

# Preprocessor defines
add_definitions(-DASIO_STANDALONE) # Build ASIO without boost
add_definitions(-D_WIN32_WINNT=0x0501) # Set Windows version
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Ignore unsafe warnings

# Set C++14 standard
set (CMAKE_CXX_STANDARD 14)

# Get all the cpp files for the server
file(GLOB SOURCES "*.cpp")

# Get all the cpp files for the shared source
file(GLOB SHARED_SOURCES "../shared/*.cpp")

# Get all c files that are associated with the libraries used
file(GLOB_RECURSE INCLUDE_CPP RELATIVE ${CMAKE_CURRENT_LIST_DIR} "../include/*.c")

# Add the server as an executable to be compiled
add_executable(server ${SOURCES} ${SHARED_SOURCES} ${INCLUDE_CPP})

# Link the server to the openssl lib
target_link_libraries(server ${OPENSSL_LIBS})