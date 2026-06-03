#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <algorithm> // std::clamp, std::max

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
//
//  Out-of-range distance / speedLimit values are *clamped* here rather than
//  rejected.  Rationale: the object must always be in a consistent state
//  after construction; the caller can fix values via setters afterwards.
//  nullptr intersections are accepted – Road is not the owner.
// ─────────────────────────────────────────────────────────────────────────────
Road::Road(
    const std::string& id,
    Intersection*      source,
    Intersection*      destination,
    int                distance,
    int                speedLimit)
    :
    roadId(id),
    sourceIntersection(source),
    destinationIntersection(destination),
    distance(std::clamp(distance,   MIN_DISTANCE,    MAX_DISTANCE)),
    speedLimit(std::clamp(speedLimit, MIN_SPEED_LIMIT, MAX_SPEED_LIMIT)),
    congestionLevel(MIN_CONGESTION),
    travelCost(0)
{
    calculateTravelCost();
}

Road::~Road()
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  Getters
// ─────────────────────────────────────────────────────────────────────────────

const std::string& Road::getRoadId() const
{
    return roadId;
}

Intersection* Road::getSourceIntersection() const
{
    return sourceIntersection;
}

Intersection* Road::getDestinationIntersection() const
{
    return destinationIntersection;
}

int Road::getDistance() const
{
    return distance;
}

int Road::getSpeedLimit() const
{
    return speedLimit;
}

int Road::getCongestionLevel() const
{
    return congestionLevel;
}

int Road::getTravelCost() const
{
    return travelCost;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Setters
// ─────────────────────────────────────────────────────────────────────────────

bool Road::setSourceIntersection(Intersection* source)
{
    if (source == nullptr)
    {
        return false;
    }

    sourceIntersection = source;
    return true;
}

bool Road::setDestinationIntersection(Intersection* destination)
{
    if (destination == nullptr)
    {
        return false;
    }

    destinationIntersection = destination;
    return true;
}

bool Road::setDistance(int distance)
{
    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE)
    {
        return false;
    }

    this->distance = distance;
    calculateTravelCost();
    return true;
}

bool Road::setSpeedLimit(int speedLimit)
{
    if (speedLimit < MIN_SPEED_LIMIT || speedLimit > MAX_SPEED_LIMIT)
    {
        return false;
    }

    this->speedLimit = speedLimit;
    calculateTravelCost();
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Road operations
// ─────────────────────────────────────────────────────────────────────────────

bool Road::updateCongestion(int newCongestionLevel)
{
    if (newCongestionLevel < MIN_CONGESTION || newCongestionLevel > MAX_CONGESTION)
    {
        return false;
    }

    congestionLevel = newCongestionLevel;
    calculateTravelCost();
    return true;
}

bool Road::calculateTravelCost()
{
    if (speedLimit <= 0)
    {
        // Guard against division by zero; should never happen in normal use
        // because setSpeedLimit and the constructor both enforce MIN_SPEED_LIMIT.
        return false;
    }

    // ── Formula ──────────────────────────────────────────────────────────────
    //
    //   travelCost = distance × (100 + congestionLevel) / speedLimit
    //
    //   Why integers only?
    //   • SDL2 works in integer coordinates; mixing floats risks accumulation
    //     errors when costs are summed over many edges.
    //   • On a 4 000×4 000 map the maximum numerator is 8 000 × 200 = 1 600 000,
    //     safely within int32.
    //   By the way, I(Kevin) guessed this formula will slightly increase our compile and execute our project a little bit. 
    //
    //   std::max(1, …) prevents zero-weight edges on very short / fast roads
    //   (e.g. distance=1, speedLimit=130 → raw result = 0).
    //   A weight of 0 would allow Dijkstra to visit cycles at no cost.
    // ─────────────────────────────────────────────────────────────────────────
    travelCost = std::max(1, (distance * (100 + congestionLevel)) / speedLimit);
    return true;
}