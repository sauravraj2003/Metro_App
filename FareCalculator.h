#pragma once

// DMRC fare slabs (2024 rates).
// Kept separate because fare rules can change independently of routing logic.
class FareCalculator {
public:
    static int calculateFare(int distanceKm);
    static void displayFareChart();
};
