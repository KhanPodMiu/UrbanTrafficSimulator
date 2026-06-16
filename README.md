# Urban Traffic Simulator 🚗


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

## UML Class Diagram

<p align="center">
  <img src="docs/diagrams/uml_class_diagram.svg">
</p>