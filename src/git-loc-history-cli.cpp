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
#include <getopt.h>
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

    string git_repo_path; // Path (filesystem or url) passed by user
    vector<string> excluded_paths;

    struct option flag_options[] {
        {"exclude", required_argument, 0, 'x'},
        {"exclude-from", required_argument, 0, 'X'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'}
    };

    int option_index = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "x:X:vh", flag_options, &option_index)) != -1) {
        switch (opt) {
            case 'x':
                excluded_paths.push_back(optarg);
                break;
            case 'X': {
                string abs_arg = optarg;
                if (abs_arg.rfind("/", 0) == 0 || abs_arg.rfind("~", 0) == 0) {
                    abs_arg = filesystem::current_path().string() + abs_arg;
                }
                ifstream file(abs_arg);
                if (!file.is_open()) {
                    cerr << "Error opening file " << optarg << "." << endl;
                    if (abs_arg.compare(optarg) != 0) {
                        cerr << "Used " << abs_arg << " for " << optarg << "." << endl;
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
            case 'v':
                cout << version << endl;
                return 0;
            case 'h':
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
                    "Paths are not absolute by default (e.g. foo will exclude /foo and /bar/foo).\n"
                    "Absolute paths (e.g. /foo) are relative to repository path."
                << endl;
                return 0;
        }
    }

    git_repo_path = argv[optind];

    // Create LoC History

    try {
        vector<Commit> result = create_loc_history(git_repo_path);
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
    }

    return 0;

}