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
    for (const auto& intersection : intersections) {
        std::string id = intersection["id"].asString();
        if (graph.getIntersection(id) != nullptr) {
            continue; 
        }
        auto newIntersection = std::make_shared<Intersection>(
            id, 
            intersection["x"].asInt(), 
            intersection["y"].asInt()
        );
        if (!graph.addIntersection(newIntersection)) {
            std::cerr << "Duplicate intersection ID..... Please check your Json map\n";
            return false;
        }
    }

    // 2. Reset trạng thái VIP của tất cả đường hiện tại về false trước khi apply file mới
    for (auto& [id, roadPtr] : graph.getRoads()) {
        if (roadPtr) {
            roadPtr->setVIPExclusive(false);
        }
    }

    // 3. Tải hoặc cập nhật trạng thái Roads
    const Json::Value Roads = data["roads"];
    for(const auto& road : Roads){
        std::string roadID = road["id"].asString();
        bool isBanned = false;
        if (road.isMember("isVIPExclusive")) {
            isBanned = road["isVIPExclusive"].asBool();
        }
        std::shared_ptr<Road> existingRoad = graph.getRoad(roadID);
        if (existingRoad != nullptr) {
            existingRoad->setVIPExclusive(isBanned);
            continue;
        }

        auto src = graph.getIntersection(road["source"].asString());
        auto des = graph.getIntersection(road["destination"].asString());

        if(!src || !des){
            std:: cerr << "Missing intersection\n";
            return false;
        }

        auto newRoad = std::make_shared<Road>(roadID, src.get(), des.get(), Road::MAX_SPEED_LIMIT);
        newRoad->setVIPExclusive(isBanned); 

        if (!graph.addRoad(newRoad)) {
            std::cerr << "[WARNING] Duplicate road ID skipped: " << roadID << "\n";
        }
    }

    return true;
}