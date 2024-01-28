cmake_minimum_required(VERSION 3.21)
project(Chat LANGUAGES CXX)

# add_subdirectory(Client)
# add_subdirectory(NewCore/Server)
add_subdirectory(connection)
add_subdirectory(sample)
add_subdirectory(NewCore/Client)
add_subdirectory(NewCore/Server)


# cmake -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build . --config Release
