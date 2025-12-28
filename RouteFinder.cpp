#include "RouteFinder.h"
#include "FareCalculator.h"
#include <algorithm>
using namespace std;

RouteFinder::RouteFinder(const Graph& g,
                         const unordered_map<string, MetroStation>& s)
    : graph(g), stations(s) {}

string RouteFinder::findCommonLine(const string& s1name, const string& s2name) const {
    if (!stations.count(s1name) || !stations.count(s2name)) return "";
    const auto& lines1 = stations.at(s1name).getLines();
    const auto& lines2 = stations.at(s2name).getLines();
    for (const string& a : lines1)
        for (const string& b : lines2)
            if (a == b) return a;
    return "";
}

JourneyInfo RouteFinder::buildJourneyInfo(RouteResult route) const {
    JourneyInfo info;
    info.route = route;
    info.fare = 0;
    info.interchangeCount = 0;
    if (route.valid)
        info.fare = FareCalculator::calculateFare(route.distanceKm);
    return info;
}

JourneyInfo RouteFinder::findShortestDistance(const string& src, const string& dst) const {
    return buildJourneyInfo(graph.dijkstra(src, dst, false));
}
