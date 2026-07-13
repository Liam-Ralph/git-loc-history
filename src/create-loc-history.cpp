// Includes

#include <array>
#include <ctime>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <git2.h>
#include "create-loc-history.hpp"


// Functions

vector<Commit> create_loc_history(string git_repo_path) {

    vector<Commit> commits = {};

    // Repository Setup

    git_libgit2_init();
    git_repository *repo = NULL;
    filesystem::path repo_path;

    // Get Repository Name

    string repo_name = git_repo_path.substr(git_repo_path.rfind('/') + 1);

    if (git_repo_path.rfind("http", 0) == 0) {

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

        if (git_repo_path.rfind("/", 0) == 0 || git_repo_path.rfind("~", 0) == 0) {
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

    // Get Commit History

    git_checkout_head(repo, NULL);

    git_revwalk *repo_walker = NULL;
    git_oid oid;
    git_commit *commit = NULL;
    git_revwalk_new(&repo_walker, repo);
    git_revwalk_push_head(repo_walker);

    while (git_revwalk_next(&oid, repo_walker) == 0) {
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &oid);
            commits.push_back(Commit(oid_str, git_commit_message(commit), git_commit_time(commit)));
            git_commit_free(commit);
        }
    }

    git_revwalk_free(repo_walker);
    git_repository_free(repo);
    git_libgit2_shutdown();

    return commits;

}