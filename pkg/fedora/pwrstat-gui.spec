Name: git-loc-history
Version: 1.0.0
Release: %{?dist}
Packager: Liam Ralph <liamralph@tutamail.com>
Summary: An application for view a git repo's lines of code across its history.

License: MIT
URL: https://github.com/liam-ralph/git-loc-history
BuildRequires: cmake
Source0: https://github.com/liam-ralph/git-loc-history/refs/tags/v%{version}.tar.gz
Source1: https://github.com/libgit2/libgit2/archive/refs/tags/v1.9.6.tar.gz

%description
An application for view a git repo's lines of code across its history.



%changelog
%autochangelog
