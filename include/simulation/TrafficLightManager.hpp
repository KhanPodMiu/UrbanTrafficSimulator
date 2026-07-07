#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cmath>

class Graph;
class Intersection;
class Road;
enum class TrafficLightState; 
enum class IntersectionType;

class TrafficLightManager {
private:
    TrafficLightManager() = default;
    ~TrafficLightManager() = default;

    struct Vector2D {
        double x = 0.0;
        double y = 0.0;
        
        void normalize() {
            double len = std::sqrt(x * x + y * y);
            if (len > 0.0) {
                x /= len;
                y /= len;
            }
        }
    };

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
        double greenDuration = 30.0;
        double yellowDuration = 5.0;
        double redDuration = 35.0; 
    };
   
    std::vector<IntersectionControl> m_signalizedIntersections;
    double calculateDotProduct(const Vector2D& v1, const Vector2D& v2) const;
    Vector2D getIncomingRoadDirection(const Intersection* intersection, const Road* road) const;
    void applyPhaseStates(IntersectionControl& control);

public:
    static TrafficLightManager& getInstance() {
        static TrafficLightManager instance;
        return instance;
    };
    TrafficLightManager(const TrafficLightManager&) = delete;
    TrafficLightManager& operator=(const TrafficLightManager&) = delete;
    void initializeTopology(Graph& graph);
    void update(double dt);

};