#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include "../simulation/TrafficLightManager.hpp"
#include "../graph/Graph.hpp"
#include "../graph/Intersection.hpp"
#include "../graph/Road.hpp"

void normalize(Vector2 &V) {
    double len = std::sqrt(V.x * V.x + V.y * V.y);
    if (len > 0.0) {
        V.x /= len;
        V.y /= len;
    }
}
double TrafficLightManager::calculateDotProduct(const Vector2& v1, const Vector2& v2) const {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

Vector2 TrafficLightManager::getIncomingRoadDirection(const Intersection* intersection, const Road* road) const {
    Vector2 dir;

    const Intersection* otherIntersection = nullptr;
    
    if (road->getSourceIntersection() == intersection) {
        otherIntersection = road->getDestinationIntersection();
    } 
    else {
        otherIntersection = road->getSourceIntersection();
    }

    if (otherIntersection != nullptr) {
        dir.x = otherIntersection->getX() - intersection->getX();
        dir.y = otherIntersection->getY() - intersection->getY();
        normalize(dir);
    }

    return dir;
}


bool TrafficLightManager::applyPhaseStates(IntersectionControl& control) {
    if (control.phases.empty()) return false;

    for (size_t i = 0; i < control.phases.size(); ++i) {
        for (auto& road : control.phases[i].roads) {
            if (road) {
                if (i == control.currentPhaseIndex) {
                    road->trafficLightState = control.currentLightState;
                } else {
                    road->trafficLightState = TrafficLightState::RED;
                }
            }
        }
        
    }
    return true;
}


bool TrafficLightManager::initializeTopology(Graph& graph) {
    m_signalizedIntersections.clear();

    graph.forEachIntersection([&](const std::shared_ptr<Intersection>& intersection) {

        if (intersection->getType() == IntersectionType::CROSS || 
            intersection->getType() == IntersectionType::T_INTERSECTION) {
            
            IntersectionControl control;
            control.intersectionID = intersection->getIntersectionID();
            control.type = intersection->getType();
            control.currentPhaseIndex = 0;
            control.currentLightState = TrafficLightState::GREEN;
            control.timer = 0.0;

            std::vector<std::shared_ptr<Road>> unassignedRoads;
            for (const Road* rawRoad : intersection->getIncomingRoads()) {
                if (rawRoad) {
                    auto sharedRoad = graph.getRoad(rawRoad->getRoadId());
                    if (sharedRoad) 
                        unassignedRoads.push_back(sharedRoad);
                }
            }

            while (!unassignedRoads.empty()) {
                SignalPhase newPhase;
                auto baseRoad = unassignedRoads.front();
                newPhase.roads.push_back(baseRoad);
                unassignedRoads.erase(unassignedRoads.begin());

                Vector2 baseDir = getIncomingRoadDirection(intersection.get(), baseRoad.get());

                for (auto it = unassignedRoads.begin(); it != unassignedRoads.end(); ) {
                    Vector2 compareDir = getIncomingRoadDirection(intersection.get(), (*it).get());
                    double dot = calculateDotProduct(baseDir, compareDir);

                    if (dot < -0.8) { 
                        newPhase.roads.push_back(*it); 
                        it = unassignedRoads.erase(it); 
                        break; 
                    } 
                    else {
                        ++it;
                    }
                }
                
                control.phases.push_back(newPhase);
            }

            applyPhaseStates(control);
            m_signalizedIntersections.push_back(control);
        }
    }); 
    
    return true; 
}

void TrafficLightManager::update(double dt) {
    for (auto& control : m_signalizedIntersections) {
        
        control.timer += dt;

        if (control.currentLightState == TrafficLightState::GREEN) {
            if (control.timer >= control.greenDuration) {
                control.currentLightState = TrafficLightState::YELLOW;
                control.timer = 0.0;
                applyPhaseStates(control);
            }
        } 
        else if (control.currentLightState == TrafficLightState::YELLOW) {
            if (control.timer >= control.yellowDuration) {
                control.currentPhaseIndex = (control.currentPhaseIndex + 1) % control.phases.size();
                control.currentLightState = TrafficLightState::GREEN;
                control.timer = 0.0;
                applyPhaseStates(control);
            }
        }
    }
}