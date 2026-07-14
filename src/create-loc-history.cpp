// Includes

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <git2.h>
#include "create-loc-history.hpp"


// Functions

vector<Commit> create_loc_history(string git_repo_path, vector<string> excluded_paths) {

    vector<Commit> commits = {};

    // Repository Setup

    git_libgit2_init();
    git_repository *repo = NULL;
    filesystem::path repo_path;

    // Get Repository Name

    string repo_name = git_repo_path.substr(git_repo_path.rfind('/') + 1);

    if (git_repo_path.find("http", 0) == 0) {

        // git_repo_path is a URL

        if (repo_name.rfind(".git") == repo_name.length() - 4) {
            repo_name = repo_name.substr(0, repo_name.length() - 4);
        }

        repo_path = "/tmp/git-loc-history/" + repo_name;

        filesystem::create_directories(repo_path);
        int error = git_clone(&repo, git_repo_path.c_str(), repo_path.c_str(), NULL);
        if (error != 0) {
            const git_error *e = git_error_last();
            throw runtime_error(
                "git_clone error " +
                to_string(error) + "/" + to_string(e->klass) + ": " + e->message
            );
        }

    } else {

        // git_repo_path is a filesystem path

        if (git_repo_path.find("/", 0) != 0 && git_repo_path.find("~", 0) != 0) {
            git_repo_path = filesystem::current_path().string() + git_repo_path;
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

    // Languages

    Language python = Language("Python", {"py"}, "#", {"\"\"\"", "\"\"\""});
    Language java = Language("Java", {"java"});
    Language html = Language("HTML", {"html"}, "", {"<!--", "-->"});
    Language css = Language("CSS", {"css"}, "", {"/*", "*/"});
    Language javascript = Language("JavaScript", {"js"});
    Language typescript = Language("TypeScript", {"ts"});
    Language c = Language("C", {"c", "h"});
    Language cpp = Language("C++", {"cpp", "hpp"});
    Language c_sharp = Language("C#", {"cs"});
    Language go = Language("Go", {"go"});
    Language rust = Language("Rust", {"rs"});
    Language shell = Language("Shell", {"sh"}, "#", {"", ""});
    Language languages[] = {
        python, java, html, css, javascript, typescript, c, cpp, c_sharp, go, rust, shell
    };

    // Get Commit History

    git_checkout_head(repo, NULL);

    git_revwalk *repo_walker = NULL;
    git_oid oid;
    git_commit *git_commit = NULL;
    git_revwalk_new(&repo_walker, repo);
    git_revwalk_push_head(repo_walker);

    function<void(const filesystem::path&, Commit&)> process_files_recursive =
    [&process_files_recursive, &excluded_paths, &git_repo_path, &languages]
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

                        // Remove Indents and Newlines

                        auto replace_str = [&contents](string old_str, string new_str) {
                            size_t pos = contents.find(old_str);
                            while (pos != string::npos) {
                                contents.replace(pos, old_str.length(), new_str);
                                pos = contents.find(old_str, pos + new_str.length());
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
                                if (line.find(lang.short_comment) == 0) {
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
                            size_t pos_end = contents.find(lang.long_comment[1]);
                            while (pos_start != string::npos && pos_end != string::npos) {
                                contents = contents.substr(0, pos_start) +
                                    contents.substr(pos_end + lang.long_comment[1].length());
                                pos_start = contents.find(lang.long_comment[0]);
                                pos_end = contents.find(lang.long_comment[1], pos_start);
                            }
                        }

                        // Finish Cleaning File

                        replace_str("\n\n", "\n");

                        // Calculate File Lines

                        size_t lines = 0;
                        for (const char &c : contents) {
                            if (c == '\n') lines++;
                        }
                        if (contents.rfind('\n') == contents.length() - 1) lines--;
                        file.lines = lines;

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

    while (git_revwalk_next(&oid, repo_walker) == 0) {
        if (git_commit_lookup(&git_commit, repo, &oid) == 0) {

            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &oid);

            Commit commit = Commit(
                oid_str, git_commit_message(git_commit), git_commit_time(git_commit)
            );

            git_tree *commit_tree = NULL;
            if (git_commit_tree(&commit_tree, git_commit) == 0 && commit_tree != NULL) {

                // Checkout Commit

                git_checkout_tree(repo, (const git_object *)commit_tree, NULL);

                // Iterate over Files

                process_files_recursive(repo_path, commit);

                git_tree_free(commit_tree);
            
            }

            commits.push_back(commit);
            git_commit_free(git_commit);

        }
    }

    git_revwalk_free(repo_walker);
    git_repository_free(repo);
    git_libgit2_shutdown();

    return commits;

}