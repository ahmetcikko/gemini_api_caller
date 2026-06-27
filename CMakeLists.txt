cmake_minimum_required(VERSION 3.30)
project(APIAPP VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Network)
add_executable(APIAPP main.cpp call.h call.cpp inputhandler.h inputhandler.cpp)
set_target_properties(APIAPP PROPERTIES OUTPUT_NAME "API App")
target_link_libraries(APIAPP PRIVATE
    Qt6::Core
    Qt6::Network
    "-lstdc++exp"
)