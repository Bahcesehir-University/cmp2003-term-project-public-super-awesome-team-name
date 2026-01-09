#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
public:
    void ingestStdin();
    std::vector<ZoneCount> topZones();
    std::vector<SlotCount> topBusySlots();

private:
    std::unordered_map<std::string, long long> zoneCounts;
    std::unordered_map<std::string, std::array<long long, 24>> slotCounts;
};

#endif
