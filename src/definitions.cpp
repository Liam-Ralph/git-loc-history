// Includes

#include "definitions.hpp"

#include <fstream>
#include <iostream>
#include <string>
using namespace std;


// Definitions

#define ABSOLUTE_PATHS false


// Definitions Class Functions


string Definitions::get_path_logo() {
    static string result = ABSOLUTE_PATHS ?
        "/usr/share/icons/hicolor/512x512/apps/git-loc-history.png" : "../logo.png";
    return result;
}
string Definitions::get_path_doc() {
    static string result = ABSOLUTE_PATHS ? "/usr/share/doc/git-loc-history" : "..";
    return result;
}
string Definitions::get_path_license() {
    static string result = ABSOLUTE_PATHS ? "/usr/share/licenses/git-loc-history/LICENSE" : "../LICENSE";
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
