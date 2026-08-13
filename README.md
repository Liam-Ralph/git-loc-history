# Git LoC History
### Released August 2026
### Version 1.0.0
### Updated August 2026

<br/>

## Description

A Qt GUI app and a CLI app for viewing a git repository's lines of code at each
commit in its history.

<br/>

## Credits

The Git Loc History logo is modified from the [Git logo](https://git-scm.com/community/logos)
created by Jason Long, and licensed under the
[Creative Commons Attribution 3.0 Unported License](https://creativecommons.org/licenses/by/3.0/).

<br/>

## Installation

Packages can be found on this repository's GitHub page under releases. Two
packages are provided for arch

<br/>

## Packaging

You can use the scripts under pkg to create Linux packages. Note that the
regular Arch packages builds from source, so only editing the PKGBUILD is
needed, but scripts can be used to build the git-loc-history-bin package.

Below are the required dependencies to build on each distro goup.

### Debian (apt)
 - cmake
 - build-essential
 - qt6-base-dev
 - qt6-charts-dev
 - qt6-declarative-dev

### Fedore (dnf)


### Arch (pacman)
 - base-devel
 - cmake
 - qt6-base
 - qt6-charts
