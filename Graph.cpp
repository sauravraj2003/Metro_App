#include "Graph.h"
#include <queue>
#include <climits>
#include <cmath>
#include <algorithm>

using namespace std;

void Graph::addVertex(const string& name) {
    if (adjList.find(name) == adjList.end()) {
        adjList[name] = {};
    }
}

void Graph::addEdge(const string& v1, const string& v2, int distance) {
    if (!containsVertex(v1) || !containsVertex(v2))
        return;

    adjList[v1].push_back({v2, distance});
    adjList[v2].push_back({v1, distance});
}

bool Graph::containsVertex(const string& name) const {
    return adjList.count(name) > 0;
}

// BFS is enough to simply check whether a path exists.
bool Graph::hasPath(const string& src, const string& dst) const {
    if (!containsVertex(src) || !containsVertex(dst))
        return false;

    unordered_map<string, bool> visited;
    queue<string> q;

    q.push(src);
    visited[src] = true;

    while (!q.empty()) {
        string curr = q.front();
        q.pop();

        if (curr == dst)
            return true;

        for (const auto& [next, dist] : adjList.at(curr)) {
            if (!visited[next]) {
                visited[next] = true;
                q.push(next);
            }
        }
    }

    return false;
}

vector<string> Graph::getNeighbors(const string& name) const {
    vector<string> result;

    if (!containsVertex(name))
        return result;

    for (const auto& [next, dist] : adjList.at(name)) {
        result.push_back(next);
    }

    return result;
}

int Graph::getEdgeWeight(const string& v1, const string& v2) const {
    if (!containsVertex(v1))
        return -1;

    for (const auto& [next, dist] : adjList.at(v1)) {
        if (next == v2)
            return dist;
    }

    return -1;
}

int Graph::getVertexCount() const {
    return (int)adjList.size();
}

const unordered_map<string, vector<pair<string, int>>>& Graph::getAdjList() const {
    return adjList;
}

// Dijkstra finds the shortest route using either distance or travel time.
RouteResult Graph::dijkstra(const string& src, const string& dst, bool useTime) const {

    RouteResult result;
    result.valid = false;
    result.distanceKm = 0;
    result.timeMinutes = 0;

    if (!containsVertex(src) || !containsVertex(dst))
        return result;

    unordered_map<string, int> dist;
    unordered_map<string, string> prev;

    for (const auto& [name, neighbors] : adjList) {
        dist[name] = INT_MAX;
        prev[name] = "";
    }

    dist[src] = 0;

    // priority_queue always processes the station with minimum cost first.
    priority_queue<pair<int, string>,
                   vector<pair<int, string>>,
                   greater<pair<int, string>>> pq;

    pq.push({0, src});

    while (!pq.empty()) {

        auto [cost, curr] = pq.top();
        pq.pop();

        if (cost > dist[curr])
            continue;

        if (curr == dst)
            break;

        for (const auto& [next, edgeDist] : adjList.at(curr)) {

            int newCost;

            if (useTime)
                newCost = cost + 120 + 40 * edgeDist;
            else
                newCost = cost + edgeDist;

            if (newCost < dist[next]) {
                dist[next] = newCost;
                prev[next] = curr;
                pq.push({newCost, next});
            }
        }
    }

    if (dist[dst] == INT_MAX)
        return result;

    // Reconstruct the path using the previous station map.
    vector<string> path;

    for (string cur = dst; !cur.empty(); cur = prev[cur]) {
        path.push_back(cur);
    }

    reverse(path.begin(), path.end());

    result.valid = true;
    result.path = path;

    int totalDist = 0;
    int totalTimeSec = 0;

    for (int i = 0; i + 1 < path.size(); i++) {
        int w = getEdgeWeight(path[i], path[i + 1]);

        totalDist += w;
        totalTimeSec += 120 + 40 * w;
    }

    result.distanceKm = totalDist;
    result.timeMinutes = (int)ceil((double)totalTimeSec / 60.0);

    return result;
}