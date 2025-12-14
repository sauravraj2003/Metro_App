#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// Stores the final route returned by Dijkstra's algorithm.
struct RouteResult {
    vector<string> path;    // ordered list of station names 
    int distanceKm;
    int timeMinutes;
    bool valid;
};

// Adjacency list chosen over matrix because the metro network is sparse.
// Each station connects to at most 3-4 others, so a matrix wastes O(N^2) space.
class Graph {
private:
    unordered_map<string, vector<pair<string, int>>> adjList;

public:
    void addVertex(const string& name);
    void addEdge(const string& v1, const string& v2, int distance);

    bool containsVertex(const string& name) const;
    bool hasPath(const string& src, const string& dst) const;

    vector<string> getNeighbors(const string& name) const;
    int getEdgeWeight(const string& v1, const string& v2) const;
    int getVertexCount() const;
    const unordered_map<string, vector<pair<string, int>>>& getAdjList() const;

    // useTime = false  -> minimize distance (km)
    // useTime = true   -> minimize time (120s stop + 40s/km travel per edge)
    // priority_queue gives O((V+E) log V), much better than naive O(V^2)
    // Dijkstra is used because all distances are positive
    // and it efficiently finds the shortest route.
    RouteResult dijkstra(const string& src, const string& dst, bool useTime) const;
};
