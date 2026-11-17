#include "RoulettePopup.hpp"
#include "LevelManager.hpp"

using namespace geode::prelude;

namespace level_roulette {

RoulettePopup* RoulettePopup::create() {
    auto ret = new RoulettePopup();

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool RoulettePopup::init() {
    if (!Popup::init(380.f, 280.f)) {
        return false;
    }

    this->setTitle("LEVEL ROULETTE");

    auto winSize = this->getContentSize();

    auto center = CCPoint{
        winSize.width / 2.f,
        winSize.height / 2.f
    };

    // Create roulette wheel
    m_wheel = RouletteWheel::create(LevelManager::load());

    if (!m_wheel) {
        return false;
    }

    m_wheel->setPosition({
        center.x - 65.f,
        center.y + 12.f
    });

    this->addChild(m_wheel);

    // Fixed pointer
    auto pointer = CCSprite::create("GJ_arrow_01_001.png");

    if (pointer) {
        pointer->setRotation(90.f);
        pointer->setScale(0.62f);

        pointer->setPosition({
            center.x - 65.f,
            center.y + 145.f
        });

        this->addChild(pointer, 5);
    }

    // Menu
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    this->addChild(menu, 6);

    // Spin button
    auto spinSprite = ButtonSprite::create(
        "SPIN",
        42,
        true,
        "goldFont.fnt",
        "GJ_button_01.png",
        30.f,
        1.f
    );

    auto spinButton = CCMenuItemSpriteExtra::create(
        spinSprite,
        this,
        menu_selector(RoulettePopup::onSpin)
    );

    spinButton->setPosition({
        center.x - 65.f,
        center.y + 12.f
    });

    menu->addChild(spinButton);

    // Level ID input
    m_levelInput = TextInput::create(100.f, "Level ID");

    if (m_levelInput) {
        m_levelInput->setPosition({
            center.x + 112.f,
            center.y + 85.f
        });

        this->addChild(m_levelInput);
    }

    // Add button
    auto addSprite = ButtonSprite::create(
        "+",
        30,
        true,
        "goldFont.fnt",
        "GJ_button_01.png",
        24.f,
        1.f
    );

    auto addButton = CCMenuItemSpriteExtra::create(
        addSprite,
        this,
        menu_selector(RoulettePopup::onAdd)
    );

    addButton->setPosition({
        center.x + 112.f,
        center.y + 45.f
    });

    menu->addChild(addButton);

    // Remove last button
    auto removeSprite = ButtonSprite::create(
        "REMOVE LAST",
        70,
        true,
        "goldFont.fnt",
        "GJ_button_06.png",
        24.f,
        1.f
    );

    auto removeButton = CCMenuItemSpriteExtra::create(
        removeSprite,
        this,
        menu_selector(RoulettePopup::onRemoveLast)
    );

    removeButton->setPosition({
        center.x + 112.f,
        center.y + 5.f
    });

    menu->addChild(removeButton);

    // Count label
    m_countLabel = CCLabelBMFont::create(
        "",
        "bigFont.fnt"
    );

    m_countLabel->setScale(0.42f);

    m_countLabel->setPosition({
        center.x + 112.f,
        center.y - 30.f
    });

    this->addChild(m_countLabel);

    // Level list
    m_listLabel = CCLabelBMFont::create(
        "",
        "chatFont.fnt"
    );

    m_listLabel->setScale(0.42f);
    m_listLabel->setAnchorPoint({0.5f, 1.f});

    m_listLabel->setPosition({
        center.x + 112.f,
        center.y - 48.f
    });

    this->addChild(m_listLabel);

    // Result label
    m_resultLabel = CCLabelBMFont::create(
        "Add levels, then spin!",
        "goldFont.fnt"
    );

    m_resultLabel->setScale(0.43f);

    m_resultLabel->setPosition({
        center.x - 65.f,
        center.y - 120.f
    });

    this->addChild(m_resultLabel);

    refreshList();

    return true;
}

void RoulettePopup::refreshList() {
    const auto& ids = LevelManager::levels();

    if (m_countLabel) {
        auto count =
            "LEVELS: " + std::to_string(ids.size());

        m_countLabel->setString(count.c_str());
    }

    if (m_listLabel) {
        std::string list =
            ids.empty() ? "No levels added" : "";

        for (size_t i = 0; i < ids.size(); ++i) {
            if (i) {
                list += "\n";
            }

            list += std::to_string(ids[i]);
        }

        m_listLabel->setString(list.c_str());
    }

    if (m_wheel) {
        m_wheel->setLevelIDs(ids);
    }
}

void RoulettePopup::onAdd(CCObject*) {
    if (!m_levelInput) {
        return;
    }

    auto value = m_levelInput->getString();

    auto parsed = numFromString<int>(value);

    if (
        value.empty() ||
        !parsed.isOk() ||
        parsed.unwrap() <= 0
    ) {
        FLAlertLayer::create(
            "Invalid ID",
            "Enter a positive numeric Geometry Dash level ID.",
            "OK"
        )->show();

        return;
    }

    std::string error;

    if (!LevelManager::add(parsed.unwrap(), error)) {
        FLAlertLayer::create(
            "Cannot add level",
            error,
            "OK"
        )->show();

        return;
    }

    m_levelInput->setString("");

    refreshList();
}

void RoulettePopup::onRemoveLast(CCObject*) {
    if (!LevelManager::removeLast()) {
        FLAlertLayer::create(
            "Level Roulette",
            "There are no levels to remove.",
            "OK"
        )->show();

        return;
    }

    refreshList();
}

void RoulettePopup::onSpin(CCObject*) {
    if (!m_wheel || m_wheel->isSpinning()) {
        return;
    }

    if (LevelManager::levels().empty()) {
        FLAlertLayer::create(
            "Level Roulette",
            "Add at least one level before spinning.",
            "OK"
        )->show();

        return;
    }

    m_resultLabel->setString("Spinning...");

    bool started = m_wheel->spin(
        [this](int levelID) {
            setResult(levelID);
        }
    );

    if (!started) {
        m_resultLabel->setString("Unable to start");
    }
}

void RoulettePopup::setResult(int levelID) {
    auto text =
        "SELECTED LEVEL: " + std::to_string(levelID);

    m_resultLabel->setString(text.c_str());
}

}
