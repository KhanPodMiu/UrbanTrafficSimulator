# Urban Traffic Simulator 🚗

# Island Tourist Transportation Simulator

## Project Overview

The Island Tourist Transportation Simulator is a graph-based urban traffic simulation system developed using Object-Oriented Programming principles.

The simulator models a tourist island as a road network consisting of intersections and roads loaded dynamically from JSON map files. Traffic conditions continuously evolve through random vehicle generation, congestion, accidents, and road events.

The main goal of the simulation is to assist a tourist bus that starts from the island's Bus Terminal and must visit all major attractions and districts across the island while minimizing travel time and transportation cost.

To achieve this objective, the routing system must continuously analyze current traffic conditions and dynamically select the most efficient route. When congestion increases, accidents occur, or roads become unavailable, the system automatically recalculates the route using pathfinding algorithms.

---

## Scenario

A group of tourists arrives on the island and begins their journey from the Bus Terminal.

Their objective is to explore the entire island by visiting all major attractions and districts, including beaches, parks, museums, markets, downtown areas, and other points of interest.

Meanwhile, the island's traffic network is constantly changing:

* Vehicles are generated dynamically.
* Traffic congestion may increase or decrease.
* Accidents may occur unexpectedly.
* Roads may become temporarily blocked.

The tourist bus must adapt to these changing conditions and find the most efficient route to complete the tour.

---

## Features

### Road Network System

* Graph-based road network
* Intersections and roads as OOP entities
* Dynamic road condition updates
* JSON map loading
* Support for custom island maps

### Traffic Simulation

* Random vehicle generation
* Dynamic congestion system
* Accident events
* Road closure events
* Real-time traffic updates

### Routing System

* Breadth-First Search (BFS)
* Dijkstra Algorithm
* A* Search Algorithm
* Congestion-aware routing
* Dynamic route recalculation

### Visualization

* SDL2-based rendering
* Road and intersection visualization
* Vehicle movement animation
* Congestion indicators
* Simulation statistics panel

### Statistics

* Total travel time
* Total travel cost
* Number of route recalculations
* Congestion metrics
* Roads visited
* Simulation performance metrics

---

## Simulation Objective

The tourist bus must:

1. Start from the Bus Terminal.
2. Visit all major attractions and districts on the island.
3. Respond to changing traffic conditions.
4. Avoid heavily congested or blocked roads when possible.
5. Complete the entire tour with the lowest travel time and cost.

---

## Technologies

* C++
* SDL2
* JSON
* Graph Data Structures
* Object-Oriented Programming
* Design Patterns
* Git & GitHub

---

## Educational Objectives

This project demonstrates:

* Graph modeling of transportation networks
* Dynamic pathfinding algorithms
* Traffic simulation systems
* Event-driven architecture
* Object-Oriented Software Design
* Real-time visualization techniques


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
    int roadId;

    int sourceIntersectionId;
    int destinationIntersectionId;

    double distance;
    double speedLimit;

    double congestionLevel;
    double travelCost;
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

# Map and Window Design

## Window Layout

The application window is divided into two main sections:

* **Map Viewport** (left side)
* **Control & Statistics Panel** (right side)

Recommended window size:

```text
1600 x 900
```

Layout:

```text
+---------------------------------------------------+------------------+
|                                                   |                  |
|                                                   |                  |
|                                                   |                  |
|                   MAP VIEWPORT                    |  CONTROL PANEL  |
|                                                   |                  |
|                                                   |                  |
|                                                   |                  |
+---------------------------------------------------+------------------+
```

Dimensions:

```text
Map Viewport : 1200 x 900
Control Panel:  400 x 900
```

---

## World Map Size

The world map should be significantly larger than the visible viewport to support camera movement and zooming.

Recommended world size:

```text
4000 x 4000
```

The entire island exists inside this world space.

Example:

```text
World Size    : 4000 x 4000
Viewport Size : 1200 x 900
```

Only a portion of the world is visible at any given time.

---

## Coordinate System

Each intersection stores its position inside the world.

Example:

```cpp
Intersection
{
    id: 1,
    name: "Beach",
    x: 2500,
    y: 1200
}
```

SDL2 uses these coordinates to render intersection sprites and roads.

The graph structure is used for pathfinding, while the coordinates are used for visualization.

---

## Camera System

A camera controls which part of the world is currently visible.

Example:

```cpp
Camera
{
    x;
    y;
    zoom;
}
```

Features:

* Zoom in / Zoom out
* Pan across the island
* Follow tourist bus (optional)

This allows large maps to be explored while keeping the application window size fixed.

---

## Control Panel

The right-side panel displays simulation controls and statistics.

Suggested controls:

* Start Simulation
* Pause Simulation
* Reset Simulation
* Simulation Speed

Suggested traffic settings:

* Congestion Level
* Accident Probability
* Vehicle Spawn Rate

Suggested statistics:

* Current Algorithm
* Travel Time
* Travel Cost
* Attractions Visited
* Number of Route Recalculations
* Current Traffic Status
