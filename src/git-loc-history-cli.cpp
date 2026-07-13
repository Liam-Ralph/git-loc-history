/*
Copyright (C) 2026 Liam Ralph
https://github.com/liam-ralph

This program, including this file, is licensed under the
MIT/Expat License. See LICENSE or this project's source
for more information.
Project Source: https://github.com/liam-ralph/git-loc-history

Git LoC History - An application for view a git repo's lines of
code across its history.
*/


// Includes

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "create-loc-history.hpp"


// Definitions

#define README_PATH "../README.md" // "/usr/share/doc/git-loc-history/README.md"


// Functions


// Main Function

int main(int argc, char *argv[]) {

    // Get Version

    ifstream file(README_PATH);
    if (!file.is_open()) {
        cerr << "Error opening file " << README_PATH << "." << endl;
        return 1;
    }
    string line;
    for (int i = 0; i < 3; i++) getline(file, line);
    const string version = line.substr(12);
    file.close();

    // Parse Args

    auto print_usage = []() {
        cout <<
            "Usage: git-loc-history-cli <git_repo_path>\n"
            "    [-x, --exclude <path>] [-X, --exclude-from <file>] [-v] [-h]\n"
            "More information can be found using --help."
        << endl;
    };

    auto print_help = []() {
        cout <<
            "Usage: git-loc-history-cli <git_repo_path>\n"
            "    [-x, --exclude <path>] [-X, --exclude-from <file>] [-v] [-h]\n\n"
            "Display a git repo's lines of code across its history.\n\n"
            "\t-x, --exclude=<path>       Exclude <path> from results\n"
            "\t-X, --exclude-from=<file>  Exclude all paths in <file> from results\n"
            "\t-v, --version              Print version and exit\n"
            "\t-h, --help                 Display this help and exit\n\n"
            "<git_repo_path> must be a url or a path to a local folder.\n"
            "<path> format follows .gitignore format.\n"
            "Paths are not absolute by default (e.g. foo will exclude both /foo and /bar/foo)."
            "Absolute paths (e.g. /foo) are relative to repository path."
        << endl;
    };

    if (argc < 2) {
        cerr << "Missing required argument <git_repo_path>." << endl;
        print_usage();
        return 1;
    }

    string git_repo_path; // Path (filesystem or url) passed by user
    vector<string> excluded_paths;
    char last_flag = 'N';
    // N: None

    for (int i = 1; i < argc; i++) {

        string arg = argv[i];
        int arg_len = arg.length();

        if (arg[0] == '-') {

            if (arg_len < 2) {
                cerr << "Missing flag." << endl;
                print_usage();
                return 1;
            }

            if (last_flag == 'x' || last_flag == 'X') {
                cerr << "Missing required argument for exclusion." << endl;
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
                    cout << version << endl;
                    return 0;
                } else if (arg.compare("--help") == 0) {
                    print_help();
                    return 0;
                } else {
                    if (arg_len > 2) cerr << "Unknown flag " << arg.substr(2) << "." << endl;
                    else cerr << "Unknown flag." << endl;
                    print_usage();
                    return 1;
                }

            } else {

                // Short Flag

                if (arg.find('v') != string::npos) {
                    cout << version << endl;
                    return 0;
                } else if (arg.find('h') != string::npos) {
                    print_help();
                    return 0;
                }

                if (arg_len > 2) {
                    // Multiple flags at once (e.g. -xX) are invalid as x and X require an argument
                    cerr << "Invalid flags." << endl;
                    print_usage();
                    return 1;
                }

                switch (arg[1]) {
                    case 'x':
                        last_flag = arg[1];
                        break;
                    case 'X':
                        last_flag = arg[1];
                        break;
                    case 'v':
                        cout << version << endl;
                        return 0;
                    case 'h':
                        print_help();
                        return 0;
                    default:
                        cerr << "Unknown flag " << arg[1] << "." << endl;
                        print_usage();
                        return 1;
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
                    string abs_arg = arg;
                    if (abs_arg.rfind("/", 0) == 0 || abs_arg.rfind("~", 0) == 0) {
                        abs_arg = filesystem::current_path().string() + abs_arg;
                    }
                    ifstream file(abs_arg);
                    if (!file.is_open()) {
                        cerr << "Error opening file " << arg << "." << endl;
                        if (abs_arg.compare(arg) != 0) {
                            cerr << "Used " << abs_arg << " for " << arg << "." << endl;
                        }
                        return 1;
                    }
                    string line;
                    while (getline(file, line)) {
                        excluded_paths.push_back(line);
                    }
                    file.close();
                    break;
            }
            last_flag = 'N';

        }
    }

    if (git_repo_path.empty()) {
        cerr << "Missing required argument <git_repo_path>." << endl;
        return 1;
    }

    // Create LoC History

    try {
        vector<Commit> result = create_loc_history(git_repo_path);
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
    }

    return 0;

}