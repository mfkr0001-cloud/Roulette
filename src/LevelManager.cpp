#include "LevelManager.hpp"

#include <algorithm>
#include <sstream>

using namespace geode::prelude;

namespace level_roulette {

std::vector<int> LevelManager::s_levels;
bool LevelManager::s_loaded = false;

void LevelManager::ensureLoaded() {
    if (s_loaded) return;

    s_levels.clear();
    auto stored = Mod::get()->getSavedValue<std::string>("level-ids", "");
    std::stringstream stream(stored);
    std::string part;

    while (std::getline(stream, part, ',')) {
        auto parsed = numFromString<int>(part);
        if (parsed.isOk() && parsed.unwrap() > 0) {
            const int id = parsed.unwrap();
            if (std::find(s_levels.begin(), s_levels.end(), id) == s_levels.end()) {
                s_levels.push_back(id);
            }
        }
    }

    s_loaded = true;
}

std::vector<int> LevelManager::load() {
    ensureLoaded();
    return s_levels;
}

bool LevelManager::add(int levelID, std::string& error) {
    ensureLoaded();

    if (levelID <= 0) {
        error = "Level ID must be a positive number.";
        return false;
    }

    if (std::find(s_levels.begin(), s_levels.end(), levelID) != s_levels.end()) {
        error = "This level ID is already in the roulette.";
        return false;
    }

    s_levels.push_back(levelID);
    save();
    return true;
}

bool LevelManager::removeAt(size_t index) {
    ensureLoaded();
    if (index >= s_levels.size()) return false;

    s_levels.erase(s_levels.begin() + static_cast<std::ptrdiff_t>(index));
    save();
    return true;
}

bool LevelManager::removeLast() {
    ensureLoaded();
    if (s_levels.empty()) return false;

    s_levels.pop_back();
    save();
    return true;
}

const std::vector<int>& LevelManager::levels() {
    ensureLoaded();
    return s_levels;
}

void LevelManager::save() {
    ensureLoaded();

    std::string serialized;
    for (size_t i = 0; i < s_levels.size(); ++i) {
        if (i != 0) serialized += ',';
        serialized += std::to_string(s_levels[i]);
    }

    Mod::get()->setSavedValue("level-ids", serialized);
}

} // namespace level_roulette
