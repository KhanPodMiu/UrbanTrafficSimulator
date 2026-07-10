#pragma once

#include <string>
#include <vector>

struct RouteResult {
    std::vector<std::string> intersectionIDs;
    bool isSuccess;
    double totalCost = 0.0;
    RouteResult() : isSuccess(false), totalCost(0.0) {}
    RouteResult(const std::vector<std::string>& path, double cost = 0.0)
        : intersectionIDs(path), isSuccess(!path.empty()), totalCost(cost) {}
};

