#pragma once

#include <string>

struct RouteRequest {
    std::string startIntersectionID;
    std::string destinationIntersectionID;

    // Constructor mặc định và có tham số
    RouteRequest() = default;
    RouteRequest(const std::string& start, const std::string& dest)
        : startIntersectionID(start), destinationIntersectionID(dest) {}
};

