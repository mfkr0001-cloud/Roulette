#include "RoulettePopup.hpp"
#include "LevelManager.hpp"

using namespace geode::prelude;

namespace level_roulette {

bool RoulettePopup::setup() {
    this->setTitle("LEVEL ROULETTE");

    auto winSize = m_mainLayer->getContentSize();
    auto center = CCPoint{winSize.width / 2.f, winSize.height / 2.f};

    m_wheel = RouletteWheel::create(LevelManager::load());
    if (!m_wheel) return false;
    m_wheel->setPosition({center.x - 42.f, center.y + 18.f});
    m_mainLayer->addChild(m_wheel);

    // Fixed pointer at the top of the wheel.
    auto pointer = CCSprite::create("GJ_arrow_01_001.png");
    if (pointer) {
        pointer->setRotation(90.f);
        pointer->setScale(0.62f);
        pointer->setPosition({center.x - 42.f, center.y + 151.f});
        m_mainLayer->addChild(pointer, 5);
    }

    auto spinSprite = ButtonSprite::create("SPIN", 42, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 1.f);
    auto spinButton = CCMenuItemSpriteExtra::create(spinSprite, this, menu_selector(RoulettePopup::onSpin));
    spinButton->setPosition({center.x - 42.f, center.y + 18.f});
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    menu->addChild(spinButton);
    m_mainLayer->addChild(menu, 6);

    m_levelInput = TextInput::create(92.f, "Level ID");
    if (m_levelInput) {
        m_levelInput->setPosition({center.x + 103.f, center.y + 84.f});
        m_mainLayer->addChild(m_levelInput);
    }

    auto addSprite = ButtonSprite::create("+", 30, true, "goldFont.fnt", "GJ_button_01.png", 24.f, 1.f);
    auto addButton = CCMenuItemSpriteExtra::create(addSprite, this, menu_selector(RoulettePopup::onAdd));
    addButton->setPosition({center.x + 103.f, center.y + 48.f});
    menu->addChild(addButton);

    auto removeSprite = ButtonSprite::create("REMOVE LAST", 70, true, "goldFont.fnt", "GJ_button_06.png", 24.f, 1.f);
    auto removeButton = CCMenuItemSpriteExtra::create(removeSprite, this, menu_selector(RoulettePopup::onRemoveLast));
    removeButton->setPosition({center.x + 103.f, center.y + 8.f});
    menu->addChild(removeButton);

    m_countLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_countLabel->setScale(0.42f);
    m_countLabel->setPosition({center.x + 103.f, center.y - 28.f});
    m_mainLayer->addChild(m_countLabel);

    m_listLabel = CCLabelBMFont::create("", "chatFont.fnt");
    m_listLabel->setScale(0.42f);
    m_listLabel->setAnchorPoint({0.5f, 1.f});
    m_listLabel->setPosition({center.x + 103.f, center.y - 48.f});
    m_mainLayer->addChild(m_listLabel);

    m_resultLabel = CCLabelBMFont::create("Add levels, then spin!", "goldFont.fnt");
    m_resultLabel->setScale(0.43f);
    m_resultLabel->setPosition({center.x - 42.f, center.y - 122.f});
    m_mainLayer->addChild(m_resultLabel);

    refreshList();
    return true;
}

void RoulettePopup::refreshList() {
    const auto& ids = LevelManager::levels();
    if (m_countLabel) {
        const auto count = "LEVELS: " + std::to_string(ids.size());
        m_countLabel->setString(count.c_str());
    }

    if (m_listLabel) {
        std::string list = ids.empty() ? "No levels added" : "";
        for (size_t i = 0; i < ids.size(); ++i) {
            if (i) list += "\n";
            list += std::to_string(ids[i]);
        }
        m_listLabel->setString(list.c_str());
    }

    if (m_wheel) m_wheel->setLevelIDs(ids);
}

void RoulettePopup::onAdd(CCObject*) {
    if (!m_levelInput) return;

    auto value = m_levelInput->getString();
    auto parsed = numFromString<int>(value);
    if (value.empty() || !parsed.isOk() || parsed.unwrap() <= 0) {
        FLAlertLayer::create("Invalid ID", "Enter a positive numeric Geometry Dash level ID.", "OK")->show();
        return;
    }

    std::string error;
    if (!LevelManager::add(parsed.unwrap(), error)) {
        FLAlertLayer::create("Cannot add level", error, "OK")->show();
        return;
    }

    m_levelInput->setString("");
    refreshList();
}

void RoulettePopup::onRemoveLast(CCObject*) {
    if (!LevelManager::removeLast()) {
        FLAlertLayer::create("Level Roulette", "There are no levels to remove.", "OK")->show();
        return;
    }
    refreshList();
}

void RoulettePopup::onSpin(CCObject*) {
    if (!m_wheel || m_wheel->isSpinning()) return;
    if (LevelManager::levels().empty()) {
        FLAlertLayer::create("Level Roulette", "Add at least one level before spinning.", "OK")->show();
        return;
    }

    m_resultLabel->setString("Spinning...");
    const bool started = m_wheel->spin([this](int levelID) {
        setResult(levelID);
    });

    if (!started) {
        m_resultLabel->setString("Unable to start");
    }
}

void RoulettePopup::setResult(int levelID) {
    const auto text = "SELECTED LEVEL: " + std::to_string(levelID);
    m_resultLabel->setString(text.c_str());
}

} // namespace level_roulette
