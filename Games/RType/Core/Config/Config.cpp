#include "Config.hpp"

bool Config::openConfigFile(const std::string& filepath)
{
    _configFile.SetUnicode();
    SI_Error rc = _configFile.LoadFile(filepath.c_str());
    return rc == SI_OK;
}

bool Config::closeCurrentConfigFile()
{
    return true;
}

std::vector<std::pair<std::string, std::string>> Config::getSection(const std::string& section)
{
    std::vector<std::pair<std::string, std::string>> result;
    CSimpleIniA::TNamesDepend keys;
    _configFile.GetAllKeys(section.c_str(), keys);
    for (const auto& key : keys) {
        const char* value = _configFile.GetValue(section.c_str(), key.pItem, NULL);
        if (value) {
            result.emplace_back(key.pItem, value);
        }
    }
    return result;
}

std::string Config::getValueOf(const std::string& section, const std::string& key)
{
    const char* value = _configFile.GetValue(section.c_str(), key.c_str(), NULL);
    return value ? value : "";
}

bool Config::setValueOf(const std::string& section, const std::string& key, const std::string& value)
{
    return _configFile.SetValue(section.c_str(), key.c_str(), value.c_str()) == SI_OK;
}
