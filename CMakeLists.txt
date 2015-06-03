cmake_minimum_required(VERSION 3.0)
project(voltmeter)

# Setup Qt5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets)
find_package(Qt5PrintSupport)
find_package(Qt5Network)

add_definitions(-std=c++11)

set(SOURCE_FILES
    main.cpp
    lib/qcustomplot.cpp
    Measurement.hpp
    GaugePlot.cpp
    ClientSocket.cpp
    ChannelPanel.cpp
    ConnectionPanel.cpp
    MainWindow.cpp)
add_executable(voltmeter ${SOURCE_FILES})

target_link_libraries(voltmeter Qt5::Widgets Qt5::PrintSupport Qt5::Network)