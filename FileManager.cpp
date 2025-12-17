#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

vector<string> FileManager::splitBy(const string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delim)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

vector<MetroStation> FileManager::loadStations(const string& filename) {
    vector<MetroStation> stations;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "\n  [Error] Cannot open station file: " << filename << "\n";
        cerr << "  Make sure stations.txt is in the same folder as the executable.\n\n";
        return stations;
    }

    string line;
    while (getline(file, line)) {
        // Skip blank lines and comment lines starting with '#'
        if (line.empty() || line[0] == '#') continue;

        vector<string> parts = splitBy(line, '|');
        if (parts.size() < 4) continue;

        string code  = parts[0];
        string name  = parts[1];
        vector<string> lines = splitBy(parts[2], ',');
        bool isInterchange = (parts[3] == "1");

        stations.emplace_back(name, code, lines, isInterchange);
    }

    file.close();
    return stations;
}

vector<tuple<string, string, int>> FileManager::loadConnections(const string& filename) {
    vector<tuple<string, string, int>> connections;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "\n  [Error] Cannot open connection file: " << filename << "\n";
        cerr << "  Make sure connections.txt is in the same folder as the executable.\n\n";
        return connections;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        vector<string> parts = splitBy(line, '|');
        if (parts.size() < 3) continue;

        string code1 = parts[0];
        string code2 = parts[1];
        int dist = stoi(parts[2]);

        connections.emplace_back(code1, code2, dist);
    }

    file.close();
    return connections;
}
