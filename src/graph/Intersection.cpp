#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

constexpr int MAP_WIDTH = 4000;
constexpr int MAP_HEIGHT = 4000;

const int WIDTH = 40;
const int ROAD_PADDING = 20;
const int ROUNDABOUT_RADIUS = 80;
const int EXPAND_ROUNDABOUT = WIDTH / 2;

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
const std::string& Intersection::getIntersectionID() const
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

int Intersection::getRadius() const {
    int neighbor = getNeighborCount();

    //Roundabout
    if (neighbor > 4) {
        return ROUNDABOUT_RADIUS + (neighbor - 4)*EXPAND_ROUNDABOUT + ROAD_PADDING;
    }
    //Non-Roundabout
    if (getDegree() > neighbor)
        return WIDTH + ROAD_PADDING;
    return  WIDTH / 2 + ROAD_PADDING;
}

int Intersection::getNeighborCount() const
{
    std::unordered_set<const Intersection*> neighbors;

    for (const Road* road : incomingRoads)
    {
        if (road != nullptr)
        {
            const Intersection* source = road->getSourceIntersection();

            if (source != nullptr && source != this)
            {
                neighbors.insert(source);
            }
        }
    }

    for (const Road* road : outgoingRoads)
    {
        if (road != nullptr)
        {
            const Intersection* destination = road->getDestinationIntersection();

            if (destination != nullptr && destination != this)
            {
                neighbors.insert(destination);
            }
        }
    }

    return static_cast<int>(neighbors.size());
}

IntersectionType Intersection::getType() const
{
    switch (getNeighborCount())
    {
        case 0:
            throw std::logic_error(
                "Intersection has no connected roads."
            );

        case 1:
            return IntersectionType::DEAD_END;

        case 2:
            return IntersectionType::STRAIGHT;

        case 3:
            return IntersectionType::T_INTERSECTION;

        case 4:
            return IntersectionType::CROSS;

        default:
            return IntersectionType::ROUNDABOUT;
    }
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

