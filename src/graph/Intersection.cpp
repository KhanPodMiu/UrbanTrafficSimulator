#include "graph/Intersection.hpp"
#include <algorithm>
#include <stdexcept>

constexpr int MAP_WIDTH = 4000;
constexpr int MAP_HEIGHT = 4000;

Intersection::Intersection(
    std::string id,
    int                x,
    int                y)
    :
    intersectionID(id),
    x(x),
    y(y)
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


const std::vector<const Road*>& Intersection::getIncomingRoads() const
{
    return incomingRoads;
}

const std::vector<const Road*>& Intersection::getOutgoingRoads() const
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

IntersectionType Intersection::getType() const
{
    const int degree = getDegree();
    if (degree == 0)
    {
        throw std::logic_error("Isolated intersection");
    }

    if (degree == 1)
    {
        return IntersectionType::DEAD_END;
    }

    if (degree == 2)
    {
        return IntersectionType::STRAIGHT;
    }

    if (degree == 3)
    {
        return IntersectionType::T_INTERSECTION;
    }

    if (degree == 4)
    {
        return IntersectionType::CROSS;
    }

    return IntersectionType::ROUNDABOUT;
}

bool Intersection::addIncomingRoad(const Road* road)
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

bool Intersection::addOutgoingRoad(const Road* road)
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

bool Intersection::removeIncomingRoad(const Road* road)
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
        return true;
    }

    incomingRoads.erase(it);

    return true;
}

bool Intersection::removeOutgoingRoad(const Road* road)
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
        return true;
    }

    outgoingRoads.erase(it);

    return true;
}

