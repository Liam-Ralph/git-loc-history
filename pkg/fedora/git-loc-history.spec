Name: git-loc-history
Version: VERSION
Release: %{?dist}
Packager: Liam Ralph <liamralph@tutamail.com>
Summary: An application for view a git repo's lines of code across its history.

License: MIT
URL: https://github.com/liam-ralph/git-loc-history
Requires: qt6-qtbase >= 6.5, qt6-qtcharts >= 6.5
Source0: %{name}-%{version}.tar.gz

%description
An application for viewing a git repo's lines of code across its history.

%global debug_package %{nil}

%prep
%autosetup

%install
install -Dm755 usr/bin/git-loc-history $RPM_BUILD_ROOT/usr/bin/git-loc-history
install -Dm755 usr/bin/git-loc-history-cli $RPM_BUILD_ROOT/usr/bin/git-loc-history-cli
install -Dm644 usr/share/applications/git-loc-history.desktop \
    $RPM_BUILD_ROOT/usr/share/applications/git-loc-history.desktop
install -Dm644 usr/share/doc/git-loc-history/copyright \
    $RPM_BUILD_ROOT/usr/share/doc/git-loc-history/copyright
install -Dm644 usr/share/doc/git-loc-history/README.md \
    $RPM_BUILD_ROOT/usr/share/doc/git-loc-history/README.md
install -Dm644 usr/share/doc/git-loc-history/CHANGELOG.md \
    $RPM_BUILD_ROOT/usr/share/doc/git-loc-history/CHANGELOG.md
install -Dm644 usr/share/icons/hicolor/512x512/apps/git-loc-history.png \
    $RPM_BUILD_ROOT/usr/share/icons/hicolor/512x512/apps/git-loc-history.png
install -Dm644 usr/share/licenses/git-loc-history/LICENSE \
    $RPM_BUILD_ROOT/usr/share/licenses/git-loc-history/LICENSE

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/bin/git-loc-history
/usr/bin/git-loc-history-cli
/usr/share/applications/git-loc-history.desktop
/usr/share/doc/git-loc-history/copyright
/usr/share/doc/git-loc-history/README.md
/usr/share/doc/git-loc-history/CHANGELOG.md
/usr/share/icons/hicolor/512x512/apps/git-loc-history.png
/usr/share/licenses/git-loc-history/LICENSE

%changelog
%autochangelog
