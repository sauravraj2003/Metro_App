#pragma once

#include <string>
#include <vector>

using namespace std;

class MetroStation {
private:
    // Basic station information
    string name;
    string code;

    // Stores all metro lines passing through this station
    vector<string> lines;

    // True if the station belongs to multiple lines
    bool interchange;

public:
    // Default constructor
    MetroStation();

    // Constructor to initialize all station details
    MetroStation(const string& name,
                 const string& code,
                 const vector<string>& lines,
                 bool interchange);

    // Getter functions
    string getName() const;
    string getCode() const;
    const vector<string>& getLines() const;
    bool isInterchange() const;
};