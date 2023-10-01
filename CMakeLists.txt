cmake_minimum_required(VERSION 3.26)
project(qt_dog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(ZXing)

message(${ZXing_DIR})
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(qt_dog src/main.cpp
        src/QRDoge.h
        src/QRDoge.cpp
        src/HoverButton.cpp
        src/HoverButton.h
        src/DogeThread.h
        src/DogeThread.cpp)
target_link_libraries(qt_dog
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ZXing
)

