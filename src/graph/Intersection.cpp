#include "graph/Intersection.hpp"
#include <algorithm>
#include <stdexcept>

constexpr int MAP_WIDTH = 4000;
constexpr int MAP_HEIGHT = 4000;

Intersection::Intersection(
    std::string id,
    int                x,
    int                y,
    IntersectionType   type)
    :
    intersectionID(id),
    x(x),
    y(y),
    type(type)
{
    if (id.empty())
    {
        throw std::invalid_argument(
            "Intersection ID cannot be empty");
    }

    if (x < 0 || x > MAP_WIDTH)
    {
        throw std::invalid_argument(
            "Intersection \"" + id +
            "\": x position " + std::to_string(x) +
            " is outside valid range [0, " +
            std::to_string(MAP_WIDTH) + "]");
    }

    if (y < 0 || y > MAP_HEIGHT)
    {
        throw std::invalid_argument(
            "Intersection \"" + id +
            "\": y position " + std::to_string(y) +
            " is outside valid range [0, " +
            std::to_string(MAP_HEIGHT) + "]");
    }
}

Intersection::~Intersection()
{
}

//Getters
std::string Intersection::getIntersectionID() const
{
    return intersectionID;
}

int Intersection::getX() const
{
    return x;
}

int Intersection::getY() const
{
    return y;
}

IntersectionType Intersection::getType() const
{
    return type;
}

const std::vector<Road*>& Intersection::getIncomingRoads() const
{
    return incomingRoads;
}

const std::vector<Road*>& Intersection::getOutgoingRoads() const
{
    return outgoingRoads;
}

int Intersection::getIncomingRoadCount() const
{
    return incomingRoads.size();
}

int Intersection::getOutgoingRoadCount() const
{
    return outgoingRoads.size();
}

int Intersection::getDegree() const
{
    return incomingRoads.size() + outgoingRoads.size();
}

//Setters
bool Intersection::setPosition(int x, int y)
{
    if (x < 0 || x > MAP_WIDTH || y < 0 || y > MAP_HEIGHT)
    {
        return false;
    }
    
    this->x = x;
    this->y = y;

    return true;
}

bool Intersection::addIncomingRoad(Road* road)
{
    if (road == nullptr)
    {
        return false;
    }

    auto it = std::find(
        incomingRoads.begin(),
        incomingRoads.end(),
        road);

    if (it != incomingRoads.end())
    {
        return true;
    }

    incomingRoads.push_back(road);

    return true;
}

bool Intersection::addOutgoingRoad(Road* road)
{
    if (road == nullptr)
    {
        return false;
    }

    auto it = std::find(
        outgoingRoads.begin(),
        outgoingRoads.end(),
        road);

    if (it != outgoingRoads.end())
    {
        return true;
    }

    outgoingRoads.push_back(road);

    return true;
}

bool Intersection::removeIncomingRoad(Road* road)
{
    if (road == nullptr)
    {
        return false;
    }

    auto it = std::find(
        incomingRoads.begin(),
        incomingRoads.end(),
        road);

    if (it == incomingRoads.end())
    {
        return false;
    }

    incomingRoads.erase(it);

    return true;
}

bool Intersection::removeOutgoingRoad(Road* road)
{
    if (road == nullptr)
    {
        return false;
    }

    auto it = std::find(
        outgoingRoads.begin(),
        outgoingRoads.end(),
        road);

    if (it == outgoingRoads.end())
    {
        return false;
    }

    outgoingRoads.erase(it);

    return true;
}

