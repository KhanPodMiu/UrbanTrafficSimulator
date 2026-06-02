#pragma once

// =============================================================================
// Road.hpp
// Project : UrbanTrafficSimulator
// Purpose : Declares the Road entity representing a directed road segment
//           in the urban traffic network.
//
// Conventions
//   - distance     : kilometres (km)
//   - speedLimit   : kilometres per hour (km/h)
//   - congestion   : dimensionless, range [0.0, 1.0]
//                     0.0 = completely free flow
//                     1.0 = fully congested
//   - travelCost   : hours (h)
//
// Travel-cost formula (linear congestion model):
//   travelCost = (distance / speedLimit) × (1 + congestionLevel)
// =============================================================================

#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------
// RoadException
// Domain-specific exception thrown on any Road validation error.
// Inherits from std::invalid_argument so callers can catch either level.
// -----------------------------------------------------------------------------
class RoadException : public std::invalid_argument {
public:
    explicit RoadException(const std::string& message)
        : std::invalid_argument(message) {}
};

// -----------------------------------------------------------------------------
// Road
//
// Represents a directed road segment connecting two intersections.
// All mutating operations automatically keep travelCost in sync.
// -----------------------------------------------------------------------------
class Road {
public:
    // =========================================================================
    // Construction / Destruction
    // =========================================================================

    /**
     * @brief Construct a fully initialised Road.
     *
     * @param roadId               Non-empty unique road identifier.
     * @param sourceIntersection   Non-empty ID of the origin  intersection.
     * @param destIntersection     Non-empty ID of the destination intersection.
     * @param distance             Segment length   [km]  – must be > 0.
     * @param speedLimit           Posted speed     [km/h] – must be > 0.
     * @param congestionLevel      Initial congestion in [0.0, 1.0].
     *                             Defaults to 0.0 (free flow).
     *
     * @throws RoadException if any parameter fails validation.
     */
    Road(const std::string& roadId,
         const std::string& sourceIntersection,
         const std::string& destIntersection,
         double             distance,
         double             speedLimit,
         double             congestionLevel = 0.0);

    ~Road() = default;

    // Value semantics: copy / move both OK – no heap ownership.
    Road(const Road&)            = default;
    Road& operator=(const Road&) = default;
    Road(Road&&)                 = default;
    Road& operator=(Road&&)      = default;


    // =========================================================================
    // Getters
    // =========================================================================

    const std::string& getRoadId()             const noexcept;
    const std::string& getSourceIntersection() const noexcept;
    const std::string& getDestIntersection()   const noexcept;
    double             getDistance()           const noexcept;
    double             getSpeedLimit()         const noexcept;
    double             getCongestionLevel()    const noexcept;

    /**
     * @brief Return the most recently computed travel cost [h].
     *        Always up-to-date: every mutation triggers a recalculation.
     */
    double             getTravelCost()         const noexcept;


    // =========================================================================
    // Setters
    // =========================================================================

    /// @throws RoadException if value ≤ 0.
    void setDistance(double distance);

    /// @throws RoadException if value ≤ 0.
    void setSpeedLimit(double speedLimit);

    /// @throws RoadException if level ∉ [0.0, 1.0].
    void setCongestionLevel(double level);


    // =========================================================================
    // Core domain operations
    // =========================================================================

    /**
     * @brief Update congestion level and immediately recompute travel cost.
     *
     * Intended for use by the simulation engine when traffic conditions change.
     *
     * @param newLevel  New congestion in [0.0, 1.0].
     * @throws RoadException if newLevel ∉ [0.0, 1.0].
     */
    void updateCongestion(double newLevel);

    /**
     * @brief Recompute and persist travelCost from the current field values.
     *
     * Called automatically by all mutating operations; may also be invoked
     * explicitly when needed.
     *
     * Formula:
     *   travelCost [h] = (distance [km] / speedLimit [km/h])
     *                    × (1.0 + congestionLevel)
     *
     * Congestion multiplier interpretation:
     *   - 0.0  → ×1.0  (free flow, no penalty)
     *   - 0.5  → ×1.5  (50 % slower than free flow)
     *   - 1.0  → ×2.0  (fully congested, twice the free-flow time)
     */
    void calculateTravelCost();


    // =========================================================================
    // Utility
    // =========================================================================

    /// @brief Return a human-readable one-line description of this road.
    std::string toString() const;


private:
    // =========================================================================
    // Attributes
    // =========================================================================

    std::string m_roadId;
    std::string m_sourceIntersection;
    std::string m_destIntersection;
    double      m_distance;
    double      m_speedLimit;
    double      m_congestionLevel;
    double      m_travelCost;


    // =========================================================================
    // Private validation helpers (static – no Road state required)
    // =========================================================================

    /**
     * @brief Throw RoadException if id is empty.
     * @param id        The value to check.
     * @param fieldName Descriptive name used in the error message.
     */
    static void validateId(const std::string& id,
                           const std::string& fieldName);

    /**
     * @brief Throw RoadException if value ≤ 0.
     * @param value     The value to check.
     * @param fieldName Descriptive name used in the error message.
     */
    static void validatePositive(double             value,
                                 const std::string& fieldName);

    /**
     * @brief Throw RoadException if level ∉ [0.0, 1.0].
     * @param level  Congestion level to validate.
     */
    static void validateCongestion(double level);
};