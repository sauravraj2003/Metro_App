#include "Graph.h"
#include <algorithm>
using namespace std;

void Graph::addVertex(const string& name) {
    if (adjList.find(name) == adjList.end()) adjList[name] = {};
}

void Graph::addEdge(const string& v1, const string& v2, int distance) {
    if (!containsVertex(v1) || !containsVertex(v2)) return;
    adjList[v1].push_back({v2, distance});
    adjList[v2].push_back({v1, distance});
}

bool Graph::containsVertex(const string& name) const {
    return adjList.count(name) > 0;
}

vector<string> Graph::getNeighbors(const string& name) const {
    vector<string> result;
    if (!containsVertex(name)) return result;
    for (const auto& [next, dist] : adjList.at(name)) result.push_back(next);
    return result;
}

int Graph::getEdgeWeight(const string& v1, const string& v2) const {
    if (!containsVertex(v1)) return -1;
    for (const auto& [next, dist] : adjList.at(v1))
        if (next == v2) return dist;
    return -1;
}

int Graph::getVertexCount() const { return (int)adjList.size(); }

const unordered_map<string, vector<pair<string, int>>>& Graph::getAdjList() const {
    return adjList;
}
