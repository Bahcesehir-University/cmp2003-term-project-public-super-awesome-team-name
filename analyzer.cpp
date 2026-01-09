#include "analyzer.h"

#include <iostream>
#include <algorithm>

void TripAnalyzer::ingestStdin() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        int commaCount = 0;
        for (char c : line)
            if (c == ',') commaCount++;

        if (commaCount != 5) continue;

        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        size_t p3 = line.find(',', p2 + 1);
        size_t p4 = line.find(',', p3 + 1);

        if (p1 == std::string::npos || p2 == std::string::npos ||
            p3 == std::string::npos || p4 == std::string::npos)
            continue;

        std::string zone = line.substr(p1 + 1, p2 - p1 - 1);
        std::string pickupTime = line.substr(p3 + 1, p4 - p3 - 1);

        if (pickupTime.size() < 13) continue;

        char c1 = pickupTime[11];
        char c2 = pickupTime[12];
        if (c1 < '0' || c1 > '9' || c2 < '0' || c2 > '9') continue;

        int hour = (c1 - '0') * 10 + (c2 - '0');
        if (hour < 0 || hour > 23) continue;

        zoneCounts[zone]++;
        slotCounts[zone][hour]++;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones() {
    std::vector<ZoneCount> res;
    for (auto &kv : zoneCounts)
        res.push_back({kv.first, kv.second});

    std::sort(res.begin(), res.end(),
         [](const ZoneCount &a, const ZoneCount &b) {
             if (a.count != b.count) return a.count > b.count;
             return a.zone < b.zone;
         });

    if (res.size() > 10) res.resize(10);
    return res;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots() {
    std::vector<SlotCount> res;

    for (auto &z : slotCounts)
        for (int h = 0; h < 24; h++)
            if (z.second[h] > 0)
                res.push_back({z.first, h, z.second[h]});

    std::sort(res.begin(), res.end(),
         [](const SlotCount &a, const SlotCount &b) {
             if (a.count != b.count) return a.count > b.count;
             if (a.zone != b.zone) return a.zone < b.zone;
             return a.hour < b.hour;
         });

    if (res.size() > 10) res.resize(10);
    return res;
}
