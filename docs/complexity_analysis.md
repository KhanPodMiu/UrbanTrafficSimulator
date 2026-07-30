1
# Algorithm Complexity Analysis

### A. Overall Complexity Comparison

| Algorithm | Average Case | Worst Case | Space Complexity | Complete | Optimal |
|-----------|--------------|------------|------------------|----------|---------|
| BFS | **O(V + E)** | **O(V + E)** | **O(V)** | Yes | Only for unweighted graphs |
| Dijkstra | **O(E log V)** | **O(E log V)** | **O(V)** | Yes | Yes |
| A* | **O(E log V)** | **O(E log V)** | **O(V)** | Yes | Yes (admissible heuristic) |

where V is Intersection and E is Road.

---

### B. Time Complexity Analysis

#### 1) Breadth-First Search (BFS)

The BFS implementation stores frontier vertices in a FIFO queue. Every vertex is visited at most once, while every edge is examined exactly once.

| Operation | Complexity |
|-----------|------------|
| Push into queue | O(1) |
| Pop from queue | O(1) |
| Visit each vertex | O(V) |
| Traverse adjacency list | O(E) |
| **Total** | **O(V + E)** |

Because the implementation marks visited vertices immediately after discovery, duplicate exploration is avoided.

---

#### 2) Dijkstra's Algorithm

The implementation uses a binary heap (`std::priority_queue`) together with lazy deletion. Each relaxation inserts a new state into the priority queue instead of decreasing keys.

| Operation | Complexity |
|-----------|------------|
| Push into priority queue | O(log V) |
| Pop minimum | O(log V) |
| Edge relaxation | O(log V) |
| Relax all edges | O(E log V) |
| Vertex processing | O(V log V) |
| **Total** | **O(E log V)** |

---

#### 3) A* Search Algorithm

The A* implementation is structurally similar to Dijkstra but augments the priority with

> f(n) = g(n) + h(n)

where `h(n)` is the heuristic estimate from the current node to the destination.

| Operation | Complexity |
|-----------|------------|
| Heuristic evaluation | O(1) |
| Push into priority queue | O(log V) |
| Pop minimum | O(log V) |
| Edge relaxation | O(log V) |
| **Worst-case Total** | **O(E log V)** |

When the heuristic is informative (e.g., Euclidean distance), substantially fewer vertices are expanded in practice than with Dijkstra, resulting in lower execution time while maintaining the same asymptotic worst-case complexity.

---

### C. Space Complexity

| Algorithm | Data Structures | Space |
|-----------|----------------|-------|
| BFS | Queue + Visited Set + Parent Map | O(V) |
| Dijkstra | Priority Queue + Distance Map + Parent Map | O(V) |
| A* | Priority Queue + gScore + fScore + Parent Map | O(V) |

Each algorithm stores at most one record per vertex (excluding temporary duplicate heap entries), giving linear auxiliary memory.

---

### D. Compare algorithm performance
#### **MAP: DinhDocLap**
RouteRequest("I1", "I414")
| Heuristic | Execution Time (µs) | Expanded Nodes | Path Length | Total Travel Cost |
|-----------|--------------------:|---------------:|------------:|------------:|
| BFS | 938 | 339 | 22 | - |
| DIJKSTRA | 1305 | 319 | 30 | 4506 |
| A* Euclidean | 782 | 57 | 35 | 5454 |
| A* Weighted Euclidean | 585 | 57 | 35 | 5454 |
| A* Distance / MAX_Speed | 1286 | 232 | 30 | 4506 |

#### **MAP: LangDaiHoc**
RouteRequest("I1", "I300")
| Heuristic | Execution Time (µs) | Expanded Nodes | Path Length | Total Travel Cost |
|-----------|--------------------:|---------------:|------------:|------------:|
| BFS | 509 | 80 | 10 | - |
| DIJKSTRA | 560 | 97 | 10 | 2150 |
| A* Euclidean | 244 | 24 | 11 | 2406 |
| A* Weighted Euclidean | 244 | 24 | 11 | 2406 |
| A* Distance / MAX_Speed | 429 | 83 | 10 | 2150 |

#### **MAP: large_map_v2**
RouteRequest("I1", "I464")
| Heuristic | Execution Time (µs) | Expanded Nodes | Path Length | Total Travel Cost |
|-----------|--------------------:|---------------:|------------:|------------:|
| BFS | 569 | 205 | 16 | - |
| DIJKSTRA | 1426 | 266 | 20 | 2994 |
| A* Euclidean | 390 | 21 | 18 | 3025 |
| A* Weighted Euclidean | 227 | 21 | 18 | 3025 |
| A* Distance / MAX_Speed | 1002 | 206 | 20 | 2994 |

---

### E. Conclusion

The experimental results show that BFS is the most efficient algorithm for unweighted graphs but cannot guarantee the minimum travel cost because edge weights are ignored. Dijkstra consistently produces the optimal route with respect to travel cost, while A* significantly reduces the number of expanded vertices and execution time by incorporating heuristic guidance. Among the evaluated heuristics, the **Distance / Maximum Speed** heuristic provides the best balance between search efficiency and solution quality, achieving the same optimal travel cost as Dijkstra on most test cases while exploring fewer vertices.