// test_graph.cpp

#include <gtest/gtest.h>
#include "include/graph/Graph.hpp"
#include "include/graph/Intersection.hpp"
#include "include/graph/Road.hpp"
#include <memory>
#include <stdexcept>

// --- TEST HÀM ADD INTERSECTION ---

TEST(GraphTest, AddIntersection_Success) {
    Graph graph;
    // Tạo một nút giao giả lập
    auto inter1 = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);

    // Kỳ vọng chạy thành công không báo lỗi
    EXPECT_NO_THROW(graph.addIntersection(inter1));
    
    // Kiểm tra xem đã lấy ra được chưa
    EXPECT_EQ(graph.getIntersection("INT_A")->getIntersectionID(), "INT_A");
}

TEST(GraphTest, AddIntersection_Exceptions) {
    Graph graph;
    auto inter1 = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    graph.addIntersection(inter1); // Thêm lần 1 thành công

    // Lỗi 1: Thêm con trỏ null
    EXPECT_THROW(graph.addIntersection(nullptr), std::invalid_argument);

    // Lỗi 2: Thêm trùng ID đã có
    EXPECT_THROW(graph.addIntersection(inter1), std::invalid_argument);
}

// --- TEST HÀM ADD ROAD ---

TEST(GraphTest, AddRoad_Success) {
    Graph graph;
    // Phải tạo 2 ngã tư trước vì hàm addRoad bắt buộc sourceID và destID phải tồn tại
    auto interA = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    auto interB = std::make_shared<Intersection>("INT_B", 10, 10, IntersectionType::CROSS);
    graph.addIntersection(interA);
    graph.addIntersection(interB);

    // Giả định constructor của Road là (ID, source, dest, length, speedLimit)
    auto road1 = std::make_shared<Road>("ROAD_1", "INT_A", "INT_B", 100.0, 50.0);

    EXPECT_NO_THROW(graph.addRoad(road1));
    EXPECT_EQ(graph.getRoad("ROAD_1")->getRoadID(), "ROAD_1");
}

TEST(GraphTest, AddRoad_Exceptions) {
    Graph graph;
    auto interA = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    graph.addIntersection(interA);

    // Lỗi 1: Thêm con trỏ null
    EXPECT_THROW(graph.addRoad(nullptr), std::invalid_argument);

    // Lỗi 2: Thêm đường khi ngã tư Nguồn (Source) không tồn tại
    auto roadBadSource = std::make_shared<Road>("ROAD_BAD", "INT_GHOST", "INT_A", 100, 50);
    EXPECT_THROW(graph.addRoad(roadBadSource), std::invalid_argument);
}

// --- TEST HÀM REMOVE ROAD ---

TEST(GraphTest, RemoveRoad_Success) {
    Graph graph;
    auto interA = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    auto interB = std::make_shared<Intersection>("INT_B", 10, 10, IntersectionType::CROSS);
    graph.addIntersection(interA);
    graph.addIntersection(interB);
    
    auto road1 = std::make_shared<Road>("ROAD_1", "INT_A", "INT_B", 100.0, 50.0);
    graph.addRoad(road1);

    // Xóa thành công
    EXPECT_NO_THROW(graph.removeRoad("ROAD_1"));

    // Sau khi xóa, nếu cố tình Get lại thì phải văng lỗi
    EXPECT_THROW(graph.getRoad("ROAD_1"), std::invalid_argument);
    
    // Kiểm tra danh sách kề của INT_A xem đã trống chưa
    EXPECT_TRUE(graph.getConnectedRoads("INT_A").empty());
}

TEST(GraphTest, RemoveRoad_Exceptions) {
    Graph graph;
    // Lỗi: Xóa một con đường không hề tồn tại trong đồ thị
    EXPECT_THROW(graph.removeRoad("ROAD_GHOST"), std::invalid_argument);
}

// --- TEST HÀM REMOVE INTERSECTION ---

TEST(GraphTest, RemoveIntersection_Success) {
    Graph graph;
    auto interA = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    auto interB = std::make_shared<Intersection>("INT_B", 10, 10, IntersectionType::CROSS);
    graph.addIntersection(interA);
    graph.addIntersection(interB);

    auto road1 = std::make_shared<Road>("ROAD_1", "INT_A", "INT_B", 100.0, 50.0);
    graph.addRoad(road1);

    // Xóa ngã tư B
    EXPECT_NO_THROW(graph.removeIntersection("INT_B"));

    // INT_B phải biến mất
    EXPECT_THROW(graph.getIntersection("INT_B"), std::invalid_argument);
    
    // Quan trọng: Bị xóa kéo theo ROAD_1 nối vào B cũng phải biến mất theo
    EXPECT_THROW(graph.getRoad("ROAD_1"), std::invalid_argument);
}

// --- TEST HÀM GET (TRUY XUẤT) ---

TEST(GraphTest, GetMethods_Exceptions) {
    Graph graph;
    
    // Tìm ngã tư không tồn tại
    EXPECT_THROW(graph.getIntersection("GHOST_INT"), std::invalid_argument);
    
    // Tìm đường không tồn tại
    EXPECT_THROW(graph.getRoad("GHOST_ROAD"), std::invalid_argument);
    
    // Tìm danh sách kề của ngã tư không tồn tại
    EXPECT_THROW(graph.getConnectedRoads("GHOST_INT"), std::invalid_argument);
}

// --- TEST HÀM ISVALID ---

TEST(GraphTest, IsValid_NormalState) {
    Graph graph;
    auto interA = std::make_shared<Intersection>("INT_A", 0, 0, IntersectionType::CROSS);
    auto interB = std::make_shared<Intersection>("INT_B", 10, 10, IntersectionType::CROSS);
    graph.addIntersection(interA);
    graph.addIntersection(interB);

    auto road1 = std::make_shared<Road>("ROAD_1", "INT_A", "INT_B", 100.0, 50.0);
    graph.addRoad(road1);

    // Mọi thứ hoàn hảo, đồ thị phải trả về true
    EXPECT_TRUE(graph.isValid());
}