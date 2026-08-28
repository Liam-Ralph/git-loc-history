#!/bin/bash

cd ../

# Edit Files

sed -i -e "s/#define RELEASE_PATHS false/#define RELEASE_PATHS true/g" src/definitions.cpp
if [ -e "build" ]; then
    rm -rf build/*
else
    mkdir build
fi

# libgit2

if [ ! -e "libgit2-1.9.7/build/libgit2.so.1.9.7" ]; then
    wget https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.7.tar.gz
    tar -xzf v1.9.7.tar.gz
    rm -f v1.9.7.tar.gz
    cd libgit2-1.9.7
    mkdir build
    cd build
    cmake ..
    cmake --build . --parallel $(nproc)
    cd ../../
fi

# Compile Binaries

cd build
cmake ..
cmake --build . --parallel $(nproc) --config Release
strip git-loc-history git-loc-history-cli
cd ..
sed -i -e "s/#define RELEASE_PATHS true/#define RELEASE_PATHS false/g" src/definitions.cpp

# Build usr Directory

if [ -e "pkg/usr" ]; then
    rm -rf pkg/usr
fi
mkdir pkg/usr
install -Dm755 build/git-loc-history pkg/usr/bin/git-loc-history
install -Dm755 build/git-loc-history-cli pkg/usr/bin/git-loc-history-cli
install -Dm644 pkg/resources/git-loc-history.desktop \
    pkg/usr/share/applications/git-loc-history.desktop
install -Dm644 pkg/resources/copyright pkg/usr/share/doc/git-loc-history/copyright
install -Dm644 src/defaults.conf pkg/usr/share/git-loc-history/defaults.conf
install -Dm644 README.md pkg/usr/share/doc/git-loc-history/README.md
install -Dm644 CHANGELOG.md pkg/usr/share/doc/git-loc-history/CHANGELOG.md
install -Dm644 logo.png pkg/usr/share/icons/hicolor/512x512/apps/git-loc-history.png
install -Dm644 LICENSE pkg/usr/share/licenses/git-loc-history/LICENSE
