// Includes

#include "create-loc-history.hpp"
#include "definitions.hpp"

#include <git2.h>

#include <algorithm>
#include <array>
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
#define CACHE_STR "Caching results..."

#define RESULTS_FORMAT_VERSION 1
// Incremented when a breaking change is made to the format used in results caching

#define UPDATE_DELAY 100 // Delay to update progress in milliseconds


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


// Language Operators

bool operator==(const Language &a, const Language &b) {
    return a.name.compare(b.name) == 0;
}
bool operator<(const Language &a, const Language &b) {
    return a.name < b.name;
}


// Functions

/**
 * Get and throw most recent error from git2.
 * 
 * Output includes the error returned by the failed function, and the category
 * and message of the most recent git2 error. Throws a runtime error, which
 * should be caught by the caller of create_loc_history.
 * 
 * @param function_name The name of the function that failed, or some
 * description.
 * @param error The error number returned by the failed function.
 */
void throw_git_error(string function_name, int error) {
    const git_error *e = git_error_last();
    throw runtime_error(
        function_name + " error " +
        to_string(error) + "/" + to_string(e->klass) + ": " + e->message
    );
}

/**
 * Count the lines of code across a given git repository's history.
 * 
 * Clones/open the given repository, then checks out each commit and reads the
 * files present. Constructs an std::vector of Commits to return. Broken down
 * into sections, and runs a given function on section change, or on progress.
 * Can also read and write cache results.
 * 
 * @param git_repo_path The path to the git repo. Can be a URL or filesystem
 * path.
 * @param excluded_paths Paths to be excluded from lines of code calculations.
 * There are two types of paths; absolute paths (e.g. "/foo") are relative
 * to the repository's base, and non-abolute paths (e.g. "foo") are matched
 * to entire paths (i.e. "foo" will match "/foo" and "/bar/foo").
 * @param cloning Whether the path is a URL that requires cloning. Used for
 * to prevent redundancy, since this is usually checked by the caller anyway.
 * @param branch The branch to checkout. An empty string will checkout the
 * default branch.
 * @param cache_results Whether to cache the produced results. Does not impact
 * the functions attempting to read existing cache files during history
 * creation.
 * @param on_progress A function to call when progress is made. This function
 * must accept an int, the current progress 0-100, and a long, the number of
 * milliseconds since epoch from the caller. The long will always be the param
 * start (see below). The function may only be called when the progress int
 * changes.
 * @param on_section_change A function to call when the current section changes.
 * Must accept a string, representing the new section, and a long, start (see
 * previous).
 * @param start Milliseconds since epoch in the calling program. Usually this
 * value will be set right before this function is called.
 */
vector<Commit> create_loc_history(
    string git_repo_path, vector<string> excluded_paths,
    const bool cloning, const string branch, const bool cache_results,
    function<void(int, long)> on_progress,
    function<void(string, long)> on_section_change,
    const long start
) {

    long last_update = start; // Milliseconds since progress last updated

    if (on_section_change != nullptr)
        on_section_change(SETUP_STR, start);

    vector<Commit> commits = {};

    // Repository Setup

    git_libgit2_init();
    git_repository *repo = nullptr;
    filesystem::path repo_path;
    string repo_name = git_repo_path.substr(git_repo_path.rfind('/') + 1);

    // Get Repository Name

    if (cloning) {

        // URL

        // Setup Target Directory

        if (repo_name.rfind(".git") == repo_name.length() - 4)
            repo_name = repo_name.substr(0, repo_name.length() - 4);

        repo_path = "/tmp/git-loc-history/" + repo_name + to_string(Definitions::get_time_ms());
        filesystem::remove_all(repo_path);
        filesystem::create_directories(repo_path);
        int error;

        // Cloning Repository

        if (on_progress != nullptr) {

            // Create Class for Function Captures

            class Captures {
                public:

                    Captures(
                        function<void(int, long)> on_progress,
                        function<void(string, long)> on_section_change,
                        const long start
                    ) : on_progress(on_progress), on_section_change(on_section_change),
                        start(start) {}

                    function<void(int, long)> on_progress;
                    function<void(string, long)> on_section_change;
                    const long start;

            };
            Captures captures = Captures(on_progress, on_section_change, start);

            // Progress Callback Function

            auto progress_callback = [](const git_transfer_progress *stats, void *payload) -> int {

                // Unpack Payload

                static Captures captures = *static_cast<Captures *>(payload);
                static function<void(double, long)> on_progress = captures.on_progress;
                static function<void(string, long)> on_section_change =
                    captures.on_section_change;
                static long start = captures.start;

                // Section Change Notification Tracking

                static bool notified_objects = false;
                static bool notified_deltas = false;

                // Run Progress Function

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

            // Create Clone Options

            git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
            opts.fetch_opts.callbacks.transfer_progress =
                static_cast<git_transfer_progress_cb>(progress_callback);
            opts.fetch_opts.callbacks.payload = &captures;

            // Clone Repository

            error = git_clone(&repo, git_repo_path.c_str(), repo_path.c_str(), &opts);

        } else {

            error = git_clone(&repo, git_repo_path.c_str(), repo_path.c_str(), nullptr);

        }

        if (error != 0) throw_git_error("git_clone", error);

    } else {
        // git_repo_path is a filesystem path

        repo_path = git_repo_path;

        int error = git_repository_open(&repo, repo_path.c_str());
        if (error != 0) throw_git_error("git_repository_open", error);

    }

    // Search Results Cache

    /*
    Results cache file format:
    (int) Results format version
    (int) Number of excluded paths
    for range Number of excluded paths
        (string) Excluded path
    for range Number of commits (this number isn't given, since file ends after commits)
        (string) Commit oid
        (time_t) Commit date in seconds since epoch
        (size_t) Commit lines of code
        (int) Number of languages in commit language map
        for range Number of languages in commit language map
            (int) Language, represented by an index in languages
            (size_t) Lines of code
        (int) Number of files in commit
        for range Number of files in commit
            (string) Path, or "//n", where n indexes a file of same path from previous commit
            (size_t) Lines of code
    */

    vector<Commit> cache_commits = {};
    const string cache_dir = Definitions::get_path_cache();
    const string cache_path = cache_dir + "/" + repo_name;

    if (filesystem::exists(cache_path)) {
        try {

            ifstream cache_file(cache_path);

            // Results Format Version

            string format_version_str;
            getline(cache_file, format_version_str);
            if (stoi(format_version_str) != RESULTS_FORMAT_VERSION) goto exited_caching;

            // Excluded Paths

            vector<string> cache_excluded_paths = {};
            string num_excluded_paths_str;
            getline(cache_file, num_excluded_paths_str);
            for (size_t i = 0; i < strtoul(num_excluded_paths_str.c_str(), nullptr, 10); i++) {
                string path;
                getline(cache_file, path);
                cache_excluded_paths.push_back(path);
            }
            if (cache_excluded_paths.size() != excluded_paths.size()) goto exited_caching;
            for (const string &path : excluded_paths)
                if (
                    find(cache_excluded_paths.begin(), cache_excluded_paths.end(), path) ==
                    cache_excluded_paths.end()
                ) goto exited_caching;

            // Commits

            string oid;
            while (getline(cache_file, oid)) {

                if (oid.size() == 0) continue; // Blank line (likely end of file)

                // Commit

                string date_str;
                getline(cache_file, date_str);
                string lines_str;
                getline(cache_file, lines_str);
                Commit commit = Commit(oid, time_t(strtoul(date_str.c_str(), nullptr, 10)));
                commit.lines = strtoul(lines_str.c_str(), nullptr, 10);

                // Language Map

                string num_langs_str;
                getline(cache_file, num_langs_str);
                for (size_t i = 0; i < strtoul(num_langs_str.c_str(), nullptr, 10); i++) {
                    string language_index_str;
                    getline(cache_file, language_index_str);
                    const Language *language = &(languages[stoi(language_index_str)]);
                    string lines_str;
                    getline(cache_file, lines_str);
                    commit.language_map[*language] = strtoul(lines_str.c_str(), nullptr, 10);
                }

                // Erase Extra Language Map Keys

                for (const Language &lang : languages)
                    if (commit.language_map[lang] == 0) commit.language_map.erase(lang);

                // Files

                string num_files_str;
                getline(cache_file, num_files_str);
                for (size_t i = 0; i < strtoul(num_files_str.c_str(), nullptr, 10); i++) {
                    string path;
                    getline(cache_file, path);
                    if (path.size() > 2 && path[0] == '/' && path[1] == '/')
                        // Path of form "//n", where n indexes a file of same path from prev commit 
                        path = cache_commits[cache_commits.size() - 1]
                            .files[strtoul(path.substr(2).c_str(), nullptr, 10)].path;
                    File file = File(path);
                    string lines_str;
                    getline(cache_file, lines_str);
                    file.lines = strtoul(lines_str.c_str(), nullptr, 10);
                    commit.files.push_back(file);
                }

                cache_commits.push_back(commit);

            }

        } catch (const runtime_error &e) {
            // Invalid format causing error in getline(), file reading error, etc.
            cache_commits = {};
        }
    }
    exited_caching:

    // Get Commit History

    if (branch.size() > 0) {
        int error = git_repository_set_head(repo, ("refs/remotes/origin/" + branch).c_str());
        if (error != 0) throw_git_error("git_repository_set_head", error);
    }
    int error = git_checkout_head(repo, nullptr);
    if (error != 0) throw_git_error("git_checkout_head", error);

    git_revwalk *repo_walker = nullptr;
    git_oid oid;
    git_commit *git_commit = nullptr;
    git_revwalk_new(&repo_walker, repo);
    git_revwalk_push_head(repo_walker);

    // Count Commits

    int total_commits = 0;
    int commits_processed = 0;
    if (on_progress != nullptr) {
        while (git_revwalk_next(&oid, repo_walker) == 0)
            if (git_commit_lookup(&git_commit, repo, &oid) == 0)
                total_commits++;
        git_revwalk_push_head(repo_walker);
        on_section_change(COMMITS_STR, start);
    }

    Commit *prev_commit_ptr = nullptr; // Used in file caching

    // File Processing Function

    function<void(const filesystem::path&, Commit&)> process_files_recursive =
    [&process_files_recursive, &excluded_paths, &git_repo_path, &prev_commit_ptr]
    (const filesystem::path &base_path, Commit &commit) {

        for (const filesystem::directory_entry &entry : filesystem::directory_iterator(base_path)) {

            filesystem::path path = entry.path();

            // Check if Path in Excluded Paths

            bool excluded = false;
            for (const string &exc_path : excluded_paths) {
                if (exc_path.size() == 0) continue;
                if (exc_path[0] == '/') {
                    string rel_path = git_repo_path + exc_path;
                    if (path.string().starts_with(rel_path)) {
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

                for (const Language &lang : languages) {
                    if (find(lang.ext.begin(), lang.ext.end(), ext) != lang.ext.end()) {

                        // Create File

                        File file = File(path);

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
                                    prev_file.contents.size() > 0 && // 0 means prev_file from cache
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

            char oid_str[GIT_OID_SHA1_HEXSIZE + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &oid);

            Commit commit = Commit(oid_str, git_commit_time(git_commit));
            git_tree *commit_tree = nullptr;

            // Check for Commit in Cache

            bool found_commit = false;
            for (const Commit &cache_commit : cache_commits) {
                if (commit.oid.compare(cache_commit.oid) == 0 && commit.date == cache_commit.date) {
                    commit = cache_commit;
                    found_commit = true;
                    break;
                }
            }

            if (!found_commit) {

                if (git_commit_tree(&commit_tree, git_commit) == 0 && commit_tree != nullptr) {

                    // Checkout Commit

                    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
                    opts.checkout_strategy = GIT_CHECKOUT_FORCE;
                    int error = git_checkout_tree(repo, (const git_object *)commit_tree, &opts);
                    if (error != 0) throw_git_error("git_checkout_tree", error);

                    // Process Files

                    if (commits.size() > 0) prev_commit_ptr = &(commits[commits.size() - 1]);
                    process_files_recursive(repo_path, commit);

                    git_tree_free(commit_tree);

                }

                // Erase Extra Language Map Keys

                for (const Language &lang : languages)
                    if (commit.language_map[lang] == 0) commit.language_map.erase(lang);

            }

            // Add Commit to Commits

            commits.push_back(commit);
            git_commit_free(git_commit);

            // Update Progress

            const long time_now = Definitions::get_time_ms();
            if (on_progress != nullptr && time_now - last_update > UPDATE_DELAY) {
                commits_processed++;
                double progress_dbl = double(commits_processed) / total_commits;
                if (cloning) progress_dbl = OBJECTS_PCT + DELTAS_PCT + COMMITS_PCT * progress_dbl;
                int progress = int(round(progress_dbl * 100));
                if (progress != prev_progress) {
                    prev_progress = progress;
                    last_update = time_now;
                    on_progress(progress, start);
                }
            }

        }

    }

    // Close git2

    git_revwalk_free(repo_walker);
    git_repository_free(repo);
    git_libgit2_shutdown();

    if (cloning)
        filesystem::remove_all(repo_path);

    // Cache Results

    if (cache_results) {

        if (on_section_change != nullptr) on_section_change(CACHE_STR, start);

        // Add New Commits to Cached Commits

        for (const Commit &commit : commits) {
            bool found_commit = false;
            for (const Commit &cache_commit : cache_commits)
                if (commit.oid.compare(cache_commit.oid) == 0)
                    found_commit = true;
            if (!found_commit) cache_commits.push_back(commit);
        }

        // Results Format and Excluded Paths
        // see "Results cache file format" for more info

        string cache_str =
            to_string(RESULTS_FORMAT_VERSION) + "\n" + to_string(excluded_paths.size()) + "\n";
        for (string &path : excluded_paths) {
            replace(path.begin(), path.end(), '\n', 'n');
            cache_str += path + "\n";
        }

        // Commits

        for (size_t i = 0; i < cache_commits.size(); i++) {

            const Commit &commit = cache_commits[i];

            // Commit Info

            cache_str +=
                commit.oid + "\n" + to_string(commit.date) + "\n" + to_string(commit.lines) + "\n" +
                to_string(commit.language_map.size()) + "\n";

            // Languages
            
            for (const auto &[language, lines] : commit.language_map)
                cache_str +=
                    to_string(
                        find(languages.begin(), languages.end(), language) - languages.begin()
                    ) + "\n" + to_string(lines) + "\n";

            // Files

            cache_str += to_string(commit.files.size()) + "\n";
            for (const File &file : commit.files) {

                bool found_path = false;
                if (i > 0) {
                    // Check for Matching Path in Previous Commit's Files
                    const Commit &prev_commit = cache_commits[i - 1];
                    for (size_t ii = 0; ii < prev_commit.files.size(); ii++) {
                        if (file.path.compare(prev_commit.files[ii].path) == 0) {
                            cache_str += "//" + to_string(ii) + "\n";
                            found_path = true;
                            break;
                        }
                    }
                }
                if (!found_path)
                    cache_str += file.path + "\n";

                cache_str += to_string(file.lines) + "\n";

            }

        }

        // Write to Cache File

        if (!filesystem::exists(cache_dir)) {
            filesystem::create_directory(cache_dir);
        }
        ofstream file(cache_path);
        if (!file.is_open()) {
            cerr << "Error opening file " << cache_path << "." << endl;
            return commits;
        }
        file << cache_str;

    }

    return commits;

}
