#include "RouletteWheel.hpp"

#include <array>
#include <algorithm>
#include <cmath>
#include <random>

using namespace geode::prelude;

namespace level_roulette {

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float WHEEL_RADIUS = 112.f;
constexpr unsigned int ARC_STEPS = 18;

const std::array<ccColor4F, 8> COLORS = {{
    {0.95f, 0.20f, 0.22f, 1.f},
    {0.98f, 0.55f, 0.12f, 1.f},
    {0.95f, 0.82f, 0.16f, 1.f},
    {0.24f, 0.75f, 0.34f, 1.f},
    {0.12f, 0.68f, 0.88f, 1.f},
    {0.25f, 0.38f, 0.92f, 1.f},
    {0.62f, 0.30f, 0.86f, 1.f},
    {0.92f, 0.30f, 0.62f, 1.f}
}};

}

RouletteWheel* RouletteWheel::create(
    const std::vector<int>& levelIDs
) {
    auto ret = new RouletteWheel();

    if (ret && ret->init(levelIDs)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool RouletteWheel::init(
    const std::vector<int>& levelIDs
) {
    if (!CCNode::init()) {
        return false;
    }

    this->setContentSize({
        WHEEL_RADIUS * 2.f + 8.f,
        WHEEL_RADIUS * 2.f + 8.f
    });

    m_wheelVisual = CCNode::create();

    if (!m_wheelVisual) {
        return false;
    }

    m_wheelVisual->setPosition({
        this->getContentSize().width / 2.f,
        this->getContentSize().height / 2.f
    });

    this->addChild(m_wheelVisual);

    setLevelIDs(levelIDs);

    this->scheduleUpdate();

    return true;
}

bool RouletteWheel::isSpinning() const {
    return m_spinning;
}

void RouletteWheel::setLevelIDs(
    const std::vector<int>& levelIDs
) {
    if (m_spinning) {
        return;
    }

    m_levelIDs = levelIDs;

    rebuild();
}

void RouletteWheel::rebuild() {
    if (!m_wheelVisual) {
        return;
    }

    m_wheelVisual->removeAllChildrenWithCleanup(true);

    auto disk = CCDrawNode::create();

    if (!disk) {
        return;
    }

    const size_t count = m_levelIDs.size();

    // Empty wheel
    if (count == 0) {
        disk->drawDot(
            CCPointZero,
            WHEEL_RADIUS,
            ccc4FFromccc3B(ccc3(45, 48, 66))
        );

        m_wheelVisual->addChild(disk);

        auto empty = CCLabelBMFont::create(
            "ADD LEVELS",
            "goldFont.fnt"
        );

        if (empty) {
            empty->setScale(0.48f);
            empty->setPosition(CCPointZero);

            m_wheelVisual->addChild(empty);
        }

        return;
    }

    const float step = 360.f / static_cast<float>(count);

    // Draw colored sectors
    for (size_t i = 0; i < count; ++i) {

        std::vector<CCPoint> points;

        points.reserve(ARC_STEPS + 2);

        points.push_back(CCPointZero);

        const float start =
            (-90.f + step * static_cast<float>(i))
            * PI / 180.f;

        const float end =
            (-90.f + step * static_cast<float>(i + 1))
            * PI / 180.f;

        for (unsigned int j = 0; j <= ARC_STEPS; ++j) {

            const float t =
                static_cast<float>(j)
                / static_cast<float>(ARC_STEPS);

            const float angle =
                start + (end - start) * t;

            points.push_back({
                std::cos(angle) * WHEEL_RADIUS,
                std::sin(angle) * WHEEL_RADIUS
            });
        }

        const auto color = COLORS[i % COLORS.size()];

        disk->drawPolygon(
            points.data(),
            static_cast<unsigned int>(points.size()),
            color,
            2.f,
            ccc4FFromccc3B(ccc3(20, 20, 28))
        );

        // Level ID label
        const float middle = (start + end) * 0.5f;

        const float labelRadius =
            count <= 6 ? 69.f : 78.f;

        auto label = CCLabelBMFont::create(
            std::to_string(m_levelIDs[i]).c_str(),
            "goldFont.fnt"
        );

        if (label) {
            label->setScale(
                count > 12 ? 0.28f :
                count > 7 ? 0.34f : 0.42f
            );

            label->setPosition({
                std::cos(middle) * labelRadius,
                std::sin(middle) * labelRadius
            });

            label->setRotation(
                -middle * 180.f / PI
            );

            m_wheelVisual->addChild(label, 2);
        }
    }

    m_wheelVisual->addChild(disk, 1);

    // Outer circle and sector boundaries
    auto outline = CCDrawNode::create();

    if (outline) {

        outline->drawCircle(
            CCPointZero,
            WHEEL_RADIUS,
            ccColor4F{0.f, 0.f, 0.f, 0.f},
            3.f,
            ccc4FFromccc3B(ccc3(12, 12, 18)),
            72
        );

        for (size_t i = 0; i < count; ++i) {

            const float angle =
                (-90.f + step * static_cast<float>(i))
                * PI / 180.f;

            outline->drawSegment(
                CCPointZero,
                {
                    std::cos(angle) * WHEEL_RADIUS,
                    std::sin(angle) * WHEEL_RADIUS
                },
                1.8f,
                ccc4FFromccc3B(ccc3(12, 12, 18))
            );
        }

        m_wheelVisual->addChild(outline, 3);
    }
}

bool RouletteWheel::spin(
    std::function<void(int)> onFinished
) {
    if (m_spinning || m_levelIDs.empty()) {
        return false;
    }

    std::random_device rd;

    std::mt19937 generator(rd());

    std::uniform_int_distribution<size_t> pick(
        0,
        m_levelIDs.size() - 1
    );

    const size_t winnerIndex = pick(generator);

    const float step =
        360.f / static_cast<float>(m_levelIDs.size());

    const float centerAngle =
        -90.f +
        (static_cast<float>(winnerIndex) + 0.5f) * step;

    const float desired = -centerAngle;

    const float current =
        std::fmod(m_wheelVisual->getRotation(), 360.f);

    float delta = desired - current;

    while (delta < 0.f) {
        delta += 360.f;
    }

    // Five full rotations
    delta += 360.f * 5.f;

    m_startRotation =
        m_wheelVisual->getRotation();

    m_targetRotation =
        m_startRotation + delta;

    m_elapsed = 0.f;

    m_winner = m_levelIDs[winnerIndex];

    m_onFinished = std::move(onFinished);

    m_spinning = true;

    return true;
}

void RouletteWheel::update(float dt) {

    if (!m_spinning) {
        return;
    }

    m_elapsed += dt;

    const float t = std::min(
        m_elapsed / m_duration,
        1.f
    );

    // Ease out animation
    const float eased =
        1.f - std::pow(1.f - t, 4.f);

    m_wheelVisual->setRotation(
        m_startRotation +
        (m_targetRotation - m_startRotation) * eased
    );

    if (t >= 1.f) {

        m_spinning = false;

        m_wheelVisual->setRotation(
            std::fmod(m_targetRotation, 360.f)
        );

        auto callback = std::move(m_onFinished);

        if (callback) {
            callback(m_winner);
        }
    }
}

}
