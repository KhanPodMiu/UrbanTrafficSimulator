// =============================================================================
// Road.cpp
// Project : UrbanTrafficSimulator
// Purpose : Implements the Road class declared in Road.hpp.
// =============================================================================

#include "Road.hpp"

#include <iomanip>
#include <sstream>


// =============================================================================
// Private static validation helpers
// =============================================================================

void Road::validateId(const std::string& id, const std::string& fieldName) {
    if (id.empty()) {
        throw RoadException(fieldName + " must not be empty.");
    }
}

void Road::validatePositive(double value, const std::string& fieldName) {
    if (value <= 0.0) {
        std::ostringstream oss;
        oss << fieldName << " must be positive (received "
            << std::fixed << std::setprecision(6) << value << ").";
        throw RoadException(oss.str());
    }
}

void Road::validateCongestion(double level) {
    if (level < 0.0 || level > 1.0) {
        std::ostringstream oss;
        oss << "Congestion level must be in [0.0, 1.0] (received "
            << std::fixed << std::setprecision(6) << level << ").";
        throw RoadException(oss.str());
    }
}


// =============================================================================
// Constructor
// =============================================================================

/*
 * Design note on member initializer list vs. body validation
 * ----------------------------------------------------------
 * Members are initialised in the initializer list for efficiency (single
 * construction, no double-assignment).  Validation is performed in the body
 * immediately after.  If validation throws, the constructor never completes,
 * so the object is never observable in an invalid state.  All member types
 * (std::string, double) are safely destructible even with empty/zero values,
 * so the throw-during-construction unwind is clean.
 */
Road::Road(const std::string& roadId,
           const std::string& sourceIntersection,
           const std::string& destIntersection,
           double             distance,
           double             speedLimit,
           double             congestionLevel)
    : m_roadId(roadId)
    , m_sourceIntersection(sourceIntersection)
    , m_destIntersection(destIntersection)
    , m_distance(distance)
    , m_speedLimit(speedLimit)
    , m_congestionLevel(congestionLevel)
    , m_travelCost(0.0)   // computed by calculateTravelCost() below
{
    validateId(roadId,             "Road ID");
    validateId(sourceIntersection, "Source intersection ID");
    validateId(destIntersection,   "Destination intersection ID");
    validatePositive(distance,     "Distance");
    validatePositive(speedLimit,   "Speed limit");
    validateCongestion(congestionLevel);

    calculateTravelCost();   // sets m_travelCost
}


// =============================================================================
// Getters
// =============================================================================

const std::string& Road::getRoadId()             const noexcept { return m_roadId; }
const std::string& Road::getSourceIntersection() const noexcept { return m_sourceIntersection; }
const std::string& Road::getDestIntersection()   const noexcept { return m_destIntersection; }
double             Road::getDistance()           const noexcept { return m_distance; }
double             Road::getSpeedLimit()         const noexcept { return m_speedLimit; }
double             Road::getCongestionLevel()    const noexcept { return m_congestionLevel; }
double             Road::getTravelCost()         const noexcept { return m_travelCost; }


// =============================================================================
// Setters
// =============================================================================

void Road::setDistance(double distance) {
    validatePositive(distance, "Distance");
    m_distance = distance;
    calculateTravelCost();
}

void Road::setSpeedLimit(double speedLimit) {
    validatePositive(speedLimit, "Speed limit");
    m_speedLimit = speedLimit;
    calculateTravelCost();
}

void Road::setCongestionLevel(double level) {
    validateCongestion(level);
    m_congestionLevel = level;
    calculateTravelCost();
}


// =============================================================================
// Core domain operations
// =============================================================================

void Road::updateCongestion(double newLevel) {
    validateCongestion(newLevel);
    m_congestionLevel = newLevel;
    calculateTravelCost();
}

void Road::calculateTravelCost() {
    // -----------------------------------------------------------------
    // Linear congestion model
    //
    //   travelCost [h] = freeFlowTime [h] × congestionMultiplier
    //
    // where:
    //   freeFlowTime        = distance / speedLimit
    //   congestionMultiplier = 1.0 + congestionLevel
    //
    // Examples (distance = 100 km, speedLimit = 50 km/h):
    //   congestion 0.0  →  (100/50) × 1.0  = 2.00 h
    //   congestion 0.5  →  (100/50) × 1.5  = 3.00 h
    //   congestion 1.0  →  (100/50) × 2.0  = 4.00 h
    // -----------------------------------------------------------------
    m_travelCost = (m_distance / m_speedLimit) * (1.0 + m_congestionLevel);
}


// =============================================================================
// Utility
// =============================================================================

std::string Road::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "Road[" << m_roadId << "]: "
        << m_sourceIntersection << " -> " << m_destIntersection
        << " | dist="        << m_distance                  << " km"
        << " | limit="       << m_speedLimit                << " km/h"
        << " | congestion="  << (m_congestionLevel * 100.0) << "%"
        << " | travelCost="  << m_travelCost                << " h";
    return oss.str();
}