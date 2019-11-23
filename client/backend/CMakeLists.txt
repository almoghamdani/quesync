cmake_minimum_required(VERSION 3.1)
project(backend)

# Make the shared headers compile for the client
add_definitions(-DQUESYNC_CLIENT)

# Ignore all C++17 deprecation warnings on windows
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Add the thirdparty as include folder
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../thirdparty)

# Change runtime library to MD
IF (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "/MD")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
ENDIF()

# Include ASIO
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../vendor/asio/asio/include)

# Include nlohmann::json
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../vendor/json/include)

# Compile Opus
set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTING OFF)
add_subdirectory(../../vendor/opus opus)
include_directories(../../vendor/opus/include)

# Compile RNNoise
add_definitions(-DCOMPILE_OPUS)
add_subdirectory(../../vendor/rnnoise-cmake rnnoise)
include_directories(../../vendor/rnnoise-cmake/include)

# Compile RtAudio
set(RTAUDIO_BUILD_TESTING OFF)
add_subdirectory(../../vendor/rtaudio rtaudio)
include_directories(../../vendor/rtaudio)

# Windows dependencies
IF (WIN32)
    set (OPENSLL_INCLUDE_DIR C:/OpenSSL-Win64/include)
    set (OPENSLL_LIBS_DIR C:/OpenSSL-Win64)
    set (OPENSSL_LIBS lib/VC/libeay32MT lib/VC/ssleay32MT)
ENDIF()

# macOS dependencies
IF (APPLE)
    set (OPENSLL_INCLUDE_DIR /usr/local/opt/openssl/include)
    set (OPENSLL_LIBS_DIR /usr/local/opt/openssl/lib)
    set (OPENSSL_LIBS ssl crypto)
ENDIF()

# Linux dependencies
IF (UNIX AND NOT APPLE)
    set (OPENSLL_INCLUDE_DIR /usr/include/openssl)
    set (OPENSLL_LIBS_DIR /usr/lib)
    set (OPENSSL_LIBS ssl crypto)
ENDIF()

# If there is an openssl dir, set the include and lib directory
IF (OPENSLL_INCLUDE_DIR AND OPENSLL_LIBS_DIR)
    include_directories(${OPENSLL_INCLUDE_DIR})
    link_directories(${OPENSLL_LIBS_DIR})
ENDIF()

# Preprocessor defines
add_definitions(-DASIO_STANDALONE) # Build ASIO without boost
add_definitions(-D_WIN32_WINNT=0x0501) # Set Windows version
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Ignore unsafe warnings

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Get all the cpp files for the server
file(GLOB_RECURSE SOURCES "backend.cpp" "src/*/**.cpp" "src/*/**.h")

# Get all the cpp files for the shared source
file(GLOB_RECURSE SHARED_SOURCES "../../shared/*/**.cpp" "../../shared/*/**.h")

# Get all c++ files that are associated with the libraries used
file(GLOB_RECURSE INCLUDE_CPP RELATIVE ${CMAKE_CURRENT_LIST_DIR} "../../thirdparty/*.cpp" "../../thirdparty/*.h")

# Add the backend as a library
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${SHARED_SOURCES} ${INCLUDE_CPP} ${CMAKE_JS_SRC})

# Remove lib prefix + add node suffix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# Compile the library
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBS} ${CMAKE_JS_LIB} opus rtaudio rnnoise)

# Copy OpenSSL dlls after build
if (WIN32 AND OPENSLL_LIBS_DIR)
    file(GLOB WIN_OPENSSL_DLLS "${OPENSLL_LIBS_DIR}/*.dll")

    foreach (OPENSSL_DLL ${WIN_OPENSSL_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENSSL_DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach()
endif()
