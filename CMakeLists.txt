cmake_minimum_required(VERSION 4.4)
project(git-loc-history)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(libgit2-1.9.6)
# Taken from https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.6.tar.gz

find_package(Qt6 COMPONENTS Core Gui Widgets Graphs REQUIRED)

add_executable(git-loc-history
    src/git-loc-history.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/create-loc-history.cpp
    src/create-loc-history.hpp
)
add_executable(git-loc-history-cli
    src/git-loc-history-cli.cpp
    src/create-loc-history.cpp
    src/create-loc-history.hpp
)

target_link_libraries(git-loc-history git2 Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Graphs)
target_link_libraries(git-loc-history-cli git2)