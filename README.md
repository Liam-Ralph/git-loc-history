# Git LoC History
### Released August 2026
### Version 1.0.1
### Updated August 2026

<br/>

## Description

A Qt GUI app and a CLI app for viewing a git repository's lines of code at each
commit in its history.

<br/>

## Setup

Run the following commands to install libgit2 v1.9.7:
```
wget https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.7.tar.gz
tar -xzf v1.9.7.tar.gz
rm -rf v1.9.7.tar.gz
cd libgit2-1.9.7
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
cd../../
```
libgit2 can be installed as a system package, which should work as well, but
distros are inconsistent with what version they ship.

Below are the required dependencies to build on each distro group. Qt 6.4 or
greater is required.

### Debian (apt)
 - build-essential
 - cmake
 - libssl-dev
 - qt6-base-dev
 - qt6-charts-dev
 - qt6-declarative-dev
 - libgl1 or libgl1-mesa-dev

### Fedora (dnf)
 - rpmdevtools
 - cmake
 - openssl-devel
 - qt6-qtbase-devel
 - qt6-qtcharts-devel

### Arch (pacman)
 - base-devel
 - cmake
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
