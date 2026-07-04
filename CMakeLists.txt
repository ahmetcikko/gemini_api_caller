cmake_minimum_required(VERSION 3.30)
project(APIAPP VERSION 1.2.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Quick QuickControls2)
add_executable(APIAPP WIN32 main.cpp backend.h backend.cpp miniaudio.h audio.cpp dr_wav.h wav.cpp)
qt_add_resources(APIAPP "assets"
    PREFIX "/"
    FILES
        main.qml
        send.svg
        mic.svg
)
set_target_properties(APIAPP PROPERTIES OUTPUT_NAME "AI Caller")
configure_file(version_template.h version.h)
target_include_directories(APIAPP PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(APIAPP PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Quick
    Qt6::QuickControls2
)