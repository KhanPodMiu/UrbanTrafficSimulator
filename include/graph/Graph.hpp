#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class Intersection;
class Road;
class Graph {
private:
    std::unordered_map<std::string, std::shared_ptr <Intersection>> Intersections;
    std::unordered_map<std::string, std::shared_ptr <Road>> Roads;
    std::unordered_map<std::string, std::vector <std::shared_ptr <Road>>> adjacencyList;
public:
    Graph();
    ~Graph();
    bool addIntersection(std::shared_ptr <Intersection> intersection);
    bool addRoad(std::shared_ptr <Road> road);
    const std::vector<std::shared_ptr <Road>> &getConnectedRoads(const std::string& intersectionID) const;
    std::shared_ptr<Intersection> getIntersection(const std::string& intersectionID) const;
    std::shared_ptr<Road> getRoad(const std::string& roadID) const;
    bool isValid() const;

};