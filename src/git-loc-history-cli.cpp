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
#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
using namespace std;

#include "create-loc-history.hpp"


// Definitions

#define README_PATH "../README.md" // "/usr/share/doc/git-loc-history/README.md"


// Functions

string format_time(clock_t start) {
    stringstream ss;
    ss << fixed << setprecision(2) << double(clock() - start) / CLOCKS_PER_SEC;
    return ss.str();
}

void progress_tracker(array<atomic<int>, 6> *progress_ptr, bool cloning, clock_t start) {

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int columns = min(int(w.ws_col), 50);

    system("clear");
    string text = format_time(start) + " Setup...\n";
    for (int i = 0; i < columns; i++) text += "▒";
    cout << text << endl;

    while (true) {

        this_thread::sleep_for(chrono::milliseconds(100));

        double progress_pct;
        if (cloning) {
            progress_pct =
                0.1 * (*progress_ptr)[0] / (*progress_ptr)[1] +
                0.05 * (*progress_ptr)[2] / (*progress_ptr)[3] +
                0.85 * (*progress_ptr)[4] / (*progress_ptr)[5];
        } else progress_pct = (*progress_ptr)[4] / (*progress_ptr)[5];

        text = format_time(start) + " ";
        if ((*progress_ptr)[4] > 0) text += "Processing Commits...\n";
        else if ((*progress_ptr)[2] > 0) text += "Cloning: Resolving Deltas...\n";
        else if ((*progress_ptr)[0] > 0) text += "Cloning: Receiving Objects...\n";
        else text += "Setup...\n";
        int bars = int(round(progress_pct * columns));
        for (int i = 0; i < columns; i++) {
            if (i < bars) text += "█";
            else text += "▒";
        }

        system("clear");
        cout << text << endl;

        if ((*progress_ptr)[4] == (*progress_ptr)[5]) break;

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

    array<atomic<int>, 6> progress;
    array<atomic<int>, 6> *progress_ptr = NULL;

    struct option flag_options[] {
        {"exclude", required_argument, 0, 'x'},
        {"exclude-from", required_argument, 0, 'X'},
        {"progress", no_argument, 0, 'p'},
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
                progress[0] = 0;
                progress[1] = 1;
                progress[2] = 0;
                progress[3] = 1;
                progress[4] = 0;
                progress[5] = 1;
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

    clock_t start = clock();

    vector<Commit> commits;

    unique_ptr<thread> t_ptr;
    if (progress_ptr != NULL)
        t_ptr = unique_ptr<thread>(
            new thread(
                progress_tracker,
                progress_ptr, git_repo_path.substr(0, 4).compare("http") == 0, start
            )
        );

    try {
        commits = create_loc_history(git_repo_path, excluded_paths, progress_ptr);
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }

    if (progress_ptr != NULL)
        (*t_ptr).join();

    string elapsed_time = format_time(start);

    // Create Graph

    // Get Terminal Dimensions

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = int(w.ws_col);
    int height = int(w.ws_row) - 5;

    // Define Language Text Colors

    map<Language, string> language_colors = {
        {python, "20"}, {java, "124"}, {html, "208"}, {css, "129"},
        {javascript, "11"}, {typescript, "27"}, {c, "19"}, {cpp, "18"}, {c_sharp, "17"},
        {go, "39"}, {rust, "202"}, {shell, "248"}
    };

    // Create Graph Bars

    int max_lines = 0;
    for (const Commit &commit : commits) if (commit.lines > max_lines) max_lines = commit.lines;

    vector<vector<string>> graph_bars(commits.size(), vector<string>());
    array<string, 7> block_chars = {"▁", "▂", "▃", "▄", "▅", "▆", "▇"};

    for (int i = 0; i < commits.size(); i++) {

        // Get Commit and Bar

        const Commit &commit = commits[i];
        vector<string> &graph_bar = graph_bars[i];

        // Create Bar Heights

        map<Language, int> bar_heights;
        for (auto &[lang, lines] : commit.language_map)
            bar_heights.emplace(lang, int(round(double(lines) / max_lines * height * 8)));
        const map<Language, int> bar_heights_const = bar_heights;

        // Create Each Block of Bar

        for (int ii = 0; ii < height; ii++) {

            // Get Parts of Block

            map<Language, int> block_parts;
            int iii = 0;
            for (auto &[lang, lines] : bar_heights) {
                if (lines == 0) continue;
                int part = min(lines, 8 - iii);
                lines -= part;
                iii += part;
                block_parts.emplace(lang, part);
                if (iii == 8) break;
            }

            // Create Block

            if (block_parts.size() == 0) break;

            const map<Language, int>::iterator first_block_part = block_parts.begin();

            if (block_parts.size() == 1) {

                // Block Has One Color
        
                if (first_block_part->second == 8)
                    // Full Block of One Color
                    graph_bar.push_back(
                        "\u001b[48;5;" + language_colors[first_block_part->first] + "m "
                    );

                else
                    // Partial Block of One Color, Top of Bar
                    graph_bar.push_back(
                        "\u001b[m\u001b[38;5;" + language_colors[first_block_part->first] + "m" +
                        block_chars[first_block_part->second - 1]
                    );
                    break;

            } else if (block_parts.size() == 2) {
                // Block Has Two Colors
                graph_bar.push_back(
                    "\u001b[38;5;" + language_colors[first_block_part->first] + "m" +
                    block_chars[first_block_part->second - 1] +
                    "\u001b[48;5;" + language_colors[block_parts.rbegin()->first] + "m "
                );

            } else {

                // Block Has Three or More Colors
                // A character can only have two colors, fore- and background

                // Get Largest Two Block Parts

                map<Language, int>::iterator largest = block_parts.begin();
                map<Language, int>::iterator largest2 = next(block_parts.begin());
                bool largest_first = true;
                if (largest->second < largest2->second) {
                    largest = next(block_parts.begin());
                    largest2 = block_parts.begin();
                    largest_first = false;
                }
                for (
                    map<Language, int>::iterator block_part = next(block_parts.begin(), 2);
                    block_part != block_parts.end(); block_part++
                ) {
                    if (block_part->second <= largest2->second) continue;
                    if (block_part->second > largest->second) {
                        largest2 = largest;
                        largest = block_part;
                        largest_first = true;
                    } else if (block_part->second > largest2->second) {
                        largest2 = block_part;
                        largest_first = false;
                    }
                }

                // Get First (Bottom) and Second Block Part Languages

                const Language *first_lang;
                const Language *second_lang;
                if (largest_first) {
                    first_lang = &(largest->first);
                    second_lang = &(largest2->first);
                } else {
                    first_lang = &(largest2->first);
                    second_lang = &(largest->first);
                }

                // Create Block

                int first_og_lines = bar_heights_const.at(*first_lang);
                int total_og_lines = first_og_lines + bar_heights_const.at(*second_lang);
                int first_char = int(round(double(first_og_lines) / (total_og_lines) * 8));

                if (first_char < 8)
                    graph_bar.push_back(
                        "\u001b[38;5;" + language_colors[*first_lang] + "m" +
                        block_chars[first_char - 1] +
                        "\u001b[48;5;" + language_colors[*second_lang] + "m "
                    );
                else
                    graph_bar.push_back(
                        "\u001b[48;5;" + language_colors[*first_lang] + "m "
                    );

            }

        }

    }

    // Get Most Recent Commit Statistics

    const Commit &last_commit = commits[0];
    string last_commit_line = "Last Commit: " + to_string(last_commit.lines) + " LoC";
    for (auto &[lang, lines] : last_commit.language_map) {
        stringstream ss;
        ss << fixed << setprecision(1) << double(lines) / last_commit.lines * 100;
        last_commit_line += ", " + lang.name + ": " + to_string(lines) + " LoC (" + ss.str() + "%)";
    }
    if (last_commit_line.length() > width)
        last_commit_line = last_commit_line.substr(0, width - 3) + "...";

    // Print Graph

    if (git_repo_path.length() > width)
        git_repo_path = git_repo_path.substr(git_repo_path.length() - width + 3) + "...";

    if (graph_bars.size() > width) {

    } else {
        system("clear");
        // for (int i = 0; i < height; i++) {
        //     for (int ii = 0; ii < graph_bars.size(); ii++) {
        //         if (graph_bars[ii].size() > i)
        //             cout << graph_bars[ii][i];
        //         else cout << graph_bars[ii].size() << " ";
        //     }
        //     cout << "\n";
        // }
        cout << git_repo_path << "\n";
        cout << last_commit_line << "\n";
        for (int i = height - 1; i >= 0; i--) {
            for (int ii = graph_bars.size() - 1; ii >= 0; ii--) {
                if (graph_bars[ii].size() > i)
                    cout << graph_bars[ii][i];
                else cout << "\u001b[m ";
            }
            cout << "\u001b[m\n";
        }
        flush(cout);
    }

    return 0;

}
