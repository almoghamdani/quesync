cmake_minimum_required(VERSION 3.0)
project(Quesync)

# Get all the cpp files
file(GLOB SOURCES "*.cpp")

# Add the server as an executable to be compiled
add_executable(server ${SOURCES})