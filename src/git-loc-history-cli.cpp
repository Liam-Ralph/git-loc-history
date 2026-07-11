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


// Definitions

#define VERSION "0.0.0"


// Functions

void print_usage() {
    cout <<
        "Usage: git-loc-history-cli <git_repo_path>\n"
        "    [-x, --exclude <pattern>] [-X, --exclude-from <file>] [-v] [-h]\n"
        "More information can be found using --help."
    << endl;
}

void print_help() {}


// Main Function

int main(int argc, char *argv[]) {

    // Parse Args

    if (argc == 0) {
        cout << "Missing required argument <git_repo_path>." << endl;
        print_usage();
        return 1;
    }

    string git_repo_path;
    vector<string> excluded_paths;
    char last_flag = 'N';
    // N: None

    for (int i = 0; i < argc; i++) {

        string arg = argv[i];
        int arg_len = arg.length();

        if (arg[0] == '-') {

            if (arg_len < 2) {
                cout << "Missing flag." << endl;
                print_usage();
                return 1;
            }

            if (last_flag == 'x' || last_flag == 'X') {
                cout << "Missing required argument for exclusion." << endl;
                print_usage();
                return 1;
            }

            if (arg[1] == '-') {

                // Long Flag

                if (arg.compare("--exclude") == 0) {
                    last_flag = 'x';
                } else if (arg.compare("--exclude-from") == 0) {
                    last_flag = 'X';
                } else if (arg.compare("--version") == 0) {
                    cout << VERSION << endl;
                    return 0;
                } else if (arg.compare("--help") == 0) {
                    print_help();
                    return 0;
                } else {
                    if (arg_len > 2) cout << "Unknown flag " << arg.substr(2) << "." << endl;
                    else cout << "Unknown flag." << endl;
                    print_usage();
                    return 1;
                }

            } else {

                // Short Flag

                if (arg.find('v') != string::npos) {
                    cout << VERSION << endl;
                    return 0;
                } else if (arg.find('h') != string::npos) {
                    print_help();
                    return 0;
                }

                if (arg_len != 2) {
                    cout << "Invalid flags." << endl;
                    print_usage();
                    return 1;
                }

                switch (arg[2]) {
                    case 'x' || 'X':
                        last_flag = arg[1];
                        break;
                    case 'v':
                        cout << VERSION << endl;
                        return 0;
                    case 'h':
                        print_help();
                        return 0;
                    default:
                        cout << "Unknown flag " << arg[1] << "." << endl;
                }

            }
        } else {

            // Argument

            switch (last_flag) {
                case 'N':
                    git_repo_path = arg;
                    break;
                case 'x':
                    excluded_paths.push_back(arg);
                    break;
                case 'X':
                    // read file
                    break;
            }
            last_flag = 'N';

        }
    }

    return 0;

}