cmake_minimum_required(VERSION 3.21)
project(Tests LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

if(BUILD_TESTING)

    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main # release-1.10.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    FetchContent_MakeAvailable(googletest)
    add_library(GTest::GTest INTERFACE IMPORTED)
    target_link_libraries(GTest::GTest INTERFACE gtest_main)

    file(GLOB CLASSES *.cpp)
    set(SOURCES ${CLASSES})

    include_directories(../Handlers)

    add_executable(${PROJECT_NAME} ${SOURCES}) 
    target_link_libraries(${PROJECT_NAME} PRIVATE GTest::GTest)

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME})
    
endif()