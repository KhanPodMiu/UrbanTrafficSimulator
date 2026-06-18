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
    bool removeIntersection(const std::string& intersectionID);
    bool addRoad(std::shared_ptr <Road> road);
    bool removeRoad(const std::string& roadID);
    const std::vector<std::shared_ptr <Road>> &getConnectedRoads(const std::string& intersectionID) const;
    std::shared_ptr<Intersection> getIntersection(const std::string& intersectionID) const;
    std::shared_ptr<Road> getRoad(const std::string& roadID) const;
    bool isValid() const;
    const std::unordered_map<std::string, std::shared_ptr<Road>>& getRoads() const;
    const std::unordered_map<std::string, std::shared_ptr <Intersection>>& getIntersections() const;

};