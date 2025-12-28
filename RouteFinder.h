#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Graph.h"
#include "MetroStation.h"

struct JourneyInfo {
    RouteResult route;
    int fare;
    int interchangeCount;
    std::vector<int> interchangeAt;
};

class RouteFinder {
private:
    const Graph& graph;
    const std::unordered_map<std::string, MetroStation>& stations;
    std::string findCommonLine(const std::string& s1, const std::string& s2) const;
    JourneyInfo buildJourneyInfo(RouteResult route) const;
public:
    RouteFinder(const Graph& graph,
                const std::unordered_map<std::string, MetroStation>& stations);
    JourneyInfo findShortestDistance(const std::string& src, const std::string& dst) const;
};
