#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <string>
#include <vector>
#include <unordered_set>

class Road;

enum class IntersectionType
{                     // neighbor = 0 => throw exception
    DEAD_END,         // neighbor = 1
    STRAIGHT,         // neighbor = 2
    T_INTERSECTION,   // neighbor = 3
    CROSS,            // neighbor = 4
    ROUNDABOUT        // neighbor > 4
};

class Intersection
{
private:
    const std::string intersectionID;
    int x;
    int y;
    std::vector<const Road*> incomingRoads;
    std::vector<const Road*> outgoingRoads;
    int getNeighborCount() const;

public:
    Intersection(std::string id,int x,int y);
    ~Intersection();
    // Getters
    const std::string& getIntersectionID() const;
    int getX() const;
    int getY() const;
    IntersectionType getType() const;
    const std::vector<const Road*>& getIncomingRoads() const;
    const std::vector<const Road*>& getOutgoingRoads() const;
    int getIncomingRoadCount() const;
    int getOutgoingRoadCount() const;
    int getDegree() const;
    int getRadius() const;
    // Setters
    bool setPosition(int x, int y);
    // Road management
    bool addIncomingRoad(const Road* road);
    bool addOutgoingRoad(const Road* road);
    bool removeIncomingRoad(const Road* road);
    //A
    bool removeOutgoingRoad(const Road* road);
};

#endif