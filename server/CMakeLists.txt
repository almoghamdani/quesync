cmake_minimum_required(VERSION 3.0)
project(Quesync)

# Make the shared headers compile for the server
add_definitions (-DQUESYNC_SERVER)

# Add the thirdparty as include folder
include_directories(${CMAKE_CURRENT_LIST_DIR}/../thirdparty)

# Include ASIO
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/asio/asio/include)

# Windows dependencies
IF (WIN32)
    set (OPENSLL_DIR C:/OpenSSL-Win64)
    set (OPENSSL_LIBS VC/libeay32MT VC/ssleay32MT)

    set (MYSQL_DIR "C:/Program Files/MySQL/Connector C++ 8.0")
    set (MYSQL_LIBS vs14/mysqlcppconn8-static)
ENDIF()

# Macos dependencies
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (OPENSLL_DIR /usr/local/opt/openssl)
    set (OPENSSL_LIBS ssl crypto)

    set (MYSQL_DIR /usr/local/mysql-connector-c++)
    set (MYSQL_LIBS libmysqlcppconn8-static.a)
ENDIF()

# If there is an openssl dir, set the include and lib directory
IF (OPENSLL_DIR)
    include_directories(${OPENSLL_DIR}/include)
    link_directories(${OPENSLL_DIR}/lib)
ENDIF()

IF (MYSQL_DIR)
    include_directories(${MYSQL_DIR}/include)
    link_directories(${MYSQL_DIR}/lib64)

    # Static compile of the library
    add_definitions(-DSTATIC_CONCPP)
ENDIF()

# Change runtime library to MD
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MD")

# Preprocessor defines
add_definitions(-DASIO_STANDALONE) # Build ASIO without boost
add_definitions(-D_WIN32_WINNT=0x0501) # Set Windows version
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Ignore unsafe warnings

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS) # Silence warnings on windows

# Get all the cpp files for the server
file(GLOB SOURCES "*.cpp")

# Get all the cpp files for the shared source
file(GLOB_RECURSE SHARED_SOURCES "../shared/*/**.cpp")

# Get all c++ files that are associated with the libraries used
file(GLOB_RECURSE INCLUDE_CPP RELATIVE ${CMAKE_CURRENT_LIST_DIR} "../thirdparty/*.cpp")

# Add the server as an executable to be compiled
add_executable(server ${SOURCES} ${SHARED_SOURCES} ${INCLUDE_CPP})

# Link the server to the dependencies' libs
target_link_libraries(server ${OPENSSL_LIBS} ${MYSQL_LIBS})