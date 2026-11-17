#pragma once

#include <Geode/Geode.hpp>
#include "RouletteWheel.hpp"

namespace level_roulette {

class RoulettePopup : public geode::Popup {
public:
    static RoulettePopup* create();

protected:
    bool init() override;

private:
    void onAdd(cocos2d::CCObject*);
    void onRemoveLast(cocos2d::CCObject*);
    void onSpin(cocos2d::CCObject*);

    void refreshList();
    void setResult(int levelID);

    RouletteWheel* m_wheel = nullptr;
    geode::TextInput* m_levelInput = nullptr;

    cocos2d::CCLabelBMFont* m_countLabel = nullptr;
    cocos2d::CCLabelBMFont* m_listLabel = nullptr;
    cocos2d::CCLabelBMFont* m_resultLabel = nullptr;
};

}
