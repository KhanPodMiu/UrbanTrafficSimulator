#include <fstream>
#include <iostream>
#include <json/json.h>
#include <memory>
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include "utils/MapLoader.hpp"

bool MapLoader::loadFromJson(const std::string& filename, Graph& graph){
    std::ifstream in(filename, std::ifstream::binary);

    if(!in.is_open()){
        std::cerr << "Error to open " << filename << std::endl;
        return false;
    }

    Json::Value data;
    Json::CharReaderBuilder readerBuilder;
    std::string err;

    if(!Json::parseFromStream(readerBuilder, in, &data, &err)){
        std::cerr << err;
        return false;
    }

    if (!data.isMember("intersections") || !data.isMember("roads")){
        std::cerr << "Invalid map format\n";
        return false;
    }

    const Json::Value intersections = data["intersections"];
    for(const auto& intersection : intersections){
        if(
        !graph.addIntersection(std::make_shared<Intersection>(intersection["id"].asString(), intersection["x"].asInt(), intersection["y"].asInt()))
        )
        {
            std::cerr << "Duplicate intersection ID..... Please check your Json map\n";
            return false;
        }
    }

    const Json::Value Roads = data["roads"];
    for(const auto& road : Roads){
        std::string roadID = road["id"].asString();
        int distance = road["distance"].asInt();
        int speedLimit = road["speedLimit"].asInt();

        auto src = graph.getIntersection(road["source"].asString());
        auto des = graph.getIntersection(road["destination"].asString());

        if(!src || !des){
            std:: cerr << "Missing intersection\n";
            return false;
        }

        if(
            !graph.addRoad(std::make_shared<Road>(roadID, src.get(), des.get(), distance, speedLimit))
        )
        {
            std::cerr << "Duplicate road ID\n";
            return false;
        }
    }

    return true;
}

