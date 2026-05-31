#include <iostream>
#include <fstream>
#include <json/json.h>


int main() {
    std::string file_path = "assets/maps/data.json";
    std::ifstream file(file_path, std::ifstream::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << file_path << std::endl;
        return 1;
    }

    Json::Value data;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    if (!Json::parseFromStream(readerBuilder, file, &data, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return 1;
    }
    
    std::string school_name = data["name"].asString();
    std::cout << "School Name: " << school_name << std::endl;
    
    std::string city = data["location"]["city"].asString();
    std::cout << "City: " << city << std::endl;

    std::string first_student_name = data["students"][0]["name"].asString();
    std::cout << "First Student's Name: " << first_student_name << std::endl;

    const Json::Value& students = data["students"];
    for(const auto& student : students){
        std::cout << student["name"].asString() << std::endl;
    }

    return 0;
}