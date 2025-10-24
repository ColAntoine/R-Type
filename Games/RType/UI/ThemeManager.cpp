#include "ThemeManager.hpp"

void ThemeManager::setTheme(const ColorPalette& palette)
{
    this->_current = palette;
}

const ColorPalette& ThemeManager::getTheme() const
{
    return this->_current;
}

const std::string ThemeManager::getThemeName() const
{
    return this->_current.name;
}