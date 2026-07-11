/*
Copyright (C) 2026 Liam Ralph
https://github.com/liam-ralph

This program, including this file, is licensed under the
MIT/Expat License. See LICENSE or this project's source
for more information.
Project Source: https://github.com/liam-ralph/git-loc-history

Git LoC History - An application for view a git repo's history
in lines of code.
*/


// Includes

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Functions


// Main Function

int main(int argc, char *argv[]) {

    // Parse Args

    class Flag {
        public:
            char flag;
            bool takesArg;
            string arg;
            int execute_flag() {

            }
    };

    if (argc == 0) {
        cout << "Missing required argument <git_repo_path>:"
            "A path (local or url) of the git repo to use." << endl;
        return 1;
    }

    string path;
    vector<Flag> flags;

    for (int i = 0; i < argc; i++) {

        string arg = argv[i];

        if (arg[0] == '-') {

            char flag;
            if (arg[1] == '-') {

            } else {

            }

        } else {

            if (flags.size() == 0 || !flags.back().takesArg) {

            }

        }
    }

    return 0;

}