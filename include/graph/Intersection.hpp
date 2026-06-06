#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <string>
#include <vector>

class Road;

enum class IntersectionType
{                     // degree = 0 => throw exception
    DEAD_END,         // degree = 1
    STRAIGHT,         // degree = 2
    T_INTERSECTION,   // degree = 3
    CROSS,            // degree = 4
    ROUNDABOUT        // degree > 4
};

class Intersection
{
private:
    const std::string& intersectionID;
    int x;
    int y;

    IntersectionType type;

    std::vector<const Road*> incomingRoads;
    std::vector<const Road*> outgoingRoads;

public:
    Intersection(
        std::string id,
        int x,
        int y
    );

    ~Intersection();

    // Getters
    std::string getIntersectionID() const;

    int getX() const;
    int getY() const;

    IntersectionType getType() const;

    const std::vector<const Road*>& getIncomingRoads() const;
    const std::vector<const Road*>& getOutgoingRoads() const;

    int getIncomingRoadCount() const;
    int getOutgoingRoadCount() const;

    int getDegree() const;

    // Setters
    bool setPosition(int x, int y);

    // Road management
    bool addIncomingRoad(const Road* road);
    bool addOutgoingRoad(const Road* road);

    bool removeIncomingRoad(const Road* road);
    bool removeOutgoingRoad(const Road* road);
};

#endif