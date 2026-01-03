#include "RouteFinder.h"
#include "FareCalculator.h"
#include <queue>
#include <climits>
#include <cmath>
#include <algorithm>

using namespace std;

RouteFinder::RouteFinder(const Graph& g,
                         const unordered_map<string, MetroStation>& s)
    : graph(g), stations(s) {}

string RouteFinder::findCommonLine(const string& s1name, const string& s2name) const {
    if (!stations.count(s1name) || !stations.count(s2name)) return "";

    const auto& lines1 = stations.at(s1name).getLines();
    const auto& lines2 = stations.at(s2name).getLines();

    for (const string& a : lines1) {
        for (const string& b : lines2) {
            if (a == b) return a;
        }
    }
    return "";
}

int RouteFinder::detectInterchanges(const vector<string>& path,
                                    vector<int>& interchangeIndices) const {
    interchangeIndices.clear();
    if (path.size() < 2) return 0;

    string currentLine = findCommonLine(path[0], path[1]);
    int count = 0;

    for (int i = 1; i + 1 < (int)path.size(); i++) {
        string nextLine = findCommonLine(path[i], path[i + 1]);
        if (!nextLine.empty() && nextLine != currentLine) {
            interchangeIndices.push_back(i);
            currentLine = nextLine;
            count++;
        }
    }
    return count;
}

JourneyInfo RouteFinder::buildJourneyInfo(RouteResult route) const {
    JourneyInfo info;
    info.route = route;
    info.fare = 0;
    info.interchangeCount = 0;

    if (route.valid) {
        info.fare = FareCalculator::calculateFare(route.distanceKm);
        info.interchangeCount = detectInterchanges(route.path, info.interchangeAt);
    }
    return info;
}

JourneyInfo RouteFinder::findShortestDistance(const string& src, const string& dst) const {
    // Pure distance Dijkstra — minimises total km only
    return buildJourneyInfo(graph.dijkstra(src, dst, false));
}

// ---------------------------------------------------------------------------
// findFastestRoute: state-space Dijkstra where state = (station, current_line)
//
// Standard Dijkstra keys on station only, so it cannot model the fact that
// arriving at an interchange on different lines carries a different future cost.
// By including the current line in the state, we can add a realistic
// interchange waiting penalty (5 minutes) whenever the line changes.
//
// This creates genuine differences from shortest-distance mode:
// a route with 0 interchanges can beat a shorter route that has 2 interchanges,
// because it avoids two 5-minute platform waits.
//
// Cost per edge  : 120 sec (stop time) + 40 sec/km (travel time)
// Interchange    : +300 sec (5 min wait for next train on new line)
// ---------------------------------------------------------------------------
JourneyInfo RouteFinder::findFastestRoute(const string& src, const string& dst) const {
    const int STOP_TIME        = 120;   // seconds at each station stop
    const int SEC_PER_KM       = 40;    // train travel speed
    const int INTERCHANGE_WAIT = 300;   // 5-minute platform wait when changing lines

    if (!stations.count(src) || !stations.count(dst)) {
        JourneyInfo empty; empty.route.valid = false; return empty;
    }

    // State key = "stationName|lineName" so we can track the current line
    // without creating a separate struct
    auto makeKey = [](const string& station, const string& line) -> string {
        return station + "|" + line;
    };

    unordered_map<string, int>    dist;   // key -> best cost in seconds
    unordered_map<string, string> prev;   // key -> previous key (path reconstruction)

    // priority queue: (cost, key)
    priority_queue<pair<int,string>, vector<pair<int,string>>, greater<pair<int,string>>> pq;

    // Seed the source for each line it belongs to (cost 0, no initial penalty)
    for (const string& startLine : stations.at(src).getLines()) {
        string key = makeKey(src, startLine);
        dist[key] = 0;
        prev[key] = "";
        pq.push({0, key});
    }

    string reachedKey = "";

    while (!pq.empty()) {
        auto [cost, currKey] = pq.top();
        pq.pop();

        if (dist.count(currKey) && cost > dist.at(currKey)) continue;

        // Decode station and line from the key
        size_t sep = currKey.rfind('|');
        string currStation = currKey.substr(0, sep);
        string currLine    = currKey.substr(sep + 1);

        if (currStation == dst) {
            reachedKey = currKey;
            break;
        }

        // Iterate over all edges from this station
        for (const auto& [nextStation, edgeDist] : graph.getAdjList().at(currStation)) {
            if (!stations.count(nextStation)) continue;

            string edgeLine = findCommonLine(currStation, nextStation);
            if (edgeLine.empty()) continue;

            // Pay the interchange penalty only when switching to a different line
            int interchangeCost = (edgeLine != currLine) ? INTERCHANGE_WAIT : 0;
            int newCost = cost + STOP_TIME + SEC_PER_KM * edgeDist + interchangeCost;

            string nextKey = makeKey(nextStation, edgeLine);
            if (!dist.count(nextKey) || newCost < dist.at(nextKey)) {
                dist[nextKey] = newCost;
                prev[nextKey] = currKey;
                pq.push({newCost, nextKey});
            }
        }
    }

    // Could not reach destination — fall back to plain time Dijkstra
    if (reachedKey.empty()) {
        return buildJourneyInfo(graph.dijkstra(src, dst, true));
    }

    // Reconstruct the station-only path by walking back through prev[]
    vector<string> path;
    for (string k = reachedKey; !k.empty(); ) {
        size_t sep = k.rfind('|');
        path.push_back(k.substr(0, sep));
        if (!prev.count(k) || prev.at(k).empty()) break;
        k = prev.at(k);
    }
    reverse(path.begin(), path.end());

    // Recompute distance and actual time (including interchange waits) from the path
    RouteResult route;
    route.valid = true;
    route.path  = path;

    int totalDist    = 0;
    int totalTimeSec = 0;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int w = graph.getEdgeWeight(path[i], path[i + 1]);
        totalDist    += w;
        totalTimeSec += STOP_TIME + SEC_PER_KM * w;
    }
    // Count interchanges in the path and add their waiting time
    vector<int> icAt;
    int icCount = detectInterchanges(path, icAt);
    totalTimeSec += icCount * INTERCHANGE_WAIT;

    route.distanceKm  = totalDist;
    route.timeMinutes = (int)ceil((double)totalTimeSec / 60.0);

    return buildJourneyInfo(route);
}
