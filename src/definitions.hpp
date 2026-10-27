#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP


// Includes

#include <string>
#include <unordered_map>
#include <vector>


// Definitions Class

class Definitions {

    public:

        // Functions

        static long get_time_ms();

        static std::string get_path_logo();
        static std::string get_path_doc();
        static std::string get_path_license();
        static std::string get_path_readme();
        static std::string get_version();

        static std::string get_path_config();
        static std::unordered_map<std::string, std::string> get_config();
        static int set_config(std::string setting, std::string value);

        static std::string get_path_cache();
        static std::string get_cache_size();

        static std::string get_path_cached_results();

        static int get_recent_paths_length();
        static std::string get_path_recent_paths();
        static std::vector<std::string> get_recent_paths();
        static void set_recent_paths(std::vector<std::string> recent_paths);

};


#endif
