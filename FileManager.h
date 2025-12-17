#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "MetroStation.h"

// Handles all file I/O for loading metro data.
// Separating this means the rest of the code doesn't care where data comes from.
class FileManager {
public:
    // Loads stations from a pipe-delimited text file.
    // Returns empty vector if the file cannot be opened.
    std::vector<MetroStation> loadStations(const std::string& filename);

    // Loads connections as (code1, code2, distanceKm) tuples.
    std::vector<std::tuple<std::string, std::string, int>> loadConnections(const std::string& filename);

private:
    std::vector<std::string> splitBy(const std::string& str, char delim);
};
