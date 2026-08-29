# Git LoC History
### Released August 2026
### Version 1.1.0
### Updated August 2026

<br/>

## Description

A Qt GUI app and a CLI app for viewing a git repository's lines of code at each
commit in its history.

<br/>

## Setup

### libgit2

Run the following commands to build libgit2 v1.9.7:
```
wget https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.7.tar.gz
tar -xzf v1.9.7.tar.gz
rm -f v1.9.7.tar.gz
cd libgit2-1.9.7
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DREGEX_BACKEND=builtin ..
cmake --build . --parallel $(nproc)
cd ../../
```

### Dependencies

Below are the required dependencies to build on each distro group. Qt 6.4 or
greater is required. This is just every package I had to install on each distro
I tried, so it may not be a comprehensive list.

#### Debian (apt)
 - build-essential
 - cmake
 - wget
 - pkgconf
 - zlib1g-dev
 - libssl-dev
 - qt6-base-dev
 - qt6-charts-dev
 - qt6-declarative-dev
 - libgl1 or libgl1-mesa-dev

#### Fedora (dnf)
 - rpmdevtools
 - cmake
 - wget
 - pkgconf
 - openssl-devel
 - qt6-qtbase-devel
 - qt6-qtcharts-devel

#### Arch (pacman)
 - base-devel
 - cmake
 - wget
 - pkgconf
 - qt6-base
 - qt6-charts


<br/>

## Installation

Packages can be found on this repository's GitHub page under releases. Qt 6.4
or greater is required.

<br/>

## Packaging

You can use the scripts under pkg to create Linux packages.

<br/>

## Credits

The Git Loc History logo is modified from the
[Git logo](https://git-scm.com/community/logos)
created by Jason Long, and licensed under the
[Creative Commons Attribution 3.0 Unported License](https://creativecommons.org/licenses/by/3.0/).
