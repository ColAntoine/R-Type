#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include "SimpleIni.h"

class Config {
    public:
        static Config& instance() {
            static Config _instance;
            return _instance;
        }

        bool openConfigFile(const std::string& filepath);
        bool closeCurrentConfigFile();
        bool saveConfigFile();

        std::vector<std::pair<std::string, std::string>> getSection(const std::string& section);

        std::string getValueOf(const std::string& section, const std::string& key);
        bool setValueOf(const std::string& section, const std::string& key, const std::string& value);
    private:
        Config() = default;
        ~Config() = default;

        std::string _filepath = "";
        CSimpleIniA _configFile;
};
