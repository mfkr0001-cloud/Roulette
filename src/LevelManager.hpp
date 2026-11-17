#pragma once

#include <Geode/Geode.hpp>
#include <string>
#include <vector>

namespace level_roulette {

class LevelManager {
public:
    static std::vector<int> load();
    static bool add(int levelID, std::string& error);
    static bool removeAt(size_t index);
    static bool removeLast();
    static const std::vector<int>& levels();
    static void save();

private:
    static std::vector<int> s_levels;
    static bool s_loaded;
    static void ensureLoaded();
};

} // namespace level_roulette
