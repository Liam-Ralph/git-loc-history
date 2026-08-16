cmake_minimum_required(VERSION 3.25)
project(git-loc-history)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(LIBGIT2_USE_STATIC_LIBS ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts QuickWidgets REQUIRED)
find_package(libgit2 REQUIRED)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(git-loc-history
    src/git-loc-history.cpp
    src/definitions.cpp
    src/definitions.hpp
    src/mainwindow.cpp
    src/mainwindow.hpp
    src/infowindow.cpp
    src/infowindow.hpp
    src/create-loc-history.cpp
    src/create-loc-history.hpp
)
add_executable(git-loc-history-cli
    src/git-loc-history-cli.cpp
    src/definitions.cpp
    src/definitions.hpp
    src/create-loc-history.cpp
    src/create-loc-history.hpp
)

target_link_libraries(
    git-loc-history git2 Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts Qt6::QuickWidgets
)
target_link_libraries(git-loc-history-cli git2)