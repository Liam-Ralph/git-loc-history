#!/bin/bash

cd ../

if [ ! -e "libgit2-1.9.6" ]; then
    wget https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.6.tar.gz
    tar -xzf v1.9.6.tar.gz
    rm -f v1.9.6.tar.gz
fi

sed -i -e "s/#define ABSOLUTE_PATHS false/#define ABSOLUTE_PATHS true/g" src/definitions.cpp
if [ -e "build" ]; then
    rm -rf build/*
else
    mkdir build
fi
cd build
cmake ..
cmake --build . --parallel $(nproc) --config Release
strip git-loc-history git-loc-history-cli
cd ..
sed -i -e "s/#define ABSOLUTE_PATHS true/#define ABSOLUTE_PATHS false/g" src/definitions.cpp

mkdir usr
install -Dm755 build/git-loc-history -t pkg/usr/bin/git-loc-history
install -Dm755 build/git-loc-history-cli -t pkg/usr/bin/git-loc-history-cli
install -Dm644 pkg/resources/git-loc-history.desktop -t \
    pkg/usr/share/applications/git-loc-history.desktop
install -Dm644 pkg/resources/copyright -t pkg/usr/share/doc/git-loc-history/copyright
install -Dm644 README.md -t pkg/usr/share/doc/git-loc-history/README.md
install -Dm644 CHANGELOG.md -t pkg/usr/share/doc/git-loc-history/CHANGELOG.md
install -Dm644 logo.png -t pkg/usr/share/icons/hicolor/512x512/git-loc-history.png
install -Dm644 LICENSE -t pkg/usr/share/licenses/git-loc-history/LICENSE

