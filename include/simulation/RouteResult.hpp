#pragma once

#include <string>
#include <vector>

struct RouteResult {
    std::vector<std::string> intersectionIDs;
    bool isSuccess;
    RouteResult() : isSuccess(false) {}
    RouteResult(const std::vector<std::string>& path)
        : intersectionIDs(path), isSuccess(!path.empty()) {}
};

