#include "FareCalculator.h"
#include <iostream>

using namespace std;

// DMRC fare slabs as of 2024
int FareCalculator::calculateFare(int distanceKm) {
    if (distanceKm <= 2)  return 10;
    if (distanceKm <= 5)  return 20;
    if (distanceKm <= 12) return 30;
    if (distanceKm <= 21) return 40;
    if (distanceKm <= 32) return 50;
    return 60;
}

void FareCalculator::displayFareChart() {
    cout << "\n  ------------------------------------------\n";
    cout << "   DMRC Fare Chart (2024)\n";
    cout << "  ------------------------------------------\n";
    cout << "   Distance              Fare\n";
    cout << "  ------------------------------------------\n";
    cout << "   0  - 2  km           Rs. 10\n";
    cout << "   2  - 5  km           Rs. 20\n";
    cout << "   5  - 12 km           Rs. 30\n";
    cout << "   12 - 21 km           Rs. 40\n";
    cout << "   21 - 32 km           Rs. 50\n";
    cout << "   32+     km           Rs. 60\n";
    cout << "  ------------------------------------------\n\n";
}
