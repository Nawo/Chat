cmake_minimum_required(VERSION 3.21)
project(Chat LANGUAGES CXX)

add_subdirectory(Client)
#add_subdirectory(Server)
add_subdirectory(Core/Server)


# cmake -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build . --config Release
