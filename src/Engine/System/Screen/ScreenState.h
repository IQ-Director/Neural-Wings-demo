#pragma once
#include <iostream>
#include <string>

class ScreenState
{
protected:
    int id;
    std::string name;

public:
    virtual ~ScreenState() = default;
    ScreenState() : id(-2), name("none") {}
    ScreenState(int id, std::string name) : id(id), name(name) {}
    operator std::string() const { return name; }
    operator int() const { return id; }
    bool operator==(const ScreenState &other) const { return id == other.id; }
    bool operator!=(const ScreenState &other) const { return id != other.id; }

    std::string getName() const { return name; }
};
static ScreenState SCREEN_STATE_START = {-1, "start"};
static ScreenState SCREEN_STATE_NONE = {-2, "none"};
static ScreenState SCREEN_STATE_EXIT = {-3, "exit"};
static ScreenState SCREEN_STATE_ERROR = {-4, "error"};