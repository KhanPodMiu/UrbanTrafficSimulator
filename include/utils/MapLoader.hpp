#pragma once

#include <string>

class Graph;

class MapLoader {
public:
    static bool loadFromJson(const std::string& filename, Graph& graph);
};