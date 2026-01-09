#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <cctype>

static int safeParseHour(const std::string& dateStr) {
    if (dateStr.length() < 13) return -1;

    size_t spacePos = dateStr.find(' ');
    if (spacePos == std::string::npos) return -1;

    size_t hourStart = spacePos + 1;
    if (hourStart + 1 >= dateStr.size()) return -1;

    char c1 = dateStr[hourStart];
    char c2 = dateStr[hourStart + 1];

    if (!isdigit(c1) || !isdigit(c2)) return -1;

    int h = (c1 - '0') * 10 + (c2 - '0');
    if (h < 0 || h > 23) return -1;

    return h;
}

void TripAnalyzer::ingestStream(std::istream& in) {
    std::string line;

    while (std::getline(in, line)) {
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

        std::string zoneID = line.substr(p1 + 1, p2 - p1 - 1);
        std::string dateStr = line.substr(p3 + 1, p4 - p3 - 1);

        int hour = safeParseHour(dateStr);
        if (hour == -1) continue;

        zone_counts[zoneID]++;
        zone_hourly_counts[zoneID][hour]++;
    }
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return;
    ingestStream(file);
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> results;
    results.reserve(zone_counts.size());

    for (const auto& kv : zone_counts)
        results.push_back({kv.first, kv.second});

    std::sort(results.begin(), results.end(),
        [](const ZoneCount& a, const ZoneCount& b) {
            if (a.count != b.count) return a.count > b.count;
            return a.zone < b.zone;
        });

    if ((size_t)k < results.size())
        results.resize(k);

    return results;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> results;

    for (const auto& z : zone_hourly_counts) {
        for (int h = 0; h < 24; ++h) {
            if (z.second[h] > 0)
                results.push_back({z.first, h, z.second[h]});
        }
    }

    std::sort(results.begin(), results.end(),
        [](const SlotCount& a, const SlotCount& b) {
            if (a.count != b.count) return a.count > b.count;
            if (a.zone != b.zone) return a.zone < b.zone;
            return a.hour < b.hour;
        });

    if ((size_t)k < results.size())
        results.resize(k);

    return results;
}
