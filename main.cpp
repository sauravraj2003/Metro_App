#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "MetroStation.h"
#include "Graph.h"
#include "FareCalculator.h"
#include "FileManager.h"
#include "RouteFinder.h"
#include "MetroMap.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// -----------------------------------------------------------------------
// Helper: find the common metro line between two adjacent stations.
// Used for route display to annotate each leg and detect interchanges.
// -----------------------------------------------------------------------
string findCommonLine(const string& s1, const string& s2,
                      const unordered_map<string, MetroStation>& stations) {
    if (!stations.count(s1) || !stations.count(s2)) return "";
    const auto& l1 = stations.at(s1).getLines();
    const auto& l2 = stations.at(s2).getLines();
    for (const string& a : l1)
        for (const string& b : l2)
            if (a == b) return a;
    return "";
}

// -----------------------------------------------------------------------
// Displays the Journey Summary after route calculation
// -----------------------------------------------------------------------
void displayJourney(const JourneyInfo& info, const string& routeType,
                    const MetroMap& metro) {
    const vector<string>& path = info.route.path;
    const auto& stations = metro.getAllStations();

    cout << "\n==================================================\n";
    cout << "                JOURNEY SUMMARY\n";
    cout << "==================================================\n\n";

    cout << "  " << left << setw(20) << "Source"
         << ": " << path.front() << "\n";
    cout << "  " << left << setw(20) << "Destination"
         << ": " << path.back()  << "\n";
    cout << "  " << left << setw(20) << "Route Type"
         << ": " << routeType    << "\n";

    cout << "\n  Optimal Route\n\n";

    // Track the current line to detect where interchanges happen
    string currentLine = "";
    if (path.size() >= 2) {
        currentLine = findCommonLine(path[0], path[1], stations);
    }

    for (int i = 0; i < (int)path.size(); i++) {
        // Print station name and its lines
        const string& sname = path[i];
        cout << "  " << left << setw(30) << sname;
        if (stations.count(sname)) {
            const auto& lines = stations.at(sname).getLines();
            cout << "  [";
            for (int j = 0; j < (int)lines.size(); j++) {
                if (j > 0) cout << ", ";
                cout << lines[j];
            }
            cout << "]";
        }
        cout << "\n";

        if (i + 1 < (int)path.size()) {
            string nextLine = findCommonLine(path[i], path[i + 1], stations);

            if (!currentLine.empty() && !nextLine.empty() && nextLine != currentLine) {
                // Line change detected — show interchange marker
                cout << "\n  ================ INTERCHANGE ================\n";
                cout << "     " << currentLine << "\n";
                cout << "          |\n";
                cout << "          v  Change Here\n";
                cout << "     " << nextLine << "\n";
                cout << "  =============================================\n\n";
                currentLine = nextLine;
            }

            cout << "     |\n";
            cout << "     v\n";
        }
    }

    cout << "\n--------------------------------------------------\n\n";
    cout << "  " << left << setw(20) << "Distance"
         << ": " << info.route.distanceKm << " km\n";
    cout << "  " << left << setw(20) << "Estimated Time"
         << ": " << info.route.timeMinutes << " minutes\n";
    cout << "  " << left << setw(20) << "Fare"
         << ": Rs. " << info.fare << "\n";
    cout << "  " << left << setw(20) << "Interchanges"
         << ": " << info.interchangeCount << "\n";
    cout << "\n==================================================\n\n";
}

// -----------------------------------------------------------------------
// Resolves a user-typed station name or code to a valid full name.
// Supports exact match, code lookup, and case-insensitive partial search.
// -----------------------------------------------------------------------
string resolveStation(const string& input, const MetroMap& metro) {
    if (metro.stationExists(input)) return input;

    // Try as a code (case-insensitive)
    string upperInput = input;
    transform(upperInput.begin(), upperInput.end(), upperInput.begin(), ::toupper);
    if (metro.stationExistsByCode(upperInput)) {
        return metro.getNameByCode(upperInput);
    }

    // Partial name search
    vector<string> results = metro.searchStation(input);
    if (results.empty()) return "";

    if (results.size() == 1) return results[0];

    // Multiple matches — ask user to pick one
    cout << "\n  Multiple stations found:\n\n";
    for (int i = 0; i < (int)results.size(); i++) {
        cout << "  " << (i + 1) << ". " << results[i] << "\n";
    }
    cout << "\n  Enter choice (1-" << results.size() << "): ";
    int choice = -1;
    if (cin >> choice) {
        cin.ignore();
        if (choice >= 1 && choice <= (int)results.size()) {
            return results[choice - 1];
        }
    } else {
        cin.clear();
        cin.ignore();
    }
    return "";
}

// -----------------------------------------------------------------------
// Menu handlers
// -----------------------------------------------------------------------

void handleSearch(const MetroMap& metro) {
    cout << "\n  Enter station name or code to search: ";
    string query;
    getline(cin, query);

    vector<string> results = metro.searchStation(query);
    if (results.empty()) {
        cout << "\n  No stations found matching \"" << query << "\".\n\n";
        return;
    }

    cout << "\n  Search Results for \"" << query << "\"\n\n";
    for (int i = 0; i < (int)results.size(); i++) {
        const MetroStation& s = metro.getStation(results[i]);
        cout << "  " << (i + 1) << ". " << left << setw(30) << results[i]
             << "  [" << s.getCode() << "]";
        if (s.isInterchange()) cout << "  *Interchange*";
        cout << "\n";
    }
    cout << "\n";
}

void handleStationInfo(const MetroMap& metro) {
    cout << "\n  Enter station name or code: ";
    string input;
    getline(cin, input);
    string name = resolveStation(input, metro);
    if (name.empty()) {
        cout << "\n  Station not found: \"" << input << "\"\n\n";
        return;
    }
    metro.displayStationInfo(name);
}

void handleFindRoute(const MetroMap& metro) {
    cout << "\n  Enter Source Station      : ";
    string srcInput;
    getline(cin, srcInput);

    cout << "  Enter Destination Station : ";
    string dstInput;
    getline(cin, dstInput);

    string src = resolveStation(srcInput, metro);
    string dst = resolveStation(dstInput, metro);

    if (src.empty()) {
        cout << "\n  [Error] Source station not found: \"" << srcInput << "\"\n\n";
        return;
    }
    if (dst.empty()) {
        cout << "\n  [Error] Destination station not found: \"" << dstInput << "\"\n\n";
        return;
    }
    if (src == dst) {
        cout << "\n  Source and destination are the same station.\n\n";
        return;
    }
    if (!metro.getGraph().hasPath(src, dst)) {
        cout << "\n  [Error] No path found between these stations.\n\n";
        return;
    }

    cout << "\n  ----------------------------------------------------------\n";
    cout << "  Select Route Type\n\n";
    cout << "  1. Shortest Distance\n";
    cout << "  2. Minimum Time\n\n";
    cout << "  Choice: ";

    int choice = -1;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(9999, '\n');
        cout << "\n  Invalid choice.\n\n";
        return;
    }
    cin.ignore();

    RouteFinder finder(metro.getGraph(), metro.getAllStations());

    if (choice == 1) {
        JourneyInfo info = finder.findShortestDistance(src, dst);
        if (!info.route.valid) {
            cout << "\n  Could not calculate route.\n\n";
            return;
        }
        displayJourney(info, "Shortest Distance", metro);
    } else if (choice == 2) {
        JourneyInfo info = finder.findFastestRoute(src, dst);
        if (!info.route.valid) {
            cout << "\n  Could not calculate route.\n\n";
            return;
        }
        displayJourney(info, "Minimum Time", metro);
    } else {
        cout << "\n  Invalid choice. Please enter 1 or 2.\n\n";
    }
}

void handleStationsByLine(const MetroMap& metro) {
    vector<string> lines = metro.getAllLines();
    cout << "\n  Available Lines\n\n";
    for (int i = 0; i < (int)lines.size(); i++) {
        cout << "  " << (i + 1) << ". " << lines[i] << "\n";
    }
    cout << "\n  Enter line number: ";
    int choice = -1;
    if (!(cin >> choice) || choice < 1 || choice > (int)lines.size()) {
        cin.clear();
        cin.ignore(9999, '\n');
        cout << "\n  Invalid choice.\n\n";
        return;
    }
    cin.ignore();
    metro.displayStationsByLine(lines[choice - 1]);
}

// -----------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Load data from files
    FileManager fm;
    vector<MetroStation> stationList = fm.loadStations("stations.txt");
    auto connectionList = fm.loadConnections("connections.txt");

    if (stationList.empty()) {
        cerr << "  Failed to load station data. Exiting.\n";
        return 1;
    }

    // Build the metro map
    MetroMap metro;
    for (const MetroStation& s : stationList) {
        metro.addStation(s);
    }
    for (const auto& [c1, c2, dist] : connectionList) {
        metro.addConnection(c1, c2, dist);
    }

    // Startup banner
    cout << "\n==========================================================\n";
    cout << "          DELHI METRO NAVIGATION SYSTEM\n";
    cout << "        (OOP + Graph + Dijkstra Project)\n";
    cout << "==========================================================\n\n";
    cout << "  Metro Network Loaded Successfully\n\n";
    cout << "  " << left << setw(16) << "Stations"
         << ": " << metro.getStationCount() << "\n";
    cout << "  " << left << setw(16) << "Lines"
         << ": " << metro.getAllLines().size() << "\n";
    cout << "  " << left << setw(16) << "Interchanges"
         << ": " << metro.getInterchangeCount() << "\n";

    // Main menu loop
    while (true) {
        cout << "\n---------------- MAIN MENU ----------------\n\n";
        cout << "  1. Display All Stations\n";
        cout << "  2. Display Metro Network\n";
        cout << "  3. Search Station\n";
        cout << "  4. Station Information\n";
        cout << "  5. Find Route\n";
        cout << "  6. Show Stations by Line\n";
        cout << "  7. Exit\n\n";
        cout << "  Enter your choice: ";

        int choice = -1;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(9999, '\n');
            cout << "\n  Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore();
        cout << "\n----------------------------------------------------------\n";

        switch (choice) {
        case 1:
            metro.displayAllStations();
            break;
        case 2:
            metro.displayNetwork();
            break;
        case 3:
            handleSearch(metro);
            break;
        case 4:
            handleStationInfo(metro);
            break;
        case 5:
            handleFindRoute(metro);
            break;
        case 6:
            handleStationsByLine(metro);
            break;
        case 7:
            cout << "\n  Thank you for using Delhi Metro Navigation System.\n";
            cout << "  Have a safe journey!\n\n";
            return 0;
        default:
            cout << "\n  Invalid choice. Please enter 1 to 7.\n\n";
        }
    }
}
