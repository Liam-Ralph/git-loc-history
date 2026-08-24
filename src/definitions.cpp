// Includes

#include "definitions.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;


// Definitions

#define RELEASE_PATHS false // true for packaged applications


// Definitions Class Functions

/**
 * Get the number of milliseconds since epoch.
 * 
 * @return Milliseconds since epoch.
 */
long Definitions::get_time_ms() {
    return chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
}

/**
 * Get the path of the program's logo.
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_logo() {
    static string result = RELEASE_PATHS ?
        "/usr/share/icons/hicolor/512x512/apps/git-loc-history.png" : "../logo.png";
    return result;
}

/**
 * Get the path of the program's documentation.
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_doc() {
    static string result = RELEASE_PATHS ? "/usr/share/doc/git-loc-history" : "..";
    return result;
}

/**
 * Get the path of the program's license.
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_license() {
    static string result = RELEASE_PATHS ? "/usr/share/licenses/git-loc-history/LICENSE" : "../LICENSE";
    return result;
}

/**
 * Get the path of the program's README.md.
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_readme() {
    static string result = Definitions::get_path_doc() + "/README.md";
    return result;
}

/**
 * Get the program version, read from the README.md.
 * 
 * @return Version, in form "x.y.z"
 */
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

/**
 * Get the path of the program's config file.
 * 
 * Uses XDG_CONFIG_HOME
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_config() {
    static string config_path = []() {
        if (!RELEASE_PATHS) return string("../src/defaults.conf");
        char *xdg_config_home = getenv("XDG_CONFIG_HOME");
        char *home = getenv("HOME");
        if (xdg_config_home == nullptr) {
            if (home == nullptr)
                return string("~/.config/git-loc-history.conf");
            else
                return string(home) + "/.config/git-loc-history.conf";
        }
        return string(xdg_config_home) + "/git-loc-history.conf";
    }();
    return config_path;
}

/**
 * Get the current user config.
 * 
 * strings "true"/"false" represent booleans true/false.
 * 
 * @return An unordered_map, with a key for each setting found.
 */
unordered_map<string, string> Definitions::get_config() {

    // Find Config File

    const string config_path = get_path_config();
    ifstream file(config_path);
    if (!file.is_open() && RELEASE_PATHS) {
        // Create User Config File
        filesystem::copy_file("/usr/share/git-loc-history/defaults.conf", config_path);
        file.close();
        file.open(config_path);
    }
    if (!file.is_open()) {
        cerr << "Error opening file " << config_path << "." << endl;
        return unordered_map<string, string>();
    }

    // Read Config File

    unordered_map<string, string> config_map;
    string line;
    while (getline(file, line)) {
        if (line.length() > 0 && line[1] != '#') {
            size_t equal_pos = line.find('=');
            config_map.emplace(line.substr(0, equal_pos), line.substr(equal_pos + 1));
        }
    }

    return config_map;

}

/**
 * Set an option in the user's config file.
 * 
 * @param setting The name of the setting to be changed.
 * @param value The value to set. true/false is "true"/"false".
 * @return An error code. 1=failed to read config file, 2=failed to find setting
 * in existing config, 3=failed to write config file.
 */
int Definitions::set_config(string setting, string value) {

    // Get Config File

    const string config_path = get_path_config();
    ifstream ifile(config_path);
    if (!ifile.is_open() && RELEASE_PATHS) {
        // Create User Config File
        filesystem::copy_file("/usr/share/git-loc-history/defaults.conf", config_path);
        ifile.close();
        ifile.open(config_path);
    }
    if (!ifile.is_open()) {
        cerr << "Error opening file " << config_path << " (read)." << endl;
        return 1;
    }

    // Read File and Change Setting

    bool found = false;
    string lines = "";
    string line;
    while (getline(ifile, line)) {
        if (line.starts_with(setting)) {
            lines += setting + "=" + value + "\n";
            found = true;
        } else {
            lines += line + "\n";
        }
    }
    ifile.close();
    if (!found) return 2;

    // Write Config File

    ofstream ofile(config_path);
    if (!ofile.is_open()) {
        cerr << "Error opening file " << config_path << " (write)." << endl;
        return 3;
    }
    ofile << lines;

    return 0;

}

/**
 * Get the path of the project's cache file.
 * 
 * @return Path, absolute if RELEASE_PATHS, otherwise relative to build.
 */
string Definitions::get_path_cache() {
    static string cache_path = []() {
        if (!RELEASE_PATHS) return string("../cache");
        char *xdg_cache_home = getenv("XDG_CACHE_HOME");
        char *home = getenv("HOME");
        if (xdg_cache_home == nullptr) {
            if (home == nullptr)
                return string("~/.cache/git-loc-history");
            else
                return string(home) + "/.cache/git-loc-history";
        }
        return string(xdg_cache_home) + "/git-loc-history";
    }();
    return cache_path;
}

/**
 * Get the current cache size, formatted to a string.
 * 
 * @return Current cache size. (e.g. "1.2kiB").
 */
string Definitions::get_cache_size() {

    // Get Cache Path

    const filesystem::path path_cache = get_path_cache();
    if (!filesystem::exists(path_cache)) {
        return "0B";
    }

    // Get Cache Size in Bytes

    long total_bytes = 0;
    for (const filesystem::directory_entry &entry : filesystem::directory_iterator(path_cache))
        if (filesystem::is_regular_file(entry.path()))
            total_bytes += filesystem::file_size(entry.path());

    // Format Size as String

    const array<string, 7> suffixes = {"B", "kiB", "MiB", "GiB", "TiB", "PiB", "EiB"};
    int exp = 0;
    while (total_bytes >= pow(1024, exp + 1)) exp++;
    stringstream ss;
    ss <<
        fixed << setprecision(1) <<
        round(float(total_bytes) / pow(1024, exp) * 10) / 10 << suffixes[exp];

    return ss.str();

}
