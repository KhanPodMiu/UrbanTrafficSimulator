#include <iostream>

#include "algorithms/Dijkstra.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <queue>
#include <unordered_map>
#include <algorithm>

struct NodeIntersection {
    std::string ID;
    int weight;
    bool operator < (const NodeIntersection& other) const { 
        return weight > other.weight; 
    }
};

RouteResult Dijkstra::calculateRoute(const Graph& graph, const RouteRequest& request) const {
    std::string start = request.startIntersectionID;
    std::string dest = request.destinationIntersectionID;
    std::vector<std::string> path;

    if (start == dest) {
        path.push_back(start);
        return RouteResult(path);
    }

    std::priority_queue <NodeIntersection> pq;
    //std::unordered_map<std::string, bool> visited;            
    std::unordered_map<std::string, int> cost;
    std::unordered_map<std::string, std::string> trace;

    cost[start] = 0;
    pq.push({start, 0});

    while (!pq.empty())
    {
        NodeIntersection currentIntersection = pq.top();
        std::string currentID = currentIntersection.ID;
        int currentCost = currentIntersection.weight;
        pq.pop();
        
        if (currentID == dest) break;


        if (cost.find(currentID) != cost.end() && currentCost > cost[currentID]) {
            continue;
        }

        std::vector <std::shared_ptr <Road>> connectedRoads = graph.getConnectedRoads(currentID);
        for (const auto &road : connectedRoads) {

            if (!road) continue;
            const Intersection* neighborIntersection = road->getDestinationIntersection();

            if (!neighborIntersection) continue;
            std::string neighborID = neighborIntersection->getIntersectionID();

            int neighborCost = road->getTravelCost();
            int newCost = currentCost + neighborCost;

            if (cost.find(neighborID) == cost.end() || newCost < cost[neighborID]) {
                cost[neighborID] = newCost;
                trace[neighborID] = currentID;
                pq.push({neighborID, newCost});
            }
        }
    }   
    
    if (trace.find(dest) == trace.end()) { 
        return RouteResult(path); 
    }

    std::string temp = dest;
    while (temp != start) {
        path.push_back(temp);
        temp = trace[temp];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());


    return RouteResult(path);
}
