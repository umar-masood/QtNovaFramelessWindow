cmake_minimum_required(VERSION 3.16)
project(Window VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(Window
    resources.qrc
    Window.cpp
    ../../components/Button.cpp  
    ../../components/SmoothOpacity.cpp  
    ../../components/SmoothShadow.cpp  
)

target_link_libraries(Window PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core dwmapi)

