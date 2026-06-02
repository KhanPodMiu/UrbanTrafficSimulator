#include "graph/Intersection.hpp"

#include <algorithm>

Intersection::Intersection()
    : intersectionID(""),
      x(0.0),
      y(0.0),
      type(IntersectionType::CROSS)
{
}

Intersection::Intersection(
    const std::string& id,
    double x,
    double y,
    IntersectionType type)
    : intersectionID(id),
      x(x),
      y(y),
      type(type)
{
}

Intersection::~Intersection()
{
}

//Getter
std::string Intersection::getIntersectionID() const
{
    return intersectionID;
}

double Intersection::getX() const
{
    return x;
}

double Intersection::getY() const
{
    return y;
}

std::vector<Road*> Intersection::getIncomingRoad() const
{
    return incomingRoads;
}

std::vector<Road*> Intersection::getOutgoingRoad() const
{
    return outgoingRoads;
}

IntersectionType Intersection::getType() const
{
    return type;
}

//Setter
void Intersection::setIntersectionID(const std::string& id)
{
    intersectionID = id;
}

void Intersection::setPosition(double x, double y)
{
    this->x = x;
    this->y = y;
}

void Intersection::setType(IntersectionType type)
{
    this->type = type;
}

// Add Roads
void Intersection::addIncomingRoad(Road* road)
{
    if (road != nullptr)
    {
        incomingRoads.push_back(road);
    }
}

void Intersection::addOutgoingRoad(Road* road)
{
    if (road != nullptr)
    {
        outgoingRoads.push_back(road);
    }
}

// Remove Roads
void Intersection::removeIncomingRoad(Road* road)
{
    incomingRoads.erase(
        std::remove(
            incomingRoads.begin(),
            incomingRoads.end(),
            road),
        incomingRoads.end());
}

void Intersection::removeOutgoingRoad(Road* road)
{
    outgoingRoads.erase(
        std::remove(
            outgoingRoads.begin(),
            outgoingRoads.end(),
            road),
        outgoingRoads.end());
}