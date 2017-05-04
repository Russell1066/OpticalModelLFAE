#pragma once

#include "json.hpp"

#include <fstream>

namespace jsonHelper
{
    using json = nlohmann::json;

    template<typename T>
    T GetValueOrDefault(const json &j, std::string const& name, T const& defaultValue)
    {
        auto at = j.find(name);
        if (at != j.end())
        {
            return at->get<T>();
        }

        return defaultValue;
    }

    template<typename T>
    T FromJson(std::string const& paramFile)
    {
        if (paramFile.size() == 0)
        {
            return T();
        }

        std::ifstream readParams(paramFile);
        json j;
        readParams >> j;

        T retv = j;

        return retv;
    }

    template<typename T>
    std::string GetJson(T const& t)
    {
        return json(t).dump(4);
    }

    template<typename T>
    void WriteJsonConfig(T const& t, std::string const& filename)
    {
        std::ostream writeParams(paramFile);
        writeParams << GetJson(t);
    }
}

