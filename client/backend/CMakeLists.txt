cmake_minimum_required(VERSION 3.1)
project(backend)

# Make the shared headers compile for the client
add_definitions(-DQUESYNC_CLIENT)

# Ignore all C++17 deprecation warnings on windows
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Add the thirdparty as include folder
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../thirdparty)

# Change runtime library to MD
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd")

add_subdirectory(../../vendor/opus opus)
include_directories(../../vendor/opus/include)

# Windows dependencies
IF (WIN32)
    set (OPENSLL_DIR C:/OpenSSL-Win64)
	set (OPENSSL_LIBS VC/libeay32MT VC/ssleay32MT)
	
	set (OPENAL_DIR "C:/Program Files (x86)/OpenAL 1.1 SDK")
	set (OPENAL_LIB_DIR libs/Win64)
	set (OPENAL_LIBS OpenAL32)
ENDIF()

# Macos dependencies
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (OPENSLL_DIR /usr/local/opt/openssl)
	set (OPENSSL_LIBS ssl crypto)
	
	set (OPENAL_DIR /usr/local/opt/openssl)
	set (OPENAL_LIB_DIR lib)
	set (OPENAL_LIBS openal)
ENDIF()

# If there is an openssl dir, set the include and lib directory
IF (OPENSLL_DIR)
    include_directories(${OPENSLL_DIR}/include)
    link_directories(${OPENSLL_DIR}/lib)
ENDIF()

# If there is an openal dir, set the include and lib directory
IF (OPENAL_DIR)
    include_directories(${OPENAL_DIR}/include)
    link_directories(${OPENAL_DIR}/${OPENAL_LIB_DIR})
ENDIF()

# Preprocessor defines
add_definitions(-DASIO_STANDALONE) # Build ASIO without boost
add_definitions(-D_WIN32_WINNT=0x0501) # Set Windows version
add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Ignore unsafe warnings

# Set C++17 standard
set (CMAKE_CXX_STANDARD 17)

# Get all the cpp files for the server
file(GLOB SOURCES "src/*.cpp" "src/*.h")

# Get all the cpp files for the shared source
file(GLOB SHARED_SOURCES "../../shared/*.cpp" "../../shared/*.h")

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
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBS} ${OPENAL_LIBS} ${CMAKE_JS_LIB} opus)