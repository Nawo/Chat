cmake_minimum_required(VERSION 3.21)
project(Chat LANGUAGES CXX)

file(GLOB CLASSES *.cpp)
set(SOURCES ${CLASSES})

add_definitions(-D_WIN32_WINNT=0x0601)

include_directories(../lib/asio-1.28.0/include)
add_executable(${PROJECT_NAME} ${SOURCES}) 