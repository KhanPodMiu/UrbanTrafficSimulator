#include "graph/Graph.hpp"         

#include "graph/Intersection.hpp"

#include "graph/Road.hpp"  

#include <iostream>

#include <algorithm>

#include <functional> //ADDED: Library for call back function

#include <stdexcept>




Graph::Graph() = default;



Graph::~Graph() = default;



//Add

bool Graph::addIntersection(std::shared_ptr <Intersection> nIntersection) { 

    if (nIntersection == nullptr) {

        return false;

    }

    if (Intersections.find(nIntersection->getIntersectionID()) != Intersections.end()) {

        return false;

    }

    Intersections[nIntersection->getIntersectionID()] = nIntersection;

    adjacencyList[nIntersection->getIntersectionID()] = std::vector<std::shared_ptr<Road>>();

    return true;

}



bool Graph::addRoad(std::shared_ptr <Road> nRoad) {

    if (nRoad == nullptr) {

        return false;

    }

    if (Roads.find(nRoad->getRoadId()) != Roads.end()) {

        return false;

    }

    const Intersection *source = nRoad->getSourceIntersection();

    const Intersection *dest = nRoad->getDestinationIntersection();

    if (source == nullptr || dest == nullptr) {

        return false;

    }

    const std::string sourceID = source->getIntersectionID();

    const std::string destID = dest->getIntersectionID();



    auto srcIt = Intersections.find(sourceID);

    auto destIt = Intersections.find(destID);

    

    if (srcIt == Intersections.end() || destIt == Intersections.end()) {

        return false;

    }

    

    srcIt->second->addOutgoingRoad(nRoad.get());

    destIt->second->addIncomingRoad(nRoad.get());

    

    Roads[nRoad->getRoadId()] = nRoad;

    adjacencyList[sourceID].push_back(nRoad);

    return true;

}



//Remove

bool Graph::removeIntersection(const std::string& intersectionID) {

    if (Intersections.find(intersectionID) == Intersections.end()) {

        return false;

    }

    std::vector<std::string> roadsToDelete;

    for (const auto& pair : Roads) {

        std::shared_ptr<Road> r = pair.second;

        if (r != nullptr) {

            const Intersection* source = r->getSourceIntersection();

            const Intersection* dest = r->getDestinationIntersection();

            if ((source && source->getIntersectionID() == intersectionID) || 

                (dest && dest->getIntersectionID() == intersectionID)) {

                roadsToDelete.push_back(r->getRoadId());

            }

        }

    }



    for (const std::string& rID : roadsToDelete) {

        removeRoad(rID);

    }

    Intersections.erase(intersectionID);

    adjacencyList.erase(intersectionID);

    

    return true;



}



bool Graph::removeRoad(const std::string& roadID) {

    auto itRoad = Roads.find(roadID);

    if (itRoad == Roads.end()) {

        return false;

    }

    

    std::shared_ptr<Road> r = itRoad->second;

    const Intersection* src = r->getSourceIntersection();

    const Intersection* dest = r->getDestinationIntersection();

    

    if (src != nullptr) {

        std::string sourceID = src->getIntersectionID();

        auto srcIt = Intersections.find(sourceID);

        if (srcIt != Intersections.end()) {

            srcIt->second->removeOutgoingRoad(r.get());

        }



        std::vector<std::shared_ptr<Road>>& connectedRoads = adjacencyList[sourceID];

        for (auto it = connectedRoads.begin(); it != connectedRoads.end(); ++it) {

            if ((*it)->getRoadId() == roadID) { 

                connectedRoads.erase(it);

                break;

            }

        }

    }



    if (dest != nullptr) {

        std::string destID = dest->getIntersectionID();

        auto destIt = Intersections.find(destID);

        if (destIt != Intersections.end()) {

            destIt->second->removeIncomingRoad(r.get());

        }

    }



    Roads.erase(itRoad);

    return true;

}



//Get

const std::vector <std::shared_ptr <Road>>& Graph::getConnectedRoads(const std::string& intersectionID) const {

    static const std::vector<std::shared_ptr<Road>> emptyVec;

    auto it = adjacencyList.find(intersectionID);

    if (it == adjacencyList.end()) {

        return emptyVec;

    }

    return it->second;

}



const std::shared_ptr<Intersection> Graph::getIntersection(const std::string& intersectionID) const {

    auto it = Intersections.find(intersectionID);

    if (it == Intersections.end()) {

        return nullptr; 

    }

    return it->second;

}

void Graph::forEachIntersection(std::function<void(const std::shared_ptr<Intersection>&)> func) const {
    for (const auto& pair : Intersections) {
        func(pair.second); 
    }
}

//ADDED: Call back function
void Graph::forEachIntersection(std::function<void(const std::shared_ptr<Intersection>&)> func) const {
    for (const auto& pair : Intersections) {
        func(pair.second); 
    }
}


const std::shared_ptr<Road> Graph::getRoad(const std::string& roadID) const {

    auto it = Roads.find(roadID);

    if (it == Roads.end()) {

        return nullptr; 

    }

    return it->second;

}



bool Graph::isValid() const {

    if (Intersections.size() != adjacencyList.size()) {

        return false; 

    }



    for (const auto& pair : Roads) {

        std::shared_ptr <Road> currentRoad = pair.second;

        if (currentRoad == nullptr) return false;



        const Intersection* source = currentRoad->getSourceIntersection();

        const Intersection* dest = currentRoad->getDestinationIntersection(); 



        if (source == nullptr || dest == nullptr) return false;



        std::string sourceID = source->getIntersectionID();

        std::string destID = dest->getIntersectionID();



        if (Intersections.find(sourceID) == Intersections.end() || 

            Intersections.find(destID) == Intersections.end()) {

            return false; 

        }



        const auto& connectedRoads = adjacencyList.at(sourceID);

        bool roadFoundInAdjacency = false;

        

        for (const auto& r : connectedRoads) {

            if (r->getRoadId() == currentRoad->getRoadId()) {

                roadFoundInAdjacency = true;

                break;

            }

        }

        

        if (!roadFoundInAdjacency) {

            return false; 

        }

    }



    return true; 

}

const std::unordered_map<std::string, std::shared_ptr<Intersection>>& Graph::getIntersections() const {
    return Intersections;
}

const std::unordered_map<std::string, std::shared_ptr<Road>>& Graph::getRoads() const {
    return Roads;
}