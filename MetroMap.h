#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "MetroStation.h"
#include "Graph.h"

// MetroMap is the domain layer — knows about Delhi Metro specifically.
// It owns the graph and station metadata, and coordinates loading and display.
class MetroMap {
private:
    std::unordered_map<std::string, MetroStation> stations;   // keyed by full station name
    std::unordered_map<std::string, std::string>  codeToName; // code -> full name lookup
    Graph graph;

public:
    MetroMap();

    // Building the map
    void addStation(const MetroStation& station);
    void addConnection(const std::string& code1, const std::string& code2, int distKm);

    // Lookups
    bool stationExists(const std::string& name) const;
    bool stationExistsByCode(const std::string& code) const;
    std::string getNameByCode(const std::string& code) const;

    const MetroStation& getStation(const std::string& name) const;
    const std::unordered_map<std::string, MetroStation>& getAllStations() const;
    const Graph& getGraph() const;

    // Stats for the startup banner
    int getStationCount() const;
    int getInterchangeCount() const;
    std::vector<std::string> getAllLines() const;

    // Case-insensitive partial-match search by name or code
    std::vector<std::string> searchStation(const std::string& query) const;

    // Display functions
    void displayAllStations() const;
    void displayNetwork() const;
    void displayStationInfo(const std::string& name) const;
    void displayStationsByLine(const std::string& lineName) const;
};
