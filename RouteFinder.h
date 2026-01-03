#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Graph.h"
#include "MetroStation.h"

// Contains everything needed to display a journey summary
struct JourneyInfo {
    RouteResult route;
    int fare;
    int interchangeCount;
    std::vector<int> interchangeAt;  // indices in path where line changes happen
};

// RouteFinder wraps Dijkstra and adds metro-specific logic:
// interchange detection, fare calculation, and journey assembly.
class RouteFinder {
private:
    const Graph& graph;
    const std::unordered_map<std::string, MetroStation>& stations;

    // Returns the common metro line between two adjacent stations.
    // This is how we know which line is being used for each edge.
    std::string findCommonLine(const std::string& s1, const std::string& s2) const;

    JourneyInfo buildJourneyInfo(RouteResult route) const;

public:
    RouteFinder(const Graph& graph,
                const std::unordered_map<std::string, MetroStation>& stations);

    JourneyInfo findShortestDistance(const std::string& src, const std::string& dst) const;
    JourneyInfo findFastestRoute(const std::string& src, const std::string& dst) const;

    // Public so main.cpp can use it for display
    int detectInterchanges(const std::vector<std::string>& path,
                           std::vector<int>& interchangeIndices) const;
};
