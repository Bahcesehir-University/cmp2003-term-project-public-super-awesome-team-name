#pragma once
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
    int hour;              // 0–23
    long long count;
};

class TripAnalyzer {
private:
    // Zone ID -> Toplam sürüş sayısı
    std::unordered_map<std::string, long long> zone_counts;
    
    // Zone ID -> [Saat 0 sayacı, Saat 1 sayacı, ... Saat 23 sayacı]
    std::unordered_map<std::string, std::array<long long, 24>> zone_hourly_counts;

public:
    // Parse Trips.csv, skip dirty rows, never crash
    void ingestFile(const std::string& csvPath);

    // Top K zones: count desc, zone asc
    std::vector<ZoneCount> topZones(int k = 10) const;

    // Top K slots: count desc, zone asc, hour asc
    std::vector<SlotCount> topBusySlots(int k = 10) const;
};
