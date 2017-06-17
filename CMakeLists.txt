cmake_minimum_required(VERSION 3.2)
project(qmdown)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebEngine REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

include_directories(include)

add_executable(qmdown
  src/main.cpp
  src/qmdown.ui
  include/qmdown.h
  src/qmdown.cpp
  res/qmdown.qrc)

target_link_libraries(qmdown Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets)
