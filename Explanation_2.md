# Giải Thích Chi Tiết 4 File Mới — Dữ Liệu Đến Từ Đâu & Chạy Như Thế Nào

**Ngày:** 09/07/2026  
**Phạm vi:** `PathFindingStrategy.hpp`, `RoutingManager.hpp/.cpp`, `RouteOptimizer.hpp/.cpp`

---

## 0. Bản đồ dữ liệu tổng quan — Mọi thứ bắt đầu từ đâu?

Trước khi đi vào 4 file mới, bạn cần hiểu rõ dữ liệu gốc đến từ đâu.

### Nguồn gốc: File JSON bản đồ

Tất cả dữ liệu bản đồ nằm trong thư mục `assets/maps/`. Ví dụ file `small_map.json`:

```json
{
    "intersections": [
        { "id": "I1", "x": 1000, "y": 1000 },
        { "id": "I2", "x": 3000, "y": 1000 },
        { "id": "I3", "x": 3000, "y": 3000 },
        { "id": "I4", "x": 1000, "y": 3000 }
    ],
    "roads": [
        { "id": "R1", "source": "I1", "destination": "I2", "distance": 2000, "speedLimit": 60 },
        { "id": "R2", "source": "I2", "destination": "I3", "distance": 2000, "speedLimit": 60 },
        ...
    ]
}
```

### Chuỗi nạp dữ liệu

```
JSON file  ──►  MapLoader::loadFromJson()  ──►  Graph
                                                  ├── Intersections (map<string, Intersection>)
                                                  ├── Roads         (map<string, Road>)
                                                  └── adjacencyList (map<string, vector<Road>>)
```

- **`MapLoader::loadFromJson(filename, graph)`** đọc file JSON, tạo các đối tượng `Intersection` (ngã tư) và `Road` (đoạn đường), rồi nhét vào đối tượng `Graph`.
- **`Graph`** lưu trữ 3 bảng băm (hash map):
  - `Intersections`: tra cứu ngã tư theo ID (ví dụ `"I1"` → tọa độ `(1000, 1000)`)
  - `Roads`: tra cứu đoạn đường theo ID (ví dụ `"R1"` → nối `I1` → `I2`, dài `2000`, tốc độ `60km/h`)
  - `adjacencyList`: tra cứu "từ ngã tư X, có thể đi tới những đoạn đường nào?" (ví dụ `"I1"` → `[R1, R8]`)

### Các thuộc tính quan trọng của Road

Mỗi `Road` (đoạn đường) chứa những thông tin mà 4 file mới sẽ sử dụng:

| Thuộc tính | Kiểu | Ý nghĩa | Lấy bằng hàm |
|---|---|---|---|
| `distance` | `int` | Chiều dài đoạn đường (đơn vị bản đồ) | `road->getDistance()` |
| `speedLimit` | `int` | Giới hạn tốc độ (km/h), từ 5 đến 130 | `road->getSpeedLimit()` |
| `congestionLevel` | `int` | Mức tắc nghẽn (0 = thông thoáng, 100 = kẹt cứng) | `road->getCongestionLevel()` |
| `travelCost` | `int` | Chi phí đi qua = tổng hợp từ distance, speed, congestion | `road->getTravelCost()` |

> **Lưu ý quan trọng:** `travelCost` được tính tự động mỗi khi `distance`, `speedLimit`, hoặc `congestionLevel` thay đổi. Đây chính là "trọng số cạnh" mà các thuật toán tìm đường sử dụng.

---

## 1. File `PathFindingStrategy.hpp` — Interface thuần ảo

### Vị trí: `include/algorithms/PathFindingStrategy.hpp`

```cpp
#pragma once

#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class PathFindingStrategy {
public:
    virtual ~PathFindingStrategy() = default;
    virtual RouteResult calculateRoute(const Graph& graph, 
                                       const RouteRequest& request) const = 0;
};
```

### Giải thích từng dòng

**`#pragma once`**  
Đảm bảo file chỉ được include 1 lần, tránh lỗi khai báo trùng.

**`class PathFindingStrategy`**  
Đây là **lớp trừu tượng** (abstract class) — nó không có implementation, chỉ định nghĩa "hợp đồng" (contract) mà mọi thuật toán phải tuân theo.

**`virtual ~PathFindingStrategy() = default;`**  
Virtual destructor — bắt buộc khi dùng đa hình (polymorphism). Nếu thiếu dòng này, khi ta xóa đối tượng `BFS` qua con trỏ `PathFindingStrategy*`, destructor của `BFS` sẽ KHÔNG được gọi → rò rỉ bộ nhớ.

**`virtual RouteResult calculateRoute(...) const = 0;`**  
- `= 0` nghĩa là **pure virtual** — bất kỳ lớp con nào cũng PHẢI override hàm này, nếu không sẽ lỗi biên dịch.
- `const` nghĩa là hàm này cam kết không thay đổi trạng thái logic của đối tượng.
- Hàm nhận vào `Graph` (bản đồ) và `RouteRequest` (điểm đi & đến), trả về `RouteResult` (đường đi tìm được).

### Dữ liệu vào — ra

```
        Đầu vào                          Đầu ra
┌──────────────────┐             ┌──────────────────────────┐
│  const Graph&    │             │  RouteResult             │
│  (toàn bộ bản đồ)│    ──►     │  ├── intersectionIDs:    │
│                  │             │  │   ["I1","I2","I3"]     │
│  RouteRequest    │             │  ├── isSuccess: true      │
│  ├── start: "I1" │             │  └── totalCost: 150.0     │
│  └── dest:  "I3" │             └──────────────────────────┘
└──────────────────┘
```

### Ai kế thừa nó?

```
PathFindingStrategy (interface)
    ├── BFS           ← Tìm đường ngắn nhất theo số bước (không xét trọng số)
    ├── Dijkstra      ← Tìm đường ngắn nhất theo tổng trọng số (travelCost)
    └── AStarStrategy ← Dijkstra + heuristic (ước lượng khoảng cách đến đích)
```

---

## 2. File `RoutingManager.hpp` + `RoutingManager.cpp` — Bộ quản lý chiến lược

### Vị trí header: `include/algorithms/RoutingManager.hpp`

```cpp
#pragma once
#include <memory>
#include "algorithms/PathFindingStrategy.hpp"
#include "graph/Graph.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"

class RoutingManager {
private:
    std::unique_ptr<PathFindingStrategy> strategy;     // (1)
public:
    RoutingManager(std::unique_ptr<PathFindingStrategy> 
                   initialStrategy = nullptr);          // (2)
    void setStrategy(std::unique_ptr<PathFindingStrategy> 
                     newStrategy);                      // (3)
    RouteResult calculateRoute(const Graph& graph, 
                               const RouteRequest& request) const;  // (4)
};
```

### Giải thích từng dòng

**(1) `std::unique_ptr<PathFindingStrategy> strategy;`**

Đây là con trỏ thông minh giữ 1 đối tượng thuật toán (BFS, Dijkstra, hoặc A*). `unique_ptr` đảm bảo:
- Chỉ có DUY NHẤT 1 chủ sở hữu (RoutingManager).
- Tự động giải phóng bộ nhớ khi RoutingManager bị hủy.
- Không thể copy (tránh lỗi double-free), chỉ có thể `move`.

**(2) Constructor: `RoutingManager(... initialStrategy = nullptr)`**

Cho phép tạo RoutingManager với hoặc không có thuật toán ban đầu:
```cpp
// Cách 1: Tạo rỗng, gắn thuật toán sau
RoutingManager manager;

// Cách 2: Tạo với thuật toán A* ngay từ đầu
RoutingManager manager(std::make_unique<AStarStrategy>());
```

**(3) `setStrategy(...)` — Chuyển đổi thuật toán lúc runtime**

Đây là trái tim của Strategy Pattern. Bạn có thể đổi thuật toán bất kỳ lúc nào:
```cpp
manager.setStrategy(std::make_unique<BFS>());       // Đổi sang BFS
manager.setStrategy(std::make_unique<Dijkstra>());  // Đổi sang Dijkstra
```
Nó dùng `std::move` để chuyển quyền sở hữu con trỏ. Thuật toán cũ tự động bị giải phóng.

**(4) `calculateRoute(...)` — Ủy thác cho thuật toán hiện tại**

Hàm này KHÔNG tự tìm đường. Nó chỉ gọi `strategy->calculateRoute(...)` — tức là thuật toán nào đang được gắn thì thuật toán đó chạy.

### Vị trí source: `src/algorithms/RoutingManager.cpp`

```cpp
#include "algorithms/RoutingManager.hpp"

RoutingManager::RoutingManager(std::unique_ptr<PathFindingStrategy> initialStrategy)
    : strategy(std::move(initialStrategy)) {}   // (A)

void RoutingManager::setStrategy(std::unique_ptr<PathFindingStrategy> newStrategy) {
    strategy = std::move(newStrategy);            // (B)
}

RouteResult RoutingManager::calculateRoute(const Graph& graph, 
                                           const RouteRequest& request) const {
    if (!strategy) {                              // (C)
        return RouteResult();
    }
    return strategy->calculateRoute(graph, request);  // (D)
}
```

**(A) `std::move(initialStrategy)`**  
`unique_ptr` không thể copy, chỉ có thể move. `std::move()` chuyển quyền sở hữu từ tham số vào biến thành viên.

**(B) `strategy = std::move(newStrategy)`**  
Thuật toán cũ (nếu có) tự động bị xóa, thuật toán mới được gắn vào.

**(C) `if (!strategy)` — Guard clause**  
Nếu chưa gắn thuật toán nào, trả về `RouteResult` rỗng (isSuccess = false). Không crash.

**(D) `strategy->calculateRoute(graph, request)` — Đa hình**  
Đây là đa hình (polymorphism) trong thực tế:
- Nếu `strategy` đang trỏ tới `BFS` → gọi `BFS::calculateRoute()`
- Nếu `strategy` đang trỏ tới `Dijkstra` → gọi `Dijkstra::calculateRoute()`
- Nếu `strategy` đang trỏ tới `AStarStrategy` → gọi `AStarStrategy::calculateRoute()`

Tất cả đều xảy ra tại **runtime**, không cần `if-else` hay `switch-case`.

### Luồng chạy minh họa

```
Bước 1: Người dùng tạo RoutingManager với A*
┌─────────────────────────────────────────────────────┐
│ auto manager = RoutingManager(                      │
│     std::make_unique<AStarStrategy>()               │
│ );                                                  │
│                                                     │
│ manager.strategy ──► [AStarStrategy object]          │
└─────────────────────────────────────────────────────┘

Bước 2: Tìm đường từ I1 đến I100
┌─────────────────────────────────────────────────────┐
│ RouteRequest req("I1", "I100");                     │
│ RouteResult result = manager.calculateRoute(        │
│     graph, req                                      │
│ );                                                  │
│                                                     │
│ Bên trong: strategy->calculateRoute(graph, req)     │
│        ==> AStarStrategy::calculateRoute() chạy     │
│        ==> Trả về: ["I1", "I5", "I23", ..., "I100"]│
└─────────────────────────────────────────────────────┘

Bước 3: Đổi sang Dijkstra lúc runtime
┌─────────────────────────────────────────────────────┐
│ manager.setStrategy(                                │
│     std::make_unique<Dijkstra>()                    │
│ );                                                  │
│                                                     │
│ [AStarStrategy object] bị XÓA tự động              │
│ manager.strategy ──► [Dijkstra object]               │
│                                                     │
│ // Giờ calculateRoute() sẽ dùng Dijkstra            │
└─────────────────────────────────────────────────────┘
```

---

## 3. File `RouteOptimizer.hpp` + `RouteOptimizer.cpp` — Bộ tối ưu tuyến đường

### Vị trí header: `include/simulation/RouteOptimizer.hpp`

```cpp
#pragma once
#include <string>
#include "graph/Graph.hpp"
#include "algorithms/RoutingManager.hpp"
#include "simulation/RouteResult.hpp"

class RouteOptimizer {
private:
    RoutingManager* routingManager;    // (1)
    int congestionThreshold;           // (2)
public:
    RouteOptimizer(RoutingManager* manager, int threshold = 70);  // (3)
    
    bool isRouteDegraded(const Graph& graph, 
                         const RouteResult& currentRoute, 
                         const std::string& currentLocation) const;  // (4)

    RouteResult optimizeRoute(const Graph& graph, 
                              const RouteResult& currentRoute, 
                              const std::string& currentLocation, 
                              const std::string& destination);       // (5)
};
```

### Giải thích từng trường và hàm

**(1) `RoutingManager* routingManager`**  
Con trỏ thường (raw pointer) trỏ tới `RoutingManager` đang hoạt động bên ngoài. `RouteOptimizer` KHÔNG sở hữu `RoutingManager` — nó chỉ "mượn" để gọi khi cần tính lại đường.

**(2) `int congestionThreshold`**  
Ngưỡng tắc nghẽn. Nếu bất kỳ đoạn đường nào trên lộ trình hiện tại có `congestionLevel > threshold`, lộ trình sẽ bị coi là "xuống cấp". Mặc định là `70` (tức 70/100).

**(3) Constructor**  
```cpp
RouteOptimizer optimizer(&manager, 70);
// hoặc
RouteOptimizer optimizer(&manager);  // threshold mặc định = 70
```

### Vị trí source: `src/simulation/RouteOptimizer.cpp`

#### Hàm `isRouteDegraded()` — Phát hiện tắc nghẽn phía trước

```cpp
bool RouteOptimizer::isRouteDegraded(
    const Graph& graph,                  // Bản đồ hiện tại (dữ liệu tắc nghẽn real-time)
    const RouteResult& currentRoute,     // Lộ trình xe đang đi
    const std::string& currentLocation   // Xe đang ở ngã tư nào
) const {

    // [Guard] Nếu lộ trình không hợp lệ hoặc rỗng → không cần kiểm tra
    if (!currentRoute.isSuccess || currentRoute.intersectionIDs.empty()) 
        return false;
    
    bool locationFound = false;

    // Duyệt từng cặp ngã tư liên tiếp trong lộ trình
    for (size_t i = 0; i < currentRoute.intersectionIDs.size() - 1; ++i) {
        
        // Tìm vị trí xe hiện tại trong lộ trình
        if (currentRoute.intersectionIDs[i] == currentLocation) {
            locationFound = true;
        }

        // CHỈ kiểm tra các đoạn đường PHÍA TRƯỚC xe
        if (locationFound) {
            const std::string& from = currentRoute.intersectionIDs[i];      // Ngã tư đầu
            const std::string& to   = currentRoute.intersectionIDs[i + 1];  // Ngã tư cuối
            
            // Hỏi Graph: "Từ ngã tư 'from', có những đường nào?"
            const auto& connectedRoads = graph.getConnectedRoads(from);

            for (const auto& road : connectedRoads) {
                if (!road) continue;  // Bỏ qua road null

                const Intersection* dest = road->getDestinationIntersection();

                // Tìm đúng đoạn đường nối from → to
                if (dest && dest->getIntersectionID() == to) {

                    // Kiểm tra mức tắc nghẽn
                    if (road->getCongestionLevel() > congestionThreshold) {
                        return true;   // ⚠️ PHÁT HIỆN TẮC NGHẼN!
                    }
                    break;  // Đã tìm thấy đoạn đường cần kiểm tra, không cần duyệt tiếp
                }
            }
        }
    }
    return false;  // ✅ Không có tắc nghẽn nghiêm trọng
}
```

#### Minh họa `isRouteDegraded()` bằng ví dụ cụ thể

Giả sử xe đang có lộ trình: `["I1", "I5", "I23", "I67", "I100"]`  
Xe hiện tại đang ở `"I23"`.

```
Lộ trình:   I1 ──► I5 ──► I23 ──► I67 ──► I100
                           ▲ (xe ở đây)
                           │
             Đã đi qua ◄──┘──► Chưa đi (cần kiểm tra)
```

Hàm sẽ:
1. Bỏ qua đoạn `I1 → I5` (đã đi qua)
2. Bỏ qua đoạn `I5 → I23` (đã đi qua)  
3. Kiểm tra đoạn `I23 → I67`: Hỏi Graph lấy road nối `I23` → `I67`, xem `congestionLevel`
4. Kiểm tra đoạn `I67 → I100`: tương tự

Nếu bất kỳ đoạn nào có `congestionLevel > 70` → trả về `true`.

#### Hàm `optimizeRoute()` — Tính lại đường nếu cần

```cpp
RouteResult RouteOptimizer::optimizeRoute(
    const Graph& graph,
    const RouteResult& currentRoute,
    const std::string& currentLocation,  // Xe đang ở đâu
    const std::string& destination        // Xe muốn đến đâu
) {
    // [Guard] Nếu không có RoutingManager → không thể tính lại → giữ nguyên
    if (!routingManager) {
        return currentRoute;
    }

    // Kiểm tra xem lộ trình có bị tắc nghẽn không
    if (isRouteDegraded(graph, currentRoute, currentLocation)) {

        // Tạo yêu cầu tìm đường MỚI: từ vị trí hiện tại → đích
        RouteRequest request(currentLocation, destination);

        // Nhờ RoutingManager tính đường mới (dùng thuật toán đang được gắn)
        RouteResult newRoute = routingManager->calculateRoute(graph, request);

        // Chỉ chấp nhận đường mới nếu nó hợp lệ
        if (newRoute.isSuccess) {
            return newRoute;
        }
    }

    // Không có tắc nghẽn, hoặc không tìm được đường mới → giữ nguyên
    return currentRoute;
}
```

#### Minh họa `optimizeRoute()` bằng ví dụ cụ thể

```
TRƯỚC khi tối ưu:
  Lộ trình cũ: I1 ──► I5 ──► I23 ──► I67 ──► I100
                                       ▲
                               congestionLevel = 85  (> 70, TẮC!)

  isRouteDegraded() trả về TRUE ✓

SAU khi tối ưu:
  RoutingManager tính lại từ I23 → I100 (bỏ qua đoạn đã đi)
  Lộ trình mới: I23 ──► I45 ──► I78 ──► I100  (đi đường vòng tránh tắc)
```

---

## 4. Luồng chạy toàn bộ hệ thống — End-to-End

Dưới đây là luồng chạy từ đầu đến cuối khi hệ thống mô phỏng hoạt động:

```
┌─────────────────────────────────────────────────────────────┐
│  BƯỚC 1: NẠP BẢN ĐỒ                                       │
│                                                             │
│  Graph graph;                                               │
│  MapLoader::loadFromJson("DinhDocLap.json", graph);         │
│                                                             │
│  → Graph chứa 414 ngã tư + hàng trăm đoạn đường            │
│  → Mỗi Road có distance, speedLimit, congestionLevel        │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  BƯỚC 2: TẠO ROUTING MANAGER VỚI THUẬT TOÁN                │
│                                                             │
│  RoutingManager manager(                                    │
│      std::make_unique<AStarStrategy>()                      │
│  );                                                         │
│                                                             │
│  → manager sở hữu 1 đối tượng AStarStrategy                │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  BƯỚC 3: TẠO ROUTE OPTIMIZER                                │
│                                                             │
│  RouteOptimizer optimizer(&manager, 70);                    │
│                                                             │
│  → optimizer giữ con trỏ tới manager                        │
│  → ngưỡng tắc nghẽn = 70                                   │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  BƯỚC 4: TÌM ĐƯỜNG LẦN ĐẦU                                │
│                                                             │
│  RouteRequest req("I1", "I100");                            │
│  RouteResult route = manager.calculateRoute(graph, req);    │
│                                                             │
│  → AStarStrategy chạy trên Graph:                           │
│    1. Lấy Intersection "I1" và "I100" từ graph              │
│    2. Dùng priority queue duyệt các ngã tư lân cận         │
│    3. Với mỗi Road, lấy getTravelCost() làm trọng số       │
│    4. Kết hợp heuristic (khoảng cách Euclidean ÷ vận tốc)  │
│    5. Trả về: route = ["I1", "I5", "I23", "I67", "I100"]   │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  BƯỚC 5: SIMULATION LOOP — MÔ PHỎNG XE CHẠY               │
│                                                             │
│  while (simulationRunning) {                                │
│      // Cập nhật trạng thái giao thông                      │
│      road_R23_I67->updateCongestion(85);  // đường bị tắc! │
│                                                             │
│      // Xe hiện tại đang ở I23                              │
│      std::string carLocation = "I23";                       │
│                                                             │
│      // Hỏi optimizer: có nên đổi đường không?              │
│      route = optimizer.optimizeRoute(                       │
│          graph, route, carLocation, "I100"                  │
│      );                                                     │
│                                                             │
│      // Bên trong optimizeRoute():                          │
│      //   1. isRouteDegraded() phát hiện I23→I67 tắc (85>70)│
│      //   2. Tạo RouteRequest("I23", "I100")                │
│      //   3. manager.calculateRoute() → A* tính đường mới   │
│      //   4. Đường mới: ["I23", "I45", "I78", "I100"]       │
│      //   5. Trả về đường mới cho xe                        │
│  }                                                          │
└─────────────────────────────────────────────────────────────┘
```

---

## 5. Bảng tổng hợp — File nào lấy dữ liệu từ đâu?

| File | Lấy dữ liệu từ | Để làm gì |
|---|---|---|
| `PathFindingStrategy` | Nhận `Graph` + `RouteRequest` | Định nghĩa "hợp đồng" chung cho mọi thuật toán |
| `RoutingManager` | Nhận `PathFindingStrategy` (qua `unique_ptr`) | Giữ + hoán đổi thuật toán, ủy thác tìm đường |
| `RouteOptimizer` | Nhận `RoutingManager*` (con trỏ mượn) | Giám sát tắc nghẽn + yêu cầu tính lại đường |
| ↳ `isRouteDegraded()` | `Graph.getConnectedRoads()` → `Road.getCongestionLevel()` | Kiểm tra từng đoạn đường phía trước xe |
| ↳ `optimizeRoute()` | `RoutingManager.calculateRoute()` | Tính đường mới nếu phát hiện tắc |

### Chuỗi phụ thuộc (dependency chain)

```
JSON file
  └──► MapLoader
        └──► Graph (Intersections + Roads + adjacencyList)
               │
               ├──► PathFindingStrategy::calculateRoute(graph, request)
               │      ├── BFS         (duyệt theo hàng đợi)
               │      ├── Dijkstra    (duyệt theo chi phí thấp nhất)
               │      └── AStarStrategy (Dijkstra + heuristic)
               │
               ├──► RoutingManager (giữ 1 strategy, gọi calculateRoute)
               │
               └──► RouteOptimizer (giám sát congestion, gọi RoutingManager)
```

---

## 6. Tóm tắt vai trò từng file

| File | Vai trò trong hệ thống | Tương tự ngoài đời |
|---|---|---|
| `PathFindingStrategy.hpp` | Bản "hợp đồng" — mọi thuật toán phải ký | Luật chơi mà mọi người chơi phải theo |
| `RoutingManager.hpp/.cpp` | "Quản lý" — giữ 1 thuật toán, cho phép đổi | Người lái xe chọn app bản đồ (Google Maps / Waze) |
| `RouteOptimizer.hpp/.cpp` | "Cảm biến thông minh" — phát hiện tắc đường, tự đổi lộ trình | Tính năng "reroute" tự động trong Google Maps |

Nếu bạn còn thắc mắc về bất kỳ phần nào, cứ hỏi mình nhé!
