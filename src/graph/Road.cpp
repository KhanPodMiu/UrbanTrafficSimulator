#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include <stdexcept> // to use invalid_argument in line 31, 39

#include <algorithm> // std::clamp, std::max

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
//
//  Validates all parameters before constructing the object:
//  - source and destination must not be nullptr (consistent with setters).
//  - distance must be within [MIN_DISTANCE, MAX_DISTANCE].
//  - speedLimit must be within [MIN_SPEED_LIMIT, MAX_SPEED_LIMIT].
//  Throws std::invalid_argument on any violation.
// ─────────────────────────────────────────────────────────────────────────────
Road::Road(
    const std::string& id,
    const Intersection*      source,
    const Intersection*      destination,
    int                distance,
    int                speedLimit)
    :
    roadId(id),
    sourceIntersection(source),
    destinationIntersection(destination),
    distance(distance),
    speedLimit(speedLimit),
    congestionLevel(MIN_CONGESTION),
    travelCost(0),
    trafficLightState(TrafficLightState::GREEN),
    trafficLightEnabled(false),
    greenDuration(DEFAULT_GREEN_DURATION),
    yellowDuration(DEFAULT_YELLOW_DURATION),
    redDuration(DEFAULT_RED_DURATION),
    timeRemaining(DEFAULT_GREEN_DURATION)
{   
        // ── id validation ────────────────────────────────────────────────────────
    if (id.empty())
    {
        throw std::invalid_argument("Road: id must not be empty");
    }

    // ── intersection nullptr validation ──────────────────────────────────────
    // Consistent with setSourceIntersection / setDestinationIntersection which
    // reject nullptr.  A Road without endpoints is not meaningful.
    if (source == nullptr)
    {
        throw std::invalid_argument(
            "Road \"" + id + "\": source intersection must not be nullptr");
    }

    if (destination == nullptr)
    {
        throw std::invalid_argument(
            "Road \"" + id + "\": destination intersection must not be nullptr");
    }

    // ── distance validation ──────────────────────────────────────────────────
    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE)
    {
        throw std::invalid_argument(
            "Road \"" + id + "\": distance " + std::to_string(distance) +
            " is outside valid range [" +
            std::to_string(MIN_DISTANCE) + ", " + std::to_string(MAX_DISTANCE) + "]");
    }

    if (speedLimit < MIN_SPEED_LIMIT || speedLimit > MAX_SPEED_LIMIT)
    {
        throw std::invalid_argument(
            "Road \"" + id + "\": speedLimit " + std::to_string(speedLimit) +
            " is outside valid range [" +
            std::to_string(MIN_SPEED_LIMIT) + ", " + std::to_string(MAX_SPEED_LIMIT) + "]");
    }

    //Fix the distance and speed limit to be within the valid range, so we can guarantee that these variantes are valid for the function.
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

const Intersection* Road::getSourceIntersection() const
{
    return sourceIntersection;
}

const Intersection* Road::getDestinationIntersection() const
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
//  Traffic light getters
// ─────────────────────────────────────────────────────────────────────────────

TrafficLightState Road::getTrafficLightState() const
{
    return trafficLightState;
}

bool Road::isTrafficLightEnabled() const
{
    return trafficLightEnabled;
}

bool Road::isGreen() const
{
    return !trafficLightEnabled ||
           trafficLightState == TrafficLightState::GREEN;
}

int Road::getTimeRemaining() const
{
    return timeRemaining;
}

int Road::getGreenDuration() const
{
    return greenDuration;
}

int Road::getYellowDuration() const
{
    return yellowDuration;
}

int Road::getRedDuration() const
{
    return redDuration;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Setters
// ─────────────────────────────────────────────────────────────────────────────

bool Road::setSourceIntersection(const Intersection* source)
{
    if (source == nullptr)
    {
        return false;
    }

    sourceIntersection = source;
    return true;
}

bool Road::setDestinationIntersection(const Intersection* destination)
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
    if (!calculateTravelCost())
    {
        return false;
    }
    return true;
}

bool Road::setSpeedLimit(int speedLimit)
{
    if (speedLimit < MIN_SPEED_LIMIT || speedLimit > MAX_SPEED_LIMIT)
    {
        return false;
    }

    this->speedLimit = speedLimit;
    if (!calculateTravelCost())
    {
        return false;
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Traffic light setters
// ─────────────────────────────────────────────────────────────────────────────

bool Road::setGreenDuration(int duration)
{
    if (duration < MIN_DURATION || duration > MAX_DURATION)
    {
        return false;
    }
    greenDuration = duration;
    return true;
}

bool Road::setYellowDuration(int duration)
{
    if (duration < MIN_DURATION || duration > MAX_DURATION)
    {
        return false;
    }
    yellowDuration = duration;
    return true;
}

bool Road::setRedDuration(int duration)
{
    if (duration < MIN_DURATION || duration > MAX_DURATION)
    {
        return false;
    }
    redDuration = duration;
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
    if (!calculateTravelCost())
    {
        return false;
    }
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

// ─────────────────────────────────────────────────────────────────────────────
//  Traffic light operations
// ─────────────────────────────────────────────────────────────────────────────

void Road::resetTrafficLight()
{
    trafficLightState = TrafficLightState::GREEN;
    timeRemaining = greenDuration;
}

bool Road::needsTrafficLightAtDestination() const
{
    if (destinationIntersection == nullptr)
    {
        return false;
    }

    // getDegree() == 0 means isolated intersection → no light needed.
    if (destinationIntersection->getDegree() == 0)
    {
        return false;
    }

    // Only T_INTERSECTION and CROSS get signalized traffic lights.
    // ROUNDABOUT uses yield-style right-of-way instead, and DEAD_END (1) /
    // STRAIGHT (2) never need one.
    IntersectionType type = destinationIntersection->getType();
    return type == IntersectionType::T_INTERSECTION ||
           type == IntersectionType::CROSS;
}
