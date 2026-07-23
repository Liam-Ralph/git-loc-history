/*
Copyright (C) 2026 Liam Ralph
https://github.com/liam-ralph

This program, including this file, is licensed under the
MIT/Expat License. See LICENSE or this project's source
for more information.
Project Source: https://github.com/liam-ralph/git-loc-history

Git LoC History - An application for viewing a git repo's lines of
code across its history.
*/


// Includes

#include <iostream>
#include <QApplication>
using namespace std;

#include "mainwindow.h"
#include "create-loc-history.hpp"


// Definitions

#define README_PATH "../README.md" // "/usr/share/doc/git-loc-history/README.md"


// Functions


// Main Function

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();

}
