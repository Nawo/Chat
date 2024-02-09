cmake_minimum_required(VERSION 3.21)
project(ClientLib LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

file(GLOB CLASSES *.cpp)
set(SOURCES ${CLASSES})

add_definitions(-D_WIN32_WINNT=0x0601) #minimum required windows 7

include_directories(../lib/asio-1.28.0/include)
include_directories(../lib/sqlite3)
include_directories(../Common)
include_directories(../Handlers)
include_directories(../Core/Client)

add_library(${PROJECT_NAME} STATIC ${SOURCES}) 

target_link_libraries(${PROJECT_NAME} sqlite)