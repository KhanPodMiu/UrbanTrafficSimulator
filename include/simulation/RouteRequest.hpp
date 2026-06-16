#pragma once

#include <string>

struct RouteRequest {
    std::string startIntersectionID;
    std::string destinationIntersectionID;

    RouteRequest() = default;
    RouteRequest(const std::string& start, const std::string& dest)
        : startIntersectionID(start), destinationIntersectionID(dest) {}
};

