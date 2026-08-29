cmake_minimum_required(VERSION 3.25)
project(git-loc-history VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_SKIP_RPATH TRUE)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts QuickWidgets REQUIRED)
qt_standard_project_setup()

find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(git2 REQUIRED)
include_directories(${GIT2_INCLUDE_DIR})

qt_add_executable(git-loc-history
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
    git-loc-history PRIVATE ${GIT2_LIBRARIES} z ${OPENSSL_LIBRARIES} Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts Qt6::QuickWidgets
)
target_link_libraries(git-loc-history-cli PRIVATE ${GIT2_LIBRARIES} z ${OPENSSL_LIBRARIES})
