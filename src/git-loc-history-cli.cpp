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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


// Definitions

#define VERSION "0.0.0"


// Functions


// Main Function

int main(int argc, char *argv[]) {

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
            "\t-h, --help                 Display this help and exit\n"
        << endl;
    };

    if (argc < 2) {
        cout << "Missing required argument <git_repo_path>." << endl;
        print_usage();
        return 1;
    }

    string git_repo_path;
    vector<string> excluded_paths;
    char last_flag = 'N';
    // N: None

    for (int i = 1; i < argc; i++) {

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

                if (arg_len > 2) {
                    // Multiple flags at once (e.g. -xX) are invalid as x and X require an argument
                    cout << "Invalid flags." << endl;
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
                        cout << VERSION << endl;
                        return 0;
                    case 'h':
                        print_help();
                        return 0;
                    default:
                        cout << "Unknown flag " << arg[1] << "." << endl;
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
                    ifstream file(arg);
                    if (!file.is_open()) {
                        cout << "Error opening file " << arg << "." << endl;
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
        cout << "Missing required argument <git_repo_path>." << endl;
        return 1;
    }

    cout << "Path: " << git_repo_path << endl;
    for (int i = 0; i < excluded_paths.size(); i++) {
        cout << excluded_paths[i] << endl;
    }

    return 0;

}