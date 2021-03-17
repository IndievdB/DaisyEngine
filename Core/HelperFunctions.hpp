#pragma once
#include <string>

namespace HelperFunctions
{
    bool HasEnding(std::string const& fullString, std::string const& ending)
    {
        if (fullString.length() >= ending.length())
        {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else
        {
            return false;
        }
    }
}