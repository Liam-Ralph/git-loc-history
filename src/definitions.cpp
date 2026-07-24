// Includes

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include "definitions.hpp"


// Definitions

#define ABSOLUTE_PATHS false


// Definitions Class Functions


string Definitions::get_path_logo() {
    return ABSOLUTE_PATHS ?
        "/usr/share/icons/hicolor/512x512/apps/git-loc-history.png" : "../logo.png";
}
string Definitions::get_path_doc() {
    return ABSOLUTE_PATHS ? "/usr/share/doc/git-loc-history/" : "../";
}
string Definitions::get_path_license() {
    return ABSOLUTE_PATHS ? "/usr/share/licenses/git-loc-history/LICENSE" : "../LICENSE";
}
string Definitions::get_version() {
    const string readme_path = Definitions::get_path_doc() + "README.md";
    ifstream file(readme_path);
    if (!file.is_open()) {
        cerr << "Error opening file " << readme_path << "." << endl;
        return "-1";
    }
    string line;
    for (int i = 0; i < 3; i++) getline(file, line);
    const string version = line.substr(12);
    file.close();
    return version;
}
