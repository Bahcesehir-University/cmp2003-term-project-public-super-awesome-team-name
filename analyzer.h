#pragma once
#include <string>
#include <vector>

struct ZoneCount {
    string zone;
    long long count;
};

struct SlotCount {
    string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
public:
    void ingestStdin();
    vector<ZoneCount> topZones();
    vector<SlotCount> topBusySlots();

private:
    unordered_map<string, long long> zoneCounts;
    unordered_map<string, array<long long, 24>> slotCounts;
};
