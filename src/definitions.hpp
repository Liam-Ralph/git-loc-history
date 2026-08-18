#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP


// Includes

#include <string>
#include <unordered_map>


// Definitions Class

class Definitions {

    public:

        static std::string get_path_logo();
        static std::string get_path_doc();
        static std::string get_path_license();
        static std::string get_path_readme();
        static std::string get_version();
        static std::string get_config_path();
        static std::unordered_map<std::string, std::string> get_config();
        static int set_config(std::string setting, std::string value);

};


#endif
