#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <string>
#include <vector>

class Road;

enum class IntersectionType
{
    T_INTERSECTION,
    ROUNDABOUT,
    CROSS
};

class Intersection
{
private:
    std::string intersectionID;

    int x;
    int y;

    IntersectionType type;

    std::vector<Road*> incomingRoads;
    std::vector<Road*> outgoingRoads;

public:
    Intersection(
        const std::string& id,
        int x,
        int y,
        IntersectionType type
    );

    ~Intersection();

    // Getters
    const std::string& getIntersectionID() const;

    int getX() const;
    int getY() const;

    IntersectionType getType() const;

    const std::vector<Road*>& getIncomingRoads() const;
    const std::vector<Road*>& getOutgoingRoads() const;

    int getIncomingRoadCount() const;
    int getOutgoingRoadCount() const;

    // Setters
    bool setPosition(int x, int y);

    // Road management
    bool addIncomingRoad(Road* road);
    bool addOutgoingRoad(Road* road);

    bool removeIncomingRoad(Road* road);
    bool removeOutgoingRoad(Road* road);

    int getDegree() const;
};

#endif