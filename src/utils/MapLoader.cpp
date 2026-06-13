#include <fstream>
#include <iostream>
#include <json/json.h>
#include <memory>
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

#include "utils/MapLoader.hpp"

bool MapLoader::loadFromJson(const std::string& filename, Graph& graph){
    std::ifstream of(filename, std::ifstream::binary);

    if(!of.is_open()){
        std::cerr << "Error to open " << filename << std::endl;
        return 1;
    }

    Json::Value data;
    Json::CharReaderBuilder readerBuilder;
    std::string err;

    if(!Json::parseFromStream(readerBuilder, of, &data, &err)){
        std::cerr << err;
        return 1;
    }

    const Json::Value Intersections = data["intersections"];
    for(const auto& intersection : Intersections){
        
    }
}

