#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "RoulettePopup.hpp"

using namespace geode::prelude;

class $modify(LevelRouletteMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto bottomMenu = this->getChildByID("bottom-menu");

        if (!bottomMenu) {
            log::warn(
                "Level Roulette: bottom-menu was not found."
            );

            return true;
        }

        auto icon = CCSprite::create(
            "roulette-button.png"
        );

        if (!icon) {
            log::error(
                "Level Roulette: roulette-button.png could not be loaded."
            );

            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(
                LevelRouletteMenuLayer::onOpenLevelRoulette
            )
        );

        if (!button) {
            log::error(
                "Level Roulette: failed to create button."
            );

            return true;
        }

        button->setID("level-roulette-button"_spr);

        auto menu = typeinfo_cast<CCMenu*>(bottomMenu);

        if (!menu) {
            return true;
        }

        menu->addChild(button);
        menu->updateLayout();

        return true;
    }

    void onOpenLevelRoulette(CCObject*) {

        auto popup =
            level_roulette::RoulettePopup::create();

        if (popup) {
            popup->show();
        }
    }
};
