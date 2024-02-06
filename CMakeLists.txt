cmake_minimum_required(VERSION 3.21)
project(Chat LANGUAGES CXX)

include(CTest)
enable_testing()

add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(lib)
add_subdirectory(Tests)


# cmake -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build . --config Release
