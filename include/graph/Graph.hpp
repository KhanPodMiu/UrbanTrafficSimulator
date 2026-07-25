#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional> //ADDED: Library for call back function
#include "../utils/vector2i.hpp" //Added: for 

class TrafficLightManager; 
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
    const std::shared_ptr<Intersection> getIntersection(const std::string& intersectionID) const;
    const std::shared_ptr<Road> getRoad(const std::string& roadID) const;
    bool isValid() const;
    // ===== ADDED: Get call back function=====
    void forEachIntersection(std::function<void(const std::shared_ptr<Intersection>&)> func) const;
    const std::unordered_map<std::string, std::shared_ptr<Intersection>>& getIntersections() const;
    const std::unordered_map<std::string, std::shared_ptr<Road>>& getRoads() const;

    Road* getRoadBetween(const std::string& sourceID, const std::string& destinationID) const;

    //Added function to find nearest intersection
    float pointToSegmentDistance(Vector2 P, Vector2 A, Vector2 B) const;
    std::shared_ptr<Intersection> findNearestIntersection(const Vector2& clickPos, float maxDistance) const;
    std::shared_ptr<Road> findNearestRoad(const Vector2& clickPos, float maxDistance) const;
    std::shared_ptr<Intersection> getRandomIntersectionExcept(const std::string& startID) const;

};
