#pragma once
#ifndef CREATE_LOC_HISTORY_HPP
#define CREATE_LOC_HISTORY_HPP


// Includes

#include <array>
#include <string>
#include <vector>


// Classes

class Language {

    public:
        
        string name;
        vector<string> ext;
        string short_comment;
        array<string, 2> long_comment;

        Language(
            string name, vector<string> ext,
            string short_comment = "//", array<string, 2> long_comment = {"/*", "*/"}
        ) : name(name), ext(ext), short_comment(short_comment), long_comment(long_comment) {}

};

class File {

    public:

        string path;
        Language lang;
        size_t lines;

        File(string path, Language lang) : path(path), lang(lang), lines(0) {}

};

class Commit {

    public:

        string oid;
        string message;
        time_t date;
        vector<File> files;
        size_t lines;

        Commit(string oid, string message, time_t date) :
            oid(oid), message(message), date(date), files({}), lines(0) {}

};


// Functions

vector<Commit> create_loc_history(
    string git_repo_path, vector<string> excluded_paths, array<int, 2> *progress_ptr
);


#endif