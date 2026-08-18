// Includes

#include "definitions.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


// Definitions

#define RELEASE_PATHS false


// Definitions Class Functions

string Definitions::get_path_logo() {
    static string result = RELEASE_PATHS ?
        "/usr/share/icons/hicolor/512x512/apps/git-loc-history.png" : "../logo.png";
    return result;
}

string Definitions::get_path_doc() {
    static string result = RELEASE_PATHS ? "/usr/share/doc/git-loc-history" : "..";
    return result;
}

string Definitions::get_path_license() {
    static string result = RELEASE_PATHS ? "/usr/share/licenses/git-loc-history/LICENSE" : "../LICENSE";
    return result;
}

string Definitions::get_path_readme() {
    static string result = Definitions::get_path_doc() + "/README.md";
    return result;
}

string Definitions::get_version() {
    static string version = []() {
        const string readme_path = Definitions::get_path_readme();
        ifstream file(readme_path);
        if (!file.is_open()) {
            cerr << "Error opening file " << readme_path << "." << endl;
            return string("-1");
        }
        string line;
        for (int i = 0; i < 3; i++) getline(file, line);
        file.close();
        return line.substr(12);
    }();
    return version;
}

string Definitions::get_config_path() {
    static string config_path = []() {
        if (!RELEASE_PATHS) return string("../src/defaults.conf");
        char *env_var = getenv("XDG_CONFIG_HOME");
        if (env_var == nullptr) {
            return string("~/.config/git-loc-history.conf");
        }
        return string(env_var) + "/git-loc-history.conf";
    }();
    return config_path;
}

unordered_map<string, string> Definitions::get_config() {

    const string config_path = get_config_path();
    ifstream file(config_path);
    if (!file.is_open() && RELEASE_PATHS) {
        filesystem::copy_file("/usr/share/git-loc-history/defaults.conf", config_path);
        file.close();
        file.open(config_path);
    }
    if (!file.is_open()) {
        cerr << "Error opening file " << config_path << "." << endl;
        return unordered_map<string, string>();
    }

    unordered_map<string, string> config_map;
    string line;
    while (getline(file, line)) {
        if (line.compare("") != 0) {
            size_t equal_pos = line.find('=');
            config_map.emplace(line.substr(0, equal_pos), line.substr(equal_pos + 1));
        }
    }

    return config_map;

}

int Definitions::set_config(string setting, string value) {

    const string config_path = get_config_path();
    ifstream ifile(config_path);
    if (!ifile.is_open() && RELEASE_PATHS) {
        filesystem::copy_file("/usr/share/git-loc-history/defaults.conf", config_path);
        ifile.close();
        ifile.open(config_path);
    }
    if (!ifile.is_open()) {
        cerr << "Error opening." << endl;
        return 1;
    }

    string lines = "";
    string line;
    while (getline(ifile, line)) {
        if (line.find(setting) == 0)
            lines += setting + "=" + value + "\n";
        else
            lines += line + "\n";
    }
    ifile.close();

    ofstream ofile(config_path);
    if (!ofile.is_open()) {
        cerr << "Error opening config file." << endl;
        return 1;
    }
    ofile << lines;

    return 0;

}