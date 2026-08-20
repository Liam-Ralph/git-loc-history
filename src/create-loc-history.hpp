#pragma once
#ifndef CREATE_LOC_HISTORY_HPP
#define CREATE_LOC_HISTORY_HPP


// Includes

#include <array>
#include <atomic>
#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <vector>


// Classes

class Language {

    public:

        Language(
            std::string name, std::vector<std::string> ext,
            std::string short_comment = "//", std::array<std::string, 2> long_comment = {"/*", "*/"}
        ) : name(name), ext(ext), short_comment(short_comment), long_comment(long_comment) {}

        std::string name;
        std::vector<std::string> ext;
        std::string short_comment;
        std::array<std::string, 2> long_comment;

};

bool operator==(const Language &a, const Language &b);
bool operator<(const Language &a, const Language &b);

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

class File {

    public:

        File(std::string path, Language language) :
            path(path), language(language), lines(0), contents("") {}

        std::string path;
        Language language;
        size_t lines;
        std::string contents;

};

class Commit {

    public:

        Commit(std::string oid, std::string message, std::time_t date) :
            oid(oid), message(message), date(date), files({}), lines(0), language_map({
                {python, 0}, {java, 0}, {html, 0}, {css, 0}, {javascript, 0}, {typescript, 0},
                {c, 0}, {cpp, 0}, {c_sharp, 0}, {go, 0}, {rust, 0}, {shell, 0}
            }) {}

        std::string oid;
        std::string message;
        std::time_t date;
        std::vector<File> files;
        size_t lines;
        std::map<Language, size_t> language_map;

};


// Functions

std::vector<Commit> create_loc_history(
    std::string git_repo_path, std::vector<std::string> excluded_paths,
    const bool cloning, const std::string branch, const bool cache_results,
    std::function<void(int, clock_t)> on_progress,
    std::function<void(std::string, clock_t)> on_section_change,
    const std::clock_t start
);


#endif
