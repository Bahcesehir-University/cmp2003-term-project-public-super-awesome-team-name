#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>

static int safeParseHour(const std::string& dateStr) {
    if (dateStr.length() < 13) return -1;

    size_t spacePos = dateStr.find(' ');
    if (spacePos == std::string::npos) return -1;

    if (spacePos + 3 > dateStr.size()) return -1;

    char c1 = dateStr[spacePos + 1];
    char c2 = dateStr[spacePos + 2];

    if (c1 < '0' || c1 > '9' || c2 < '0' || c2 > '9') return -1;

    int h = (c1 - '0') * 10 + (c2 - '0');
    if (h >= 0 && h <= 23) return h;

    return -1;
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t pos1 = line.find(',');
        if (pos1 == std::string::npos) continue;

        size_t pos2 = line.find(',', pos1 + 1);
        if (pos2 == std::string::npos) continue;

        if (pos2 - pos1 <= 1) continue;
        std::string zoneID = line.substr(pos1 + 1, pos2 - (pos1 + 1));

        size_t pos3 = line.find(',', pos2 + 1);
        if (pos3 == std::string::npos) continue;

        size_t pos4 = line.find(',', pos3 + 1);
        if (pos4 == std::string::npos) continue; 
        
        std::string dateStr = line.substr(pos3 + 1, pos4 - (pos3 + 1));

        int hour = safeParseHour(dateStr);
        if (hour == -1) continue;

        zone_counts[zoneID]++;
        zone_hourly_counts[zoneID][hour]++;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> results;
    results.reserve(zone_counts.size());

    for (const auto& kv : zone_counts) {
        results.push_back({kv.first, kv.second});
    }

    std::sort(results.begin(), results.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        return a.zone < b.zone;
    });

    if ((size_t)k < results.size()) {
        results.resize(k);
    }

    return results;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> results;
    results.reserve(zone_hourly_counts.size() * 12); 

    for (const auto& pair : zone_hourly_counts) {
        const std::string& zone = pair.first;
        const auto& hours = pair.second;

        for (int h = 0; h < 24; ++h) {
            if (hours[h] > 0) {
                results.push_back({zone, h, hours[h]});
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        if (a.zone != b.zone) {
            return a.zone < b.zone;
        }
        return a.hour < b.hour;
    });

    if ((size_t)k < results.size()) {
        results.resize(k);
    }

    return results;
}
