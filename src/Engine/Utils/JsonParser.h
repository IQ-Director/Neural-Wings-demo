#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "Engine/Math/Math.h"

using json = nlohmann::json;

class JsonParser
{
public:
    static Vector3f ToVector3f(const json &j, Vector3f defaultValue = Vector3f::ZERO)
    {
        if (j.is_array() && j.size() == 3)
        {
            return Vector3f(j[0].get<float>(), j[1].get<float>(), j[2].get<float>());
        }
        return defaultValue;
    }
    static Color ToColor(const json &j, Color defaultValue = WHITE)
    {
        if (j.is_array() && j.size() == 4)
        {
            return Color{(unsigned char)j[0], (unsigned char)j[1], (unsigned char)j[2], (unsigned char)j[3]};
        }
        return defaultValue;
    }
};