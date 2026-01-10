# Delhi Metro Navigation System

A modular, **OOP-driven C++ console application** that models the Delhi Metro network as a weighted graph and computes optimal transit paths using **Dijkstra's Algorithm** with two routing modes — shortest distance and minimum travel time.

---

## Features

- **Shortest Distance Routing** — standard Dijkstra over a weighted adjacency list minimising total kilometres.
- **Minimum Time Routing** — state-space Dijkstra where the graph state is `(station, current_line)`, enabling accurate 5-minute interchange wait penalties whenever a line change occurs.
- **BFS Connectivity Check** — fast `O(V + E)` reachability check before running Dijkstra, preventing unnecessary computation.
- **O(1) Station Lookups** — all station data and code-to-name mappings are stored in `std::unordered_map` (hash maps), giving constant-time access.
- **DMRC Fare Calculation** — distance-based fare slabs matching the 2024 Delhi Metro Rail Corporation fare chart (Rs. 10 to Rs. 60).
- **Interchange Detection** — identifies every line change along the computed route and annotates the journey summary with interchange markers.
- **Fuzzy Station Search** — supports exact name, station code (e.g. `YMC`), and case-insensitive partial-name matching with an interactive disambiguation menu.
- **Data-Driven Design** — all station and connection data is loaded from plain-text files (`stations.txt`, `connections.txt`), making the network easy to extend without recompiling.

---

## Project Structure

```
MetroNavigation/
│
├── main.cpp              # Entry point — menu loop, journey display, station resolution
│
├── MetroMap.h / .cpp     # Domain layer — owns the Graph and station metadata,
│                         # coordinates loading, search, and display
│
├── Graph.h / .cpp        # Core data structure — weighted undirected adjacency list,
│                         # Dijkstra (distance & time modes), BFS connectivity check
│
├── RouteFinder.h / .cpp  # Routing layer — state-space Dijkstra for minimum time,
│                         # interchange detection, JourneyInfo assembly
│
├── MetroStation.h / .cpp # Station entity — name, station code, lines, interchange flag
│
├── FareCalculator.h / .cpp  # DMRC fare slab logic (static utility class)
│
├── FileManager.h / .cpp  # File I/O — loads stations.txt and connections.txt,
│                         # parses pipe-delimited records into domain objects
│
├── stations.txt          # Station data: name | code | lines | interchange flag
└── connections.txt       # Edge data: code1 | code2 | distance (km)
```

---

## Data Structures & Algorithms

| Component | Implementation |
|---|---|
| Metro Network | Weighted undirected graph via `std::unordered_map<string, vector<pair<string,int>>>` (adjacency list) |
| Station Lookup | `std::unordered_map<string, MetroStation>` — **O(1)** average lookup |
| Code-to-Name | `std::unordered_map<string, string>` — **O(1)** average lookup |
| Shortest Distance | Dijkstra with `std::priority_queue` (min-heap) — **O((V + E) log V)** |
| Minimum Time | State-space Dijkstra on `(station, line)` pairs with 5-min interchange penalty |
| Connectivity Check | BFS using `std::queue` — **O(V + E)** |
| Fare Calculation | Distance-based slab lookup — **O(1)** |

> **Why adjacency list over adjacency matrix?**  
> The Delhi Metro network is sparse — each station connects to at most 3–4 neighbours. An adjacency matrix would waste `O(N²)` space (≈ N² entries for a few hundred stations) with no performance benefit. The adjacency list uses only `O(V + E)` space.

> **Why state-space Dijkstra for time routing?**  
> Standard Dijkstra keys only on the current station, so it cannot model the fact that arriving at an interchange via different lines carries a different future cost. By expanding the state to `(station, current_line)` using a composite key `"station|line"`, the algorithm correctly applies a 300-second (5-minute) penalty only when the line actually changes, producing routes that genuinely differ from shortest-distance mode.

---

## Fare Chart (DMRC 2024)

| Distance | Fare |
|---|---|
| 0 – 2 km | Rs. 10 |
| 2 – 5 km | Rs. 20 |
| 5 – 12 km | Rs. 30 |
| 12 – 21 km | Rs. 40 |
| 21 – 32 km | Rs. 50 |
| 32+ km | Rs. 60 |

---

## Build & Run

**Requirements:** A C++17-compatible compiler (GCC 7+, Clang 5+, MSVC 2017+).

```bash
# Compile
g++ -std=c++17 -O2 -o metro \
    main.cpp Graph.cpp MetroMap.cpp MetroStation.cpp \
    RouteFinder.cpp FareCalculator.cpp FileManager.cpp

# Run (stations.txt and connections.txt must be in the working directory)
./metro
```

On Windows, the pre-compiled `metro.exe` can be run directly from the project folder.

---

## Menu Options

```
1. Display All Stations       — lists every station with its code and lines
2. Display Metro Network      — prints the full adjacency list
3. Search Station             — name/code/partial-match search
4. Station Information        — detailed info for a single station
5. Find Route                 — Dijkstra routing (shortest distance or minimum time)
6. Show Stations by Line      — filter stations by metro line
7. Exit
```

---

## Example Journey Output

```
==================================================
                JOURNEY SUMMARY
==================================================

  Source              : Rajiv Chowk
  Destination         : Huda City Centre
  Route Type          : Minimum Time

  Optimal Route

  Rajiv Chowk                    [Yellow Line, Blue Line]
     |
     v
  ...

  ================ INTERCHANGE ================
     Blue Line
          |
          v  Change Here
     Yellow Line
  =============================================

  ...

--------------------------------------------------

  Distance            : 32 km
  Estimated Time      : 48 minutes
  Fare                : Rs. 60
  Interchanges        : 1

==================================================
```

---

## Class Diagram (Overview)

```
main.cpp
  └── MetroMap           (domain layer)
        ├── Graph        (adjacency list + Dijkstra + BFS)
        ├── MetroStation (entity)
        └── FileManager  (I/O)
  └── RouteFinder        (state-space Dijkstra, interchange detection)
        └── FareCalculator (fare slab logic)
```

---

## Author

**Saurav Raj**  
[GitHub](https://github.com/sauravraj2003)
