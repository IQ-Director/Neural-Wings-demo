#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

inline bool __SHOWINFO__ = true;

class Config
{
public:
    virtual ~Config() = default;

    bool load(const std::string &filePath);

protected:
    virtual void ParseJson(const json &configJson) = 0;
};