#include "algorithms/AStarStrategy.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

// AStarStrategy::AStarStrategy(
//     HeuristicType heuristic)
//     : heuristicType(heuristic)
// {
// }

double AStarStrategy::heuristic(const Intersection& current, const Intersection& goal) const {
    return std::hypot(current.getX() - goal.getX(), current.getY() - goal.getY()) / Road::MAX_SPEED_LIMIT;
}

//========= Use for benchmarking heuristics ========
// double AStarStrategy::heuristic(
//     const Intersection& current,
//     const Intersection& goal) const
// {
//     double dx =
//         std::abs(
//             current.getX() -
//             goal.getX());

//     double dy =
//         std::abs(
//             current.getY() -
//             goal.getY());

//     switch (heuristicType)
//     {
//     case HeuristicType::Zero:
//         return 0.0;

//     case HeuristicType::Euclidean:
//         return std::hypot(dx, dy);

//     case HeuristicType::WeightedEuclidean:
//         return 2.0 * std::hypot(dx, dy);

//     case HeuristicType::DivideSpeed:
//         return std::hypot(dx, dy)/ Road::MAX_SPEED_LIMIT;
//     }

//     return 0.0;
// }

RouteResult AStarStrategy::reconstructPath(
    const std::unordered_map<std::string,std::string>& cameFrom,
    const std::string& start,
    const std::string& goal) const
{
    std::vector<std::string> path;

    std::string current = goal;

    path.push_back(current);

    while(current != start)
    {
        auto it = cameFrom.find(current);

        if(it == cameFrom.end())
            return RouteResult();

        current = it->second;
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());

    return RouteResult(path);
}


RouteResult AStarStrategy::calculateRoute(
    const Graph& graph,
    const RouteRequest& request) const
{
    expandedNodes = 0;
    lastTravelCost = 0.0;

    auto start = graph.getIntersection(request.startIntersectionID);
    auto goal  = graph.getIntersection(request.destinationIntersectionID);

    if(!start || !goal)
        return RouteResult();

    using OpenSet = std::priority_queue<Node, std::vector<Node>, Compare>;
    OpenSet openSet;

    std::unordered_map<std::string,double> gScore;
    std::unordered_map<std::string,std::string> cameFrom;

    constexpr double INF = std::numeric_limits<double>::infinity();
    gScore[start->getIntersectionID()] = 0.0;

    const double h = heuristic(*start, *goal);
    openSet.push(
    {
        start->getIntersectionID(),
        0.0,
        h,
        h
    });
    while(!openSet.empty())
    {
        const Node current = openSet.top();
        openSet.pop();

        /*
            Lazy deletion.

            Ignore outdated queue entries.
        */

        const double currentScore = gScore[current.intersectionID];

        if(current.g > currentScore)
            continue;

        expandedNodes++;

        if(current.intersectionID == goal->getIntersectionID()) {
            lastTravelCost = currentScore;
            return reconstructPath(cameFrom, start->getIntersectionID(), goal->getIntersectionID());
        }

        const auto& roads = graph.getConnectedRoads(current.intersectionID);

        for(const auto& road : roads) {
            const Intersection* neighbor = road->getDestinationIntersection();
            if(neighbor == nullptr)
                continue;

            double tentative = currentScore + road->getTravelCost();
            auto it = gScore.find(neighbor->getIntersectionID());
            double oldScore =(it == gScore.end()) ? INF : it->second;

            if(tentative < oldScore) {
                cameFrom[neighbor->getIntersectionID()] = current.intersectionID;
                gScore[neighbor->getIntersectionID()] = tentative;
                double h = heuristic(*neighbor,*goal);
                openSet.push({neighbor->getIntersectionID(), tentative, h, tentative + h});
            }
        }
    }

    return RouteResult();
}