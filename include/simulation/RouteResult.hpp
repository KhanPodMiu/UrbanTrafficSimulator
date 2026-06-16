#pragma once

#include <string>
#include <vector>

struct Route {
    std::vector<std::string> intersectionIDs;
    bool isValid;
    Route() : isValid(false) {}
    Route(const std::vector<std::string>& path)
        : intersectionIDs(path), isValid(!path.empty()) {}
};

