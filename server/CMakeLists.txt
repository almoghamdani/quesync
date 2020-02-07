cmake_minimum_required(VERSION 3.0)
project(server)

# Make the shared headers compile for the server
add_definitions (-DQUESYNC_SERVER)

# Build into bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include zxcvbn
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/zxcvbn)

# Include asio
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/asio/asio/include)

# Include nlohmann::json
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/json/include)

# Include termcolor
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/termcolor/include)

# Include magic_enum
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/magic_enum/include)

# Include sole
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/sole)

# Include cxxopts
include_directories(${CMAKE_CURRENT_LIST_DIR}/../vendor/cxxopts/include)

# Windows dependencies
IF (WIN32)
    set (OPENSSL_INCLUDE_DIR C:/OpenSSL-Win64/include)
    set (OPENSSL_LIBS_DIR C:/OpenSSL-Win64/lib)
    set (OPENSSL_LIBS VC/libeay32MT VC/ssleay32MT)
    set (OPENSSL_DLLS_DIR C:/OpenSSL-Win64)

    set (MYSQL_DIR "C:/Program Files/MySQL/Connector C++ 8.0")
    set (MYSQL_LIBS vs14/mysqlcppconn8-static)
ENDIF()

# macOS dependencies
IF (APPLE)
    set (OPENSSL_INCLUDE_DIR /usr/local/opt/openssl/include)
    set (OPENSSL_LIBS_DIR /usr/local/opt/openssl/lib)
    set (OPENSSL_LIBS ssl crypto)

    set (MYSQL_DIR /usr/local/mysql-connector-c++)
    set (MYSQL_LIBS libmysqlcppconn8-static.a)
ENDIF()

# If there is an openssl dir, set the include and lib directory
IF (OPENSSL_INCLUDE_DIR AND OPENSSL_LIBS_DIR)
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_directories(${OPENSSL_LIBS_DIR})
ENDIF()

IF (MYSQL_DIR)
    include_directories(${MYSQL_DIR}/include)
    link_directories(${MYSQL_DIR}/lib64)

    # Static compile of the library
    add_definitions(-DSTATIC_CONCPP)
ENDIF()

# Generate header file with database dump content
file(WRITE ${PROJECT_BINARY_DIR}/database_dump.c "")
file(READ database-dump.sql filedata HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
file(APPEND ${PROJECT_BINARY_DIR}/database_dump.c "const unsigned char database_dump[] = {${filedata}};\nconst unsigned database_dump_size = sizeof(database_dump);\n")

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
file(GLOB SOURCES "src/*.cpp")

# Get all the cpp files for the shared source
file(GLOB_RECURSE SHARED_SOURCES "../shared/*/**.cpp")

# Get all c++ files that are associated with the libraries used
file(GLOB_RECURSE INCLUDE_CPP RELATIVE ${CMAKE_CURRENT_LIST_DIR} "../vendor/zxcvbn/**/*.cpp" ${PROJECT_BINARY_DIR}/database_dump.c)

# Add the server as an executable to be compiled
add_executable(${PROJECT_NAME} ${SOURCES} ${SHARED_SOURCES} ${INCLUDE_CPP})

# Link the server to the dependencies' libs
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBS} ${MYSQL_LIBS})

# Copy OpenSSL dlls after build
if (WIN32 AND OPENSSL_DLLS_DIR)
    file(GLOB WIN_OPENSSL_DLLS "${OPENSSL_DLLS_DIR}/*.dll")

    foreach (OPENSSL_DLL ${WIN_OPENSSL_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENSSL_DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach()
endif()

# Delete PDB and ILK after build since they aren't needed
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.ilk)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.pdb)
endif()