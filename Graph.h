#pragma once
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class Graph {
private:
    unordered_map<string, vector<pair<string, int>>> adjList;
public:
    void addVertex(const string& name);
    void addEdge(const string& v1, const string& v2, int distance);
    bool containsVertex(const string& name) const;
    bool hasPath(const string& src, const string& dst) const;    // BFS
    vector<string> getNeighbors(const string& name) const;
    int getEdgeWeight(const string& v1, const string& v2) const;
    int getVertexCount() const;
    const unordered_map<string, vector<pair<string, int>>>& getAdjList() const;
};
