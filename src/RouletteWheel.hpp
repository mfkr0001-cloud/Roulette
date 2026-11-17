#pragma once

#include <Geode/Geode.hpp>
#include <functional>
#include <vector>

namespace level_roulette {

class RouletteWheel : public cocos2d::CCNode {
public:
    static RouletteWheel* create(const std::vector<int>& levelIDs);
    bool init(const std::vector<int>& levelIDs);

    bool isSpinning() const;
    bool spin(std::function<void(int)> onFinished);
    void setLevelIDs(const std::vector<int>& levelIDs);

private:
    void rebuild();
    void update(float dt);

    std::vector<int> m_levelIDs;
    cocos2d::CCNode* m_wheelVisual = nullptr;
    bool m_spinning = false;
    float m_startRotation = 0.f;
    float m_targetRotation = 0.f;
    float m_elapsed = 0.f;
    float m_duration = 5.2f;
    int m_winner = 0;
    std::function<void(int)> m_onFinished;
};

} // namespace level_roulette
