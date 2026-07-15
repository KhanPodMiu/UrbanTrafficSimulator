#pragma once
#ifndef ROAD_HPP
#define ROAD_HPP

#include <string>

class Intersection;

// ─────────────────────────────────────────────────────────────────────────────
//  TrafficLightState – embedded in Road because each Road owns its own light.
// ─────────────────────────────────────────────────────────────────────────────
enum class TrafficLightState {
    RED,
    YELLOW,
    GREEN
};

class Road {
    friend class TrafficLightManager;
public:
    // ── validation bounds (public so tests can reference them) ────────────────
    static constexpr int MIN_SPEED_LIMIT = 5;     // km/h
    static constexpr int MAX_SPEED_LIMIT = 130;   // km/h
    static constexpr int MIN_CONGESTION  = 0;
    static constexpr int MAX_CONGESTION  = 100;
    static constexpr int MIN_DISTANCE    = 1;     // map units
    static constexpr int MAX_DISTANCE    = 66400;  // ≈ 1.4× map diagonal

    // ── traffic light duration bounds ────────────────────────────────────────
    static constexpr int DEFAULT_GREEN_DURATION  = 30;  // seconds
    static constexpr int DEFAULT_YELLOW_DURATION = 5;   // seconds
    static constexpr int DEFAULT_RED_DURATION    = 30;  // seconds
    static constexpr int MIN_DURATION            = 1;   // seconds
    static constexpr int MAX_DURATION            = 120; // seconds

    // ── constructor / destructor ──────────────────────────────────────────────
    Road(
        const std::string& id,
        const Intersection*      source,
        const Intersection*      destination,
        int                distance,
        int                speedLimit
    );

    ~Road();

    // ── getters ───────────────────────────────────────────────────────────────
    // Returns a const-ref to avoid copying the string.
    const std::string& getRoadId()                  const;

    // Returns raw pointers – callers must NOT take ownership.
    const Intersection*      getSourceIntersection()      const;
    const Intersection*      getDestinationIntersection() const;

    int  getDistance()       const;
    int  getSpeedLimit()     const;
    int  getCongestionLevel()const;
    int  getTravelCost()     const;

    // ── traffic light getters ─────────────────────────────────────────────────
    TrafficLightState getTrafficLightState()  const;
    bool              isTrafficLightEnabled() const;
    bool              isGreen()              const;
    int               getTimeRemaining()     const;
    int               getGreenDuration()     const;
    int               getYellowDuration()    const;
    int               getRedDuration()       const;

    // ── setters ───────────────────────────────────────────────────────────────
    bool setSourceIntersection(const Intersection* source);
    bool setDestinationIntersection(const Intersection* destination);
    bool setDistance(int distance);     // accepted range: [MIN_DISTANCE,  MAX_DISTANCE ]
    bool setSpeedLimit(int speedLimit); // accepted range: [MIN_SPEED_LIMIT,MAX_SPEED_LIMIT]

    // ── traffic light setters ─────────────────────────────────────────────────
    bool setGreenDuration(int duration);
    bool setYellowDuration(int duration);
    bool setRedDuration(int duration);

    // ── road operations ───────────────────────────────────────────────────────

    // updateCongestion: sets congestionLevel and recomputes travelCost.
    // Returns false when newCongestionLevel is outside [MIN_CONGESTION, MAX_CONGESTION].
    bool updateCongestion(int newCongestionLevel);

    // calculateTravelCost: recomputes travelCost from current attributes.
    // Called automatically by every setter that touches a relevant attribute.
    // Returns false only if speedLimit is 0 (division-by-zero guard).
    bool calculateTravelCost();

    // ── traffic light operations ──────────────────────────────────────────────

    // Advance the traffic light timer by deltaTimeSeconds; auto-switches state.
    void updateTrafficLight(int deltaTimeSeconds);

    // Reset traffic light to GREEN with full timer.
    void resetTrafficLight();

    // Enable / disable the traffic light on this road.
    void enableTrafficLight();
    void disableTrafficLight();

    // Check whether the destination intersection's type requires a traffic
    // light (T_INTERSECTION, CROSS, or ROUNDABOUT → true).
    bool needsTrafficLightAtDestination() const;

private:
    std::string   roadId;

    const Intersection* sourceIntersection;
    const Intersection* destinationIntersection;

    int distance;        // map units      [MIN_DISTANCE,   MAX_DISTANCE  ]
    int speedLimit;      // km/h           [MIN_SPEED_LIMIT, MAX_SPEED_LIMIT]
    int congestionLevel; // dimensionless  [MIN_CONGESTION,  MAX_CONGESTION ]
    int travelCost;      // dimensionless weight, kept in sync by setters

    // ── traffic light state ──────────────────────────────────────────────────
    TrafficLightState trafficLightState;
    bool              trafficLightEnabled;
    int               greenDuration;
    int               yellowDuration;
    int               redDuration;
    int               timeRemaining;   // countdown timer in seconds

    // Internal helper: cycle to the next state (GREEN→YELLOW→RED→GREEN).
    void switchToNextState();
};

#endif // ROAD_HPP