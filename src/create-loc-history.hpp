#pragma once
#ifndef CREATE_LOC_HISTORY_HPP
#define CREATE_LOC_HISTORY_HPP


// Includes

#include <array>
#include <atomic>
#include <map>
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
        Language language;
        size_t lines;

        File(string path, Language language) : path(path), language(language), lines(0) {}

};

class Commit {

    public:

        string oid;
        string message;
        time_t date;
        vector<File> files;
        size_t lines;
        map<Language, size_t> language_map;

        Commit(string oid, string message, time_t date) :
            oid(oid), message(message), date(date), files({}), lines(0), language_map() {}

};


// Functions

vector<Commit> create_loc_history(
    string git_repo_path, vector<string> excluded_paths, array<atomic<int>, 6> *progress_ptr
);


#endif
