// Includes

#include "create-loc-history.hpp"

#include <git2.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <ctime>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


// Definitions

#define OBJECTS_PCT 0.1
#define DELTAS_PCT 0.05
#define COMMITS_PCT 0.85

#define SETUP_STR "Setup..."
#define OBJECTS_STR "Cloning: Receiving Objects..."
#define DELTAS_STR "Cloning: Resolving Deltas..."
#define COMMITS_STR "Processing Commits..."


// Language Variables

const Language python = Language("Python", {"py"}, "#", {"\"\"\"", "\"\"\""});
const Language java = Language("Java", {"java"});
const Language html = Language("HTML", {"html"}, "", {"<!--", "-->"});
const Language css = Language("CSS", {"css"}, "", {"/*", "*/"});
const Language javascript = Language("JavaScript", {"js"});
const Language typescript = Language("TypeScript", {"ts"});
const Language c = Language("C", {"c", "h"});
const Language cpp = Language("C++", {"cpp", "hpp"});
const Language c_sharp = Language("C#", {"cs"});
const Language go = Language("Go", {"go"});
const Language rust = Language("Rust", {"rs"});
const Language shell = Language("Shell", {"sh", "bash"}, "#", {"", ""});
const array<Language, 12> languages = {
    python, java, html, css, javascript, typescript, c, cpp, c_sharp, go, rust, shell
};


// Language < Operator

bool operator==(const Language &a, const Language &b) {
    return a.name.compare(b.name) == 0;
}
bool operator<(const Language &a, const Language &b) {
    return a.name < b.name;
}


// Functions

vector<Commit> create_loc_history(
    string git_repo_path, vector<string> excluded_paths, const bool cloning,
    function<void(int, clock_t)> on_progress,
    function<void(string, clock_t)> on_section_change,
    const clock_t start
) {

    if (on_section_change != nullptr)
        on_section_change(SETUP_STR, start);

    vector<Commit> commits = {};

    // Repository Setup

    git_libgit2_init();
    git_repository *repo = nullptr;
    filesystem::path repo_path;

    // Get Repository Name

    if (cloning) {

        // git_repo_path is a URL

        string repo_name = git_repo_path.substr(git_repo_path.rfind('/') + 1);
        if (repo_name.rfind(".git") == repo_name.length() - 4)
            repo_name = repo_name.substr(0, repo_name.length() - 4);

        repo_path = "/tmp/git-loc-history/" + repo_name +
            to_string(
                chrono::duration_cast<chrono::milliseconds>
                (chrono::system_clock::now().time_since_epoch()).count()
            );
        filesystem::remove_all(repo_path);
        filesystem::create_directories(repo_path);
        int error;

        if (on_progress != nullptr) {

            class Captures {
                public:
                    Captures(
                        function<void(int, clock_t)> on_progress,
                        function<void(string, clock_t)> on_section_change,
                        const clock_t start
                    ) : on_progress(on_progress), on_section_change(on_section_change),
                    start(start) {}
                    function<void(int, clock_t)> on_progress;
                    function<void(string, clock_t)> on_section_change;
                    const clock_t start;
            };
            Captures captures = Captures(on_progress, on_section_change, start);

            auto progress_callback = [](const git_transfer_progress *stats, void *payload) -> int {

                static Captures captures = *static_cast<Captures *>(payload);
                static function<void(double, clock_t)> on_progress = captures.on_progress;
                static function<void(string, clock_t)> on_section_change =
                    captures.on_section_change;
                static clock_t start = captures.start;

                static bool notified_objects = false;
                static bool notified_deltas = false;

                if (stats->total_objects > 0) {
                    on_progress(int(round(
                        OBJECTS_PCT * stats->received_objects / stats->total_objects * 100
                    )), start);
                    if (!notified_objects) {
                        notified_objects = true;
                        on_section_change(OBJECTS_STR, start);
                    }
                } else {
                    on_progress(int(round(
                        (OBJECTS_PCT + DELTAS_PCT * stats->indexed_deltas / stats->total_deltas) *
                        100
                    )), start);
                    if (!notified_deltas) {
                        notified_deltas = true;
                        on_section_change(DELTAS_STR, start);
                    }
                }

                return 0;

            };

            git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
            opts.fetch_opts.callbacks.transfer_progress =
                static_cast<git_transfer_progress_cb>(progress_callback);
            opts.fetch_opts.callbacks.payload = &captures;

            error = git_clone(&repo, git_repo_path.c_str(), repo_path.c_str(), &opts);

        } else {

            error = git_clone(&repo, git_repo_path.c_str(), repo_path.c_str(), nullptr);

        }

        if (error != 0) {
            const git_error *e = git_error_last();
            throw runtime_error(
                "git_clone error " +
                to_string(error) + "/" + to_string(e->klass) + ": " + e->message
            );
        }

    } else {

        // git_repo_path is a filesystem path

        if (git_repo_path[0] != '/' && git_repo_path[0] != '~') {
            git_repo_path = filesystem::current_path().string() + '/' + git_repo_path;
        }

        repo_path = git_repo_path;

        int error = git_repository_open(&repo, repo_path.c_str());
        if (error != 0) {
            const git_error *e = git_error_last();
            throw runtime_error(
                "git_repository_open error " +
                to_string(error) + "/" + to_string(e->klass) + ": " + e->message
            );
        }

    }

    // Get Commit History

    git_checkout_head(repo, nullptr);

    git_revwalk *repo_walker = nullptr;
    git_oid oid;
    git_commit *git_commit = nullptr;
    git_revwalk_new(&repo_walker, repo);
    git_revwalk_push_head(repo_walker);

    int total_commits = 0;
    int commits_processed = 0;
    if (on_progress != nullptr) {
        while (git_revwalk_next(&oid, repo_walker) == 0)
            if (git_commit_lookup(&git_commit, repo, &oid) == 0)
                total_commits++;
        git_revwalk_push_head(repo_walker);
        on_section_change(COMMITS_STR, start);
    }

    Commit *prev_commit_ptr = nullptr;

    // File Processing Function

    function<void(const filesystem::path&, Commit&)> process_files_recursive =
    [&process_files_recursive, &excluded_paths, &git_repo_path, &prev_commit_ptr]
    (const filesystem::path &base_path, Commit &commit) {

        for (const filesystem::directory_entry &entry : filesystem::directory_iterator(base_path)) {

            filesystem::path path = entry.path();

            // Check if Path in Excluded Paths

            bool excluded = false;
            for (const string &exc_path : excluded_paths) {
                if (exc_path[0] == '/') {
                    string rel_path = exc_path;
                    if (rel_path.replace(0, 1, git_repo_path).compare(path) == 0) {
                        excluded = true;
                        break;
                    }
                } else if (path.string().find(exc_path) != string::npos) {
                    excluded = true;
                    break;
                }
            }
            if (excluded) continue;

            // Check if Path is File or Directory

            if (filesystem::is_regular_file(path)) {

                // Parse File

                string ext = path.extension().string();
                if (ext.length() > 0 && ext[0] == '.') ext = ext.substr(1);

                for (Language lang : languages) {
                    if (find(lang.ext.begin(), lang.ext.end(), ext) != lang.ext.end()) {

                        // Create File

                        File file = File(path, lang);

                        // Read File Contents

                        ifstream f(filesystem::absolute(path));
                        if (!f.is_open()) break;
                        string contents(
                            (istreambuf_iterator<char>(f)), istreambuf_iterator<char>()
                        );

                        file.contents = contents;

                        // Search for File in Previous Commit

                        if (prev_commit_ptr != nullptr) {
                            bool found = false;
                            for (const File &prev_file : prev_commit_ptr->files) {
                                if (
                                    prev_file.path.compare(file.path) == 0 &&
                                    prev_file.contents.compare(file.contents) == 0
                                ) {
                                    file.lines = prev_file.lines;
                                    commit.lines += file.lines;
                                    commit.language_map[lang] += file.lines;
                                    commit.files.push_back(file);
                                    found = true;
                                    break;
                                }
                            }
                            if (found) break;
                        }

                        // Remove Indents and Newlines

                        auto replace_str = [&contents](string old_str, string new_str) {
                            size_t pos = contents.find(old_str);
                            while (pos != string::npos) {
                                contents.replace(pos, old_str.length(), new_str);
                                pos = contents.find(old_str);
                            }
                        };

                        replace_str("    ", "");
                        replace_str("\r", "\n");
                        replace_str("\n\n", "\n");

                        // Remove Single-Line Comments

                        if (!lang.short_comment.empty()) {
                            string new_contents = "";
                            istringstream stream(contents);
                            string line;
                            while (getline(stream, line)) {
                                if (
                                    line.substr(0, lang.short_comment.length())
                                    .compare(lang.short_comment) == 0
                                ) {
                                    continue;
                                }
                                for (const char &c : line) {
                                    if (c != ' ') {
                                        new_contents += line + "\n";
                                        break;
                                    }
                                }
                            }
                            contents = new_contents;
                        }

                        // Remove Multi-Line Comments

                        if (!lang.long_comment[0].empty()) {
                            size_t pos_start = contents.find(lang.long_comment[0]);
                            size_t pos_end = contents.find(
                                lang.long_comment[1], pos_start + lang.long_comment[0].length()
                            );
                            while (pos_start != string::npos && pos_end != string::npos) {
                                contents = contents.substr(0, pos_start) +
                                    contents.substr(pos_end + lang.long_comment[1].length());
                                pos_start = contents.find(lang.long_comment[0]);
                                pos_end = contents.find(
                                    lang.long_comment[1], pos_start + lang.long_comment[0].length()
                                );
                            }
                        }

                        // Finish Cleaning File

                        replace_str("\n\n", "\n");

                        while (contents.length() != 0 && contents[0] == '\n') {
                            contents = contents.substr(1);
                        }
                        while (contents.length() != 0 && contents[contents.length() - 1] == '\n') {
                            contents = contents.substr(0, contents.length() - 1);
                        }

                        // Calculate File Lines

                        size_t lines = 1;
                        for (const char &c : contents) {
                            if (c == '\n') lines++;
                        }
                        file.lines = lines;
                        commit.lines += lines;
                        commit.language_map[lang] += lines;

                        // Add File to Commit

                        commit.files.push_back(file);

                        break;

                    }
                }

            } else if (filesystem::is_directory(path) && path.extension().string() != ".git") {
                process_files_recursive(path, commit);
            }

        }

    };

    int prev_progress = cloning ? (OBJECTS_PCT + DELTAS_PCT) * 100 : 0;

    while (git_revwalk_next(&oid, repo_walker) == 0) {

        if (git_commit_lookup(&git_commit, repo, &oid) == 0) {

            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &oid);

            Commit commit = Commit(
                oid_str, git_commit_message(git_commit), git_commit_time(git_commit)
            );
            git_tree *commit_tree = nullptr;

            if (git_commit_tree(&commit_tree, git_commit) == 0 && commit_tree != nullptr) {

                // Checkout Commit

                git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
                opts.checkout_strategy = GIT_CHECKOUT_FORCE;
                int error = git_checkout_tree(repo, (const git_object *)commit_tree, &opts);
                if (error != 0) {
                    const git_error *e = git_error_last();
                    throw runtime_error(
                        "git_checkout_tree error " +
                        to_string(error) + "/" + to_string(e->klass) + ": " + e->message
                    );
                }

                // Process Files

                if (commits.size() > 0) prev_commit_ptr = &(commits[commits.size() - 1]);
                process_files_recursive(repo_path, commit);

                git_tree_free(commit_tree);

            }

            // Erase Extra Language Map Keys

            for (const Language &lang : languages)
                if (commit.language_map[lang] == 0) commit.language_map.erase(lang);

            commits.push_back(commit);
            git_commit_free(git_commit);

            if (on_progress != nullptr) {
                commits_processed++;
                double progress_dbl = double(commits_processed) / total_commits;
                if (cloning) progress_dbl = OBJECTS_PCT + DELTAS_PCT + COMMITS_PCT * progress_dbl;
                int progress = int(round(progress_dbl * 100));
                if (progress != prev_progress) {
                    prev_progress = progress;
                    on_progress(progress, start);
                }
            }

        }

    }

    git_revwalk_free(repo_walker);
    git_repository_free(repo);
    git_libgit2_shutdown();

    if (cloning)
        filesystem::remove_all(repo_path);

    return commits;

}
