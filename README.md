# Git LoC History
### Released August 2026
### Version 1.0.1
### Updated August 2026

<br/>

## Description

A Qt GUI app and a CLI app for viewing a git repository's lines of code at each
commit in its history.

<br/>

## Credits

The Git Loc History logo is modified from the
[Git logo](https://git-scm.com/community/logos)
created by Jason Long, and licensed under the
[Creative Commons Attribution 3.0 Unported License](https://creativecommons.org/licenses/by/3.0/).

<br/>

## Installation

Packages can be found on this repository's GitHub page under releases. Qt 6.4
or greater is required.

<br/>

## Packaging

You can use the scripts under pkg to create Linux packages.

Below are the required dependencies to build on each distro group. Qt 6.4 or
greater is required. At the time of writing, unsupported distros include Debian
12, Ubuntu, and Linux Mint.

### Debian (apt)
 - build-essential
 - cmake
 - libgit2-1.9 (older version seem to work as well)
 - libssl-dev
 - qt6-base-dev
 - qt6-charts-dev
 - qt6-declarative-dev

### Fedora (dnf)
 - rpmdevtools
 - cmake
 - openssl-devel
 - libgit2-devel
 - qt6-qtbase-devel
 - qt6-qtcharts-devel

### Arch (pacman)
 - base-devel
 - cmake
 - libgit2
 - qt6-base
 - qt6-charts
