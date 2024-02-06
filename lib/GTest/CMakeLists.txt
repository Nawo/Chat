cmake_minimum_required(VERSION 3.21)
project(GTest)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main # release-1.10.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB CLASSES *.cpp)
set(SOURCES ${CLASSES})

enable_testing()

add_executable(${PROJECT_NAME} ${SOURCES}) 

target_link_libraries(
  ${PROJECT_NAME}
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})