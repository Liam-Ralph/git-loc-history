cmake_minimum_required(VERSION 3.25)
project(git-loc-history VERSION 1.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts QuickWidgets REQUIRED)
qt_standard_project_setup()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(git2 REQUIRED)
include_directories(${GIT2_INCLUDE_DIR})

qt_add_executable(git-loc-history
    src/git-loc-history.cpp
    src/definitions.hpp
    src/definitions.cpp
    src/mainwindow.hpp
    src/mainwindow.cpp
    src/infowindow.hpp
    src/infowindow.cpp
    src/worker.hpp
    src/worker.cpp
    src/create-loc-history.hpp
    src/create-loc-history.cpp
)
add_executable(git-loc-history-cli
    src/git-loc-history-cli.cpp
    src/definitions.hpp
    src/definitions.cpp
    src/create-loc-history.hpp
    src/create-loc-history.cpp
)

target_link_libraries(
    git-loc-history PRIVATE ${GIT2_LIBRARIES}
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts Qt6::QuickWidgets
)
target_link_libraries(git-loc-history-cli PRIVATE ${GIT2_LIBRARIES})
