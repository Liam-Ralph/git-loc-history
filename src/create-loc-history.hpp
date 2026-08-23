#pragma once
#ifndef CREATE_LOC_HISTORY_HPP
#define CREATE_LOC_HISTORY_HPP


// Includes

#include <array>
#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <vector>


// Classes

// Language

class Language {

    public:

        Language(
            std::string name, std::vector<std::string> ext,
            std::string short_comment = "//", std::array<std::string, 2> long_comment = {"/*", "*/"}
        ) : name(name), ext(ext), short_comment(short_comment), long_comment(long_comment) {}

        std::string name; // e.g. "Python"
        std::vector<std::string> ext; // e.g. "py"
        std::string short_comment; // e.g. "//"
        std::array<std::string, 2> long_comment; // e.g. {"/*", "*/"}

};

bool operator==(const Language &a, const Language &b);
bool operator<(const Language &a, const Language &b); // Needed to make a map

extern const Language python;
extern const Language java;
extern const Language html;
extern const Language css;
extern const Language javascript;
extern const Language typescript;
extern const Language c;
extern const Language cpp;
extern const Language c_sharp;
extern const Language go;
extern const Language rust;
extern const Language shell;
extern const std::array<Language, 12> languages;

// File

class File {

    public:

        File(std::string path) : path(path), lines(0), contents("") {}

        std::string path; // Absolute path to file
        size_t lines;
        std::string contents; // Empty when file retrieved from cache

};

// Commit

class Commit {

    public:

        Commit(std::string oid, std::time_t date) :
            oid(oid), date(date), lines(0), language_map({
                {python, 0}, {java, 0}, {html, 0}, {css, 0}, {javascript, 0}, {typescript, 0},
                {c, 0}, {cpp, 0}, {c_sharp, 0}, {go, 0}, {rust, 0}, {shell, 0}
            }) {}

        std::string oid; // SHA1 sum, 40 characters
        std::time_t date;
        size_t lines;
        std::map<Language, size_t> language_map; // Lines of code for each language

};


// Functions

void throw_git_error(std::string function_name, int error);

std::vector<Commit> create_loc_history(
    std::string git_repo_path, std::vector<std::string> excluded_paths,
    const bool cloning, const std::string branch, const bool cache_results,
    std::function<void(int, long)> on_progress,
    std::function<void(std::string, long)> on_section_change,
    const long start
);


#endif
