#include <iostream>
#include <fstream>
#include <json/json.h>


int main() {
    std::string file_name = "assets/maps/data.json";
    std::ifstream file(file_name, std::ifstream::binary);

    if(!file.is_open()){
        std::cerr << "Error to open " << file_name << std::endl;
        return 1;
    }

    Json::Value data;
    Json::CharReaderBuilder readerBuilder;
    std::string err;

    if(!Json::parseFromStream(readerBuilder, file, &data, &err)){
        std::cerr << err;
        return 1;
    }

    const Json::Value students = data["students"];
    for(const auto& student : students){
        std::cout << student["name"].asString() << " " << student["age"].asInt() << '\n';
    }

    file.close();

    return 0;
}