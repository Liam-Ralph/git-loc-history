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

#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

#include "create-loc-history.hpp"


// Definitions

#define README_PATH "../README.md" // "/usr/share/doc/git-loc-history/README.md"


// Functions

void progress_tracker(array<int, 6> *progress_ptr, bool cloning) {

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int columns = min(int(w.ws_col), 50);

    system("clear");
    cout << "Setup..." <<  endl;
    for (int i = 0; i < columns; i++) cout << "▒";
    int empty_bars = columns;
    int section = 0;

    while (true) {

        double progress_pct;
        if (cloning) {
            progress_pct =
                0.2 * (*progress_ptr)[0] / (*progress_ptr)[1] +
                0.1 * (*progress_ptr)[2] / (*progress_ptr)[3] +
                0.7 * (*progress_ptr)[4] / (*progress_ptr)[5];
        } else progress_pct = (*progress_ptr)[4] / (*progress_ptr)[5];

        int new_section;
        if ((*progress_ptr)[0] > 0) new_section = 1;
        else if ((*progress_ptr)[2] > 0) new_section = 2;
        else if ((*progress_ptr)[4] > 0) new_section = 3;
        else new_section = 0;

        if (new_section != section) {
            cout << "\x1b[s";
            cout << "\x1b[1;1H";
            if (new_section == 1) cout << "Cloning: Receiving Objects...";
            else if (new_section == 2) cout << "Cloning: Resolving Deltas...";
            else if (new_section == 3) cout << "Processing Commits...";
            cout << "\x1b[u";
        }

        int new_empty_bars = int(round((1 - progress_pct) * columns));
        if (new_empty_bars != empty_bars) {
            cout << "\x1b[s";
            cout << "\x1b["<< empty_bars <<"D";
            for (int i = 0; i < empty_bars - new_empty_bars; i++) cout << "█";
            for (int i = 0; i < new_empty_bars; i++) cout << "▒";
            cout << "\x1b[u";
        }

        if ((*progress_ptr)[4] == (*progress_ptr)[5]) break;
        section = new_section;
        empty_bars = new_empty_bars;

        this_thread::sleep_for(chrono::milliseconds(100));

    }

}


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

    string git_repo_path; // Path (filesystem or url) passed by user
    vector<string> excluded_paths;

    array<int, 6> progress;
    array<int, 6> *progress_ptr = NULL;

    struct option flag_options[] {
        {"exclude", required_argument, 0, 'x'},
        {"exclude-from", required_argument, 0, 'X'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'}
    };

    int option_index = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "x:X:pvh", flag_options, &option_index)) != -1) {
        switch (opt) {
            case 'x':
                excluded_paths.push_back(optarg);
                break;
            case 'X': {
                string abs_arg = optarg;
                if (abs_arg[0] == '/' || abs_arg[0] == '~') {
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
            case 'p': {
                progress = {0, 1, 0, 1, 0, 1};
                progress_ptr = &progress;
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
            case '?':
                cout <<
                    "Usage: git-loc-history-cli <git_repo_path>\n"
                    "    [-x, --exclude <path>] [-X, --exclude-from <file>] [-v] [-h]\n"
                    "More information can be found using --help."
                << endl;
                return 1;
        }
    }

    git_repo_path = argv[optind];

    // Create LoC History

    vector<Commit> commits;

    thread t([]{});
    if (progress_ptr != NULL)
        t = thread(progress_tracker, progress_ptr, git_repo_path.substr(0, 4).compare("http") == 0);

    try {
        commits = create_loc_history(git_repo_path, excluded_paths, progress_ptr);
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }

    if (progress_ptr != NULL)
        t.join();

    return 0;

}
