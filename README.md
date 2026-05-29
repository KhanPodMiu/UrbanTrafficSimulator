# Urban Traffic Simulator 🚗

An object-oriented traffic simulation system built with C++ and SDL2.

The project simulates:

* road networks,
* vehicle movement,
* traffic congestion,
* pathfinding algorithms,
* and dynamic traffic events
  inside a virtual urban environment.


---

# 🧠 Architecture Overview

The project is divided into several independent modules:

| Module           | Responsibility                                       |
| ---------------- | ---------------------------------------------------- |
| `algorithms/`    | Pathfinding algorithms such as BFS, Dijkstra, and A* |
| `graph/`         | Road network representation using graph structures   |
| `simulation/`    | Traffic simulation engine and event system           |
| `vehicle/`       | Vehicle entities and movement logic                  |
| `visualization/` | SDL2 rendering system and UI                         |
| `core/`          | Main application loop, timing, and engine systems    |
| `utils/`         | Utility helpers and file loading                     |

---

# 🎮 Technologies

* C++17
* SDL2
* SDL2_image
* SDL2_ttf
* Makefile build system
* GoogleTest (unit testing)

---

# ⚙️ Build & Run

## macOS

Install dependencies:

```bash
brew install sdl2 sdl2_image sdl2_ttf
```

Build project:

```bash
make
```

Run:

```bash
make run
```

Clean build:

```bash
make clean
```

---

# 📌 Notes

* All rendering-related systems are isolated inside the `visualization/` module.
* SDL2 should only be used inside the visualization layer.
* Simulation and pathfinding systems are independent from rendering systems.
* Header files are stored inside `include/`.
* Source files are stored inside `src/`.

---

# 🚀 Future Features

* Dynamic congestion system
* Traffic light simulation
* Real-time route recalculation
* Zoomable map camera
* Statistics dashboard
* Heatmap visualization
* Multi-vehicle simulation
* Event-driven traffic system

---


# 📁 Project Structure

```text
UrbanTrafficSimulator/
│
├── assets/
│   ├── fonts/
│   ├── textures/
│   ├── icons/
│   └── maps/
│
├── bin/
│
├── config/
│   ├── settings.json
│   └── simulation.json
│
├── docs/
│
├── include/
│
│   ├── algorithms/
│   │   ├── PathFindingStrategy.hpp
│   │   ├── Dijkstra.hpp
│   │   ├── AStar.hpp
│   │   └── BFS.hpp
│   │
│   ├── core/
│   │   ├── Application.hpp
│   │   ├── Engine.hpp
│   │   ├── Time.hpp
│   │   └── Logger.hpp
│   │
│   ├── graph/
│   │   ├── Graph.hpp
│   │   ├── Node.hpp
│   │   ├── Edge.hpp
│   │   ├── Road.hpp
│   │   └── Intersection.hpp
│   │
│   ├── simulation/
│   │   ├── TrafficSimulator.hpp
│   │   ├── TrafficController.hpp
│   │   ├── TrafficEvent.hpp
│   │   └── CongestionManager.hpp
│   │
│   ├── vehicle/
│   │   ├── Vehicle.hpp
│   │   ├── Car.hpp
│   │   ├── Bus.hpp
│   │   └── EmergencyVehicle.hpp
│   │
│   ├── visualization/
│   │   ├── Window.hpp
│   │   ├── Renderer.hpp
│   │   ├── Camera.hpp
│   │   ├── TextureManager.hpp
│   │   ├── FontManager.hpp
│   │   ├── Color.hpp
│   │   │
│   │   ├── renderers/
│   │   │   ├── GraphRenderer.hpp
│   │   │   ├── VehicleRenderer.hpp
│   │   │   ├── RoadRenderer.hpp
│   │   │   ├── TrafficRenderer.hpp
│   │   │   └── UIOverlayRenderer.hpp
│   │   │
│   │   └── ui/
│   │       ├── Button.hpp
│   │       ├── Panel.hpp
│   │       ├── Label.hpp
│   │       └── StatisticsPanel.hpp
│   │
│   └── utils/
│       ├── FileLoader.hpp
│       ├── JsonLoader.hpp
│       └── MathUtils.hpp
│
├── src/
│
│   ├── algorithms/
│   ├── core/
│   ├── graph/
│   ├── simulation/
│   ├── vehicle/
│   ├── visualization/
│   │
│   └── main.cpp
│
├── tests/
│   ├── graph/
│   ├── algorithms/
│   ├── simulation/
│   └── vehicle/
│
├── .vscode/
│
├── Makefile
│
├── README.md
│
└── .gitignore
```

---

# Class attributes

```
class Intersection {
private:
    int id;
    float x;
    float y;
};

class Road {
private:
    int id;
    int sourceId;
    int destinationId;
    double distance;
    double speedLimit;
};

{
  "intersections": [
    {
      "id": 1,
      "x": 100,
      "y": 200
    }
  ],
  "roads": [
    {
      "id": 1,
      "source": 1,
      "destination": 2,
      "distance": 50,
      "speedLimit": 60
    }
  ]
}
```