cmake_minimum_required(VERSION 3.21)
project(sqlite)

SET (BUILD_SHARED_LIBS TRUE)
add_library(${PROJECT_NAME} STATIC sqlite3.c)