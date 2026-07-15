#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include "../utils/vector2i.hpp"

class Graph;
class Intersection;
class Road;
enum class TrafficLightState; 
enum class IntersectionType;

class TrafficLightManager {
private:
    TrafficLightManager() = default;
    ~TrafficLightManager() = default;
    struct SignalPhase {
        std::vector<std::shared_ptr<Road>> roads; 
    };
    
    struct IntersectionControl {
        std::string intersectionID;
        IntersectionType type;
        std::vector<SignalPhase> phases;         
        size_t currentPhaseIndex = 0;             
        TrafficLightState currentLightState;      
        double timer = 0.0;                      
        static constexpr int greenDuration = 30.0;
        static constexpr int yellowDuration = 5.0;
        static constexpr int redDuration = 30.0; 
    };
   
    std::vector<IntersectionControl> m_signalizedIntersections;
    double calculateDotProduct(const Vector2& v1, const Vector2& v2) const;
    Vector2 getIncomingRoadDirection(const Intersection* intersection, const Road* road) const;
    bool applyPhaseStates(IntersectionControl& control);

public:
    static TrafficLightManager& getInstance() {
        static TrafficLightManager instance;
        return instance;
    };
    TrafficLightManager(const TrafficLightManager&) = delete;
    TrafficLightManager& operator=(const TrafficLightManager&) = delete;
    bool initializeTopology(Graph& graph);
    void update(double dt);
};

void normalize(Vector2 &V);