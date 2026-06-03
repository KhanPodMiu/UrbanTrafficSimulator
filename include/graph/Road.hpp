#pragma once
#ifndef ROAD_HPP
#define ROAD_HPP

#include <string>

class Intersection;

class Road
{
public:
    // ── validation bounds (public so tests can reference them) ────────────────
    static constexpr int MIN_SPEED_LIMIT = 5;     // km/h
    static constexpr int MAX_SPEED_LIMIT = 130;   // km/h
    static constexpr int MIN_CONGESTION  = 0;
    static constexpr int MAX_CONGESTION  = 100;
    static constexpr int MIN_DISTANCE    = 1;     // map units
    static constexpr int MAX_DISTANCE    = 8000;  // ≈ 1.4× map diagonal

    // ── constructor / destructor ──────────────────────────────────────────────
    Road(
        const std::string& id,
        Intersection*      source,
        Intersection*      destination,
        int                distance,
        int                speedLimit
    );

    ~Road();

    // ── getters ───────────────────────────────────────────────────────────────
    // Returns a const-ref to avoid copying the string.
    const std::string& getRoadId()                  const;

    // Returns raw pointers – callers must NOT take ownership.
    Intersection*      getSourceIntersection()      const;
    Intersection*      getDestinationIntersection() const;

    int  getDistance()       const;
    int  getSpeedLimit()     const;
    int  getCongestionLevel()const;
    int  getTravelCost()     const;

    // ── setters ───────────────────────────────────────────────────────────────
    bool setSourceIntersection(Intersection* source);
    bool setDestinationIntersection(Intersection* destination);
    bool setDistance(int distance);     // accepted range: [MIN_DISTANCE,  MAX_DISTANCE ]
    bool setSpeedLimit(int speedLimit); // accepted range: [MIN_SPEED_LIMIT,MAX_SPEED_LIMIT]

    // ── road operations ───────────────────────────────────────────────────────

    // updateCongestion: sets congestionLevel and recomputes travelCost.
    // Returns false when newCongestionLevel is outside [MIN_CONGESTION, MAX_CONGESTION].
    bool updateCongestion(int newCongestionLevel);

    // calculateTravelCost: recomputes travelCost from current attributes.
    // Called automatically by every setter that touches a relevant attribute.
    // Returns false only if speedLimit is 0 (division-by-zero guard).
    bool calculateTravelCost();

private:
    std::string   roadId;

    Intersection* sourceIntersection;
    Intersection* destinationIntersection;

    int distance;        // map units      [MIN_DISTANCE,   MAX_DISTANCE  ]
    int speedLimit;      // km/h           [MIN_SPEED_LIMIT, MAX_SPEED_LIMIT]
    int congestionLevel; // dimensionless  [MIN_CONGESTION,  MAX_CONGESTION ]
    int travelCost;      // dimensionless weight, kept in sync by setters
};

#endif // ROAD_HPP