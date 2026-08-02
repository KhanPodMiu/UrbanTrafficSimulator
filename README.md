# 🚦 Urban Traffic Simulator

> ⚠️ **Important**
>
> The project assets are **not included** in this repository due to GitHub file size limitations.
>
> Please download the assets before building and running the project:
>
> **https://drive.google.com/drive/folders/1CQbJjuW34emZOPZTr5rSRfgFvRLjUHFo?usp=sharing**
>
> Extract the downloaded **assets/** folder into the project root.

---

> [!NOTE]
> Before building the project, ensure that SDL2, SDL2_image, SDL2_ttf, and JsonCpp are installed and can be found by CMake. On Windows, using **vcpkg** is recommended.

---

# 📖 Overview

Urban Traffic Simulator is an Object-Oriented Programming project developed in **C++** using **SDL2**.

The simulator models an urban transportation system with graph-based road networks, multiple routing algorithms, intelligent traffic management, and real-time visualization.

---

# ⚙️ Requirements

- C++20 compatible compiler
- CMake 3.16+
- SDL2

## macOS

```bash
brew install cmake
brew install sdl2
```

## Windows

- Visual Studio 2022 (Desktop development with C++)
- CMake
- SDL2 Development Library


---

# 🔨 Build

```bash
cmake -B build
cmake --build build
```

For Visual Studio:

```bash
cmake -B build
cmake --build build --config Release
```

---

# ▶️ Run

### macOS

```bash
./build/main
```

### Windows

```bash
./build/main.exe
```

---

# 🎮 Controls

## Camera

| Action | Control |
|--------|---------|
| Move Camera | **W / A / S / D** |
| Move Camera | **Mouse Drag** |
| Zoom In | **Mouse Wheel Up** or **E** |
| Zoom Out | **Mouse Wheel Down** or **Q** |

## Simulation

| Action | Control |
|--------|---------|
| Spawn Vehicle |  Point to an intersection and click V button|
| Toggle Heat Map | H |

> 💡 The application also provides additional runtime controls through the **left control panel**, including routing algorithms, simulation speed, traffic events, and other interactive features.

---

# ✨ Features

- 🚗 Multiple vehicle types
- 🗺️ Graph-based road network loaded from JSON
- 🧠 Multiple routing algorithms
  - BFS
  - Dijkstra
  - A*
- 🚦 Traffic light system
- 🚑 Emergency vehicle priority
- 🚧 Dynamic congestion
- 📊 Traffic heat map
- 🎯 Click-to-spawn vehicles
- 📷 Interactive camera (keyboard + mouse)
- ⚡ Adjustable simulation speed



---

# 🎨 Design Patterns

## Strategy Pattern
Runtime switching between BFS, Dijkstra, and A*.

## Factory Pattern
Vehicle creation.

## Singleton Pattern
TrafficLightManager.

---

# 📂 Project Structure

```text
UrbanTrafficSimulator
├── assets/
├── include/
├── src/
│   ├── algorithms/
│   ├── core/
│   ├── factory/
│   ├── graph/
│   ├── simulation/
│   ├── utils/
│   ├── vehicles/
│   └── visualization/
├── CMakeLists.txt
└── README.md
```



---

# 📈 Visualization

- Window Resolution: **1600 × 900**
- Control Panel: **400 × 900**
- Simulation Viewport: **1200 × 900**
- World Size: **53000 × 40000**

---

# 🔗 UML Class Diagram

The UML diagram can be viewed here:

**https://lucid.app/lucidchart/621ff6fe-6c69-467e-9886-c0219d5e9a98/edit?invitationId=inv_f5f53f59-8181-4c40-bcf0-7c06040d4c17&page=0_0#**


---

# 📝 Notes

- Download the assets before running the simulator.
- Build the project using CMake.
- Launch the executable from the **build/** directory.
- Most simulation features are accessible from the application's graphical interface.
