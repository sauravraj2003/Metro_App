#include "MetroMap.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

MetroMap::MetroMap() {}

void MetroMap::addStation(const MetroStation& station) {
    stations[station.getName()] = station;
    codeToName[station.getCode()] = station.getName();
    graph.addVertex(station.getName());
}

void MetroMap::addConnection(const string& code1, const string& code2, int distKm) {
    if (!stationExistsByCode(code1) || !stationExistsByCode(code2)) return;
    string name1 = getNameByCode(code1);
    string name2 = getNameByCode(code2);
    graph.addEdge(name1, name2, distKm);
}

bool MetroMap::stationExists(const string& name) const {
    return stations.count(name) > 0;
}

bool MetroMap::stationExistsByCode(const string& code) const {
    return codeToName.count(code) > 0;
}

string MetroMap::getNameByCode(const string& code) const {
    auto it = codeToName.find(code);
    if (it != codeToName.end()) return it->second;
    return "";
}

const MetroStation& MetroMap::getStation(const string& name) const {
    return stations.at(name);
}

const unordered_map<string, MetroStation>& MetroMap::getAllStations() const {
    return stations;
}

const Graph& MetroMap::getGraph() const {
    return graph;
}

int MetroMap::getStationCount() const {
    return (int)stations.size();
}

int MetroMap::getInterchangeCount() const {
    int count = 0;
    for (const auto& [name, station] : stations) {
        if (station.isInterchange()) count++;
    }
    return count;
}

vector<string> MetroMap::getAllLines() const {
    set<string> lineSet;
    for (const auto& [name, station] : stations) {
        for (const string& line : station.getLines()) {
            lineSet.insert(line);
        }
    }
    return vector<string>(lineSet.begin(), lineSet.end());
}

vector<string> MetroMap::searchStation(const string& query) const {
    string lq = query;
    transform(lq.begin(), lq.end(), lq.begin(), ::tolower);

    vector<string> results;
    for (const auto& [name, station] : stations) {
        string ln = name;
        transform(ln.begin(), ln.end(), ln.begin(), ::tolower);

        string lc = station.getCode();
        transform(lc.begin(), lc.end(), lc.begin(), ::tolower);

        if (ln.find(lq) != string::npos || lc.find(lq) != string::npos) {
            results.push_back(name);
        }
    }
    // Sort alphabetically so output is consistent
    sort(results.begin(), results.end());
    return results;
}

void MetroMap::displayAllStations() const {
    cout << "\n==================================================================\n";
    cout << "                      ALL METRO STATIONS\n";
    cout << "==================================================================\n\n";
    cout << "  " << left << setw(4) << "No."
         << setw(28) << "Station Name"
         << setw(8)  << "Code"
         << "Line(s)\n";
    cout << "  ------------------------------------------------------------------\n";

    // Collect and sort by name for a clean list
    vector<pair<string, MetroStation>> sorted(stations.begin(), stations.end());
    sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    int i = 1;
    for (const auto& [name, station] : sorted) {
        string displayName = name + (station.isInterchange() ? " *" : "");
        cout << "  " << left << setw(4) << i
             << setw(28) << displayName
             << setw(8)  << station.getCode();

        const auto& lines = station.getLines();
        for (int j = 0; j < (int)lines.size(); j++) {
            if (j > 0) cout << ", ";
            cout << lines[j];
        }
        cout << "\n";
        i++;
    }

    cout << "\n  ------------------------------------------------------------------\n";
    cout << "  Total: " << stations.size() << " stations   |   * = Interchange Station\n";
    cout << "==================================================================\n\n";
}

void MetroMap::displayNetwork() const {
    cout << "\n==================================================================\n";
    cout << "                    DELHI METRO NETWORK\n";
    cout << "==================================================================\n";

    vector<string> lines = getAllLines();
    const auto& adjList = graph.getAdjList();

    for (const string& line : lines) {
        // Collect all stations on this line
        vector<string> lineStations;
        for (const auto& [name, station] : stations) {
            for (const string& l : station.getLines()) {
                if (l == line) { lineStations.push_back(name); break; }
            }
        }
        sort(lineStations.begin(), lineStations.end());

        cout << "\n  " << line << "  (" << lineStations.size() << " stations)\n";
        cout << "  " << string(line.size() + 16, '-') << "\n";

        // Show connections between stations on this line
        set<pair<string,string>> printed;
        for (const string& sname : lineStations) {
            if (adjList.count(sname) == 0) continue;
            for (const auto& [neighbor, dist] : adjList.at(sname)) {
                // Only show connection if neighbor is also on this line
                if (!stations.count(neighbor)) continue;
                bool neighborOnLine = false;
                for (const string& nl : stations.at(neighbor).getLines()) {
                    if (nl == line) { neighborOnLine = true; break; }
                }
                if (!neighborOnLine) continue;

                string a = sname, b = neighbor;
                if (a > b) swap(a, b);
                if (printed.count({a, b})) continue;
                printed.insert({a, b});

                cout << "  " << left << setw(26) << sname
                     << "  -->  "
                     << left << setw(26) << neighbor
                     << "  (" << dist << " km)\n";
            }
        }
    }
    cout << "\n==================================================================\n\n";
}

void MetroMap::displayStationInfo(const string& name) const {
    if (!stationExists(name)) {
        cout << "\n  Station \"" << name << "\" not found.\n\n";
        return;
    }
    const MetroStation& station = stations.at(name);

    cout << "\n==================================================\n";
    cout << "              Station Information\n";
    cout << "==================================================\n\n";
    cout << "  " << left << setw(18) << "Name"  << ": " << station.getName() << "\n";
    cout << "  " << left << setw(18) << "Code"  << ": " << station.getCode() << "\n";
    cout << "  " << left << setw(18) << "Lines" << ": ";

    const auto& lines = station.getLines();
    for (int i = 0; i < (int)lines.size(); i++) {
        if (i > 0) cout << ", ";
        cout << lines[i];
    }
    cout << "\n";
    cout << "  " << left << setw(18) << "Type"
         << ": " << (station.isInterchange() ? "Interchange" : "Regular") << "\n";

    // Show connected stations and the line connecting them
    cout << "\n  Connected Stations\n\n";
    vector<string> neighbors = graph.getNeighbors(name);
    sort(neighbors.begin(), neighbors.end());

    for (const string& nbr : neighbors) {
        int dist = graph.getEdgeWeight(name, nbr);
        // Find common line
        string commonLine = "";
        if (stations.count(nbr)) {
            for (const string& l1 : station.getLines()) {
                for (const string& l2 : stations.at(nbr).getLines()) {
                    if (l1 == l2) { commonLine = l1; break; }
                }
                if (!commonLine.empty()) break;
            }
        }
        cout << "  * " << left << setw(28) << nbr
             << "  (" << dist << " km)";
        if (!commonLine.empty()) cout << "  [" << commonLine << "]";
        cout << "\n";
    }
    cout << "\n==================================================\n\n";
}

void MetroMap::displayStationsByLine(const string& lineName) const {
    vector<pair<string, string>> lineStations;  // (name, code)
    for (const auto& [name, station] : stations) {
        for (const string& l : station.getLines()) {
            if (l == lineName) {
                lineStations.push_back({name, station.getCode()});
                break;
            }
        }
    }

    if (lineStations.empty()) {
        cout << "\n  No stations found for line: " << lineName << "\n\n";
        return;
    }
    sort(lineStations.begin(), lineStations.end());

    cout << "\n==================================================\n";
    cout << "  " << lineName << "\n";
    cout << "==================================================\n\n";
    cout << "  " << left << setw(4) << "No."
         << setw(28) << "Station Name"
         << "Code\n";
    cout << "  --------------------------------------------------\n";

    int i = 1;
    for (const auto& [name, code] : lineStations) {
        bool ic = stations.at(name).isInterchange();
        string displayName = name + (ic ? " *" : "");
        cout << "  " << left << setw(4) << i
             << setw(28) << displayName
             << code << "\n";
        i++;
    }
    cout << "\n  Total: " << lineStations.size() << " stations   |   * = Interchange\n";
    cout << "==================================================\n\n";
}
