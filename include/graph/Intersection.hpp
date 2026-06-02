#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <string>
#include <vector>

class Road;

enum class IntersectionType
{
    T_INTERSECTION,
    CROSS,
    ROUNDABOUT
};

class Intersection
{
private:
    std::string intersectionID;

    double x;
    double y;

    std::vector<Road*> incomingRoads;
    std::vector<Road*> outgoingRoads;

    IntersectionType type;

public:
    Intersection();

    Intersection(
        const std::string& id,
        double x,
        double y,
        IntersectionType type
    );

    ~Intersection();

    // Getter
    std::string getIntersectionID() const;

    double getX() const;
    double getY() const;

    std::vector<Road*> getIncomingRoad() const;
    std::vector<Road*> getOutgoingRoad() const;

    IntersectionType getType() const;

    // Setter
    void setIntersectionID(const std::string& id);
    void setPosition(double x, double y);
    void setType(IntersectionType type);

    // Road management
    void addIncomingRoad(Road* road);
    void addOutgoingRoad(Road* road);
    void removeIncomingRoad(Road* road);
    void removeOutgoingRoad(Road* road);
};

#endif