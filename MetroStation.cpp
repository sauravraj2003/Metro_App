#include "MetroStation.h"

using namespace std;

// Default constructor
MetroStation::MetroStation() {
    interchange = false;
}

// Constructor to initialize station details
MetroStation::MetroStation(const string& n, const string& c,
                           const vector<string>& l, bool ic) {
    name = n;
    code = c;
    lines = l;
    interchange = ic;
}

// Returns station name
string MetroStation::getName() const {
    return name;
}

// Returns station code
string MetroStation::getCode() const {
    return code;
}

// Returns all metro lines of the station
const vector<string>& MetroStation::getLines() const {
    return lines;
}

// Checks if the station is an interchange
bool MetroStation::isInterchange() const {
    return interchange;
}