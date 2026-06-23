#include <iostream>

#include "BFS.hpp"
#include "Graph.hpp"
#include "Road.hpp"
#include "Intersection.hpp"

#include <queue>
#include <unordered_map>
#include <algorithm>

RouteResult BFS::findShortestPath(const Graph& graph, const RouteRequest& request) {
    std::string start = request.startIntersectionID;
    std::string dest = request.destinationIntersectionID;

    std::vector<std::string> path;

    if (start == dest) {
        path.push_back(start);
        return RouteResult(path);
    }

    std::queue <std::string> q;
    std::unordered_map<std::string, bool> visited;            
    std::unordered_map<std::string, std::string> trace;

    q.push(start);
    visited[start] = true; 

    while (!q.empty())
    {
        std::string currentID = q.front();
        q.pop();
        if (currentID == dest) {
            break;
        }
        
        std::vector <std::shared_ptr <Road>> connectedRoads = graph.getConnectedRoads(currentID);
        for (const auto &road : connectedRoads) {

            if (!road) continue;
            const Intersection* neighborIntersection = road->getDestinationIntersection();

            if (!neighborIntersection) continue;
            std::string neighborID = neighborIntersection->getIntersectionID();

            if (!visited[neighborID])
            {
                visited[neighborID] = true; 
                q.push(neighborID);
                trace[neighborID] = currentID;
            }
        }
    }

    if (!visited[dest]) {
        return RouteResult(path);
    }

    std::string temp = dest;
    while (temp != start)
    {
        path.push_back(temp);
        temp = trace[temp];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return RouteResult(path);
}