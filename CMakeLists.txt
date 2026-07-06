cmake_minimum_required(VERSION 3.30)
project(PardusJarvis VERSION 1.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Quick QuickControls2)
include(FetchContent)
FetchContent_Declare(
    Lowwi
    GIT_REPOSITORY https://github.com/CLFML/lowwi
    GIT_TAG main
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/Lowwi
)
FetchContent_MakeAvailable(Lowwi)
add_executable(PardusJarvis main.cpp backend.h backend.cpp miniaudio.h audio.cpp dr_wav.h wav.cpp)
add_executable(JarvisDaemon daemon/main.cpp)
qt_add_resources(PardusJarvis "assets"
    PREFIX "/"
    FILES
        main.qml
        send.svg
        mic.svg
)
set_target_properties(PardusJarvis PROPERTIES OUTPUT_NAME "Pardus Jarvis")
set_target_properties(JarvisDaemon PROPERTIES OUTPUT_NAME "Pardus-Jarvis-Daemon")
configure_file(version_template.h version.h)
target_include_directories(PardusJarvis PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(JarvisDaemon PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(PardusJarvis PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Quick
    Qt6::QuickControls2
)
target_link_libraries(JarvisDaemon PRIVATE CLFML::lowwi)
add_custom_command(TARGET JarvisDaemon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lib/Lowwi/models
        $<TARGET_FILE_DIR:JarvisDaemon>/models
)