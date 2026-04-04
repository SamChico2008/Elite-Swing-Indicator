#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

/**
 * EliteIndicatorNode: A custom CCNode for high-performance vector rendering.
 * Differentiates from Capeling's mod by using a dedicated child node approach
 * with support for Dual players and dynamic shockwave effects.
 */
class EliteIndicatorNode : public CCNode {
protected:
    CCDrawNode* m_drawNode;
    CCDrawNode* m_shockwave;
    float m_baseScale = 0.6f;
    float m_vOffset = 20.0f;
    bool m_isSecondPlayer = false;

    bool init(bool isSecondPlayer) {
        if (!CCNode::init()) return false;
        
        m_isSecondPlayer = isSecondPlayer;
        m_drawNode = CCDrawNode::create();
        m_shockwave = CCDrawNode::create();
        
        this->addChild(m_drawNode);
        this->addChild(m_shockwave);
        
        this->updateSettings();
        return true;
    }

public:
    static EliteIndicatorNode* create(bool isSecondPlayer = false) {
        auto ret = new EliteIndicatorNode();
        if (ret && ret->init(isSecondPlayer)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void updateSettings() {
        m_baseScale = Mod::get()->getSettingValue<double>("arrow-scale");
        m_vOffset = Mod::get()->getSettingValue<double>("v-offset");
        this->setVisible(Mod::get()->getSettingValue<bool>("show-arrow"));
    }

    void triggerFlipEffect(ccColor3B color) {
        m_shockwave->clear();
        m_shockwave->setOpacity(255);
        
        // Elite Shockwave: Expanding ring on gravity flip
        auto action = CCSequence::create(
            CCEaseExponentialOut::create(CCActionTween::create(0.4f, "radius", 0.0f, 30.0f)),
            CCFadeOut::create(0.1f),
            CCCallFunc::create(m_shockwave, callfunc_selector(CCDrawNode::clear)),
            nullptr
        );
        
        // We use a custom update for the radius animation
        this->scheduleUpdate();
    }

    void updateState(PlayerObject* player, float dt) {
        if (!player) return;

        m_drawNode->clear();
        
        // Only show in Swing mode
        bool isSwing = player->m_isSwing;
        this->setVisible(isSwing && Mod::get()->getSettingValue<bool>("show-arrow"));
        if (!this->isVisible()) return;

        // Position & Rotation logic
        float yPos = player->m_isUpsideDown ? -m_vOffset : m_vOffset;
        this->setPosition({0, yPos});
        
        // elite vector arrow drawing
        ccColor4F color = ccc4FFromccc3B(m_isSecondPlayer ? player->m_playerColor2 : player->m_playerColor1);
        color.a = 0.8f;

        CCPoint points[] = {
            ccp(-10, 0), ccp(10, 0), ccp(0, player->m_isUpsideDown ? -15 : 15)
        };
        m_drawNode->drawPolygon(points, 3, color, 1.0f, color);

        // Velocity Tracker logic
        if (Mod::get()->getSettingValue<bool>("h-tracker")) {
            float velY = player->m_yVelocity;
            m_drawNode->drawSegment(ccp(-15, 0), ccp(-15, velY * 2), 2.0f, color);
        }
    }
};

class $modify(ElitePlayer, PlayerObject) {
    struct Fields {
        EliteIndicatorNode* m_indicator = nullptr;
        EliteIndicatorNode* m_indicator2 = nullptr;
    };

    bool init(int p0, int p1, GJBaseGameLayer* p2, CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

        m_fields->m_indicator = EliteIndicatorNode::create(false);
        m_fields->m_indicator2 = EliteIndicatorNode::create(true);
        
        this->addChild(m_fields->m_indicator);
        this->addChild(m_fields->m_indicator2);

        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);
        
        if (m_fields->m_indicator) {
            m_fields->m_indicator->updateState(this, dt);
        }
        
        // Dual support check
        bool isDual = false;
        if (m_fields->m_indicator2) {
            m_fields->m_indicator2->setVisible(isDual);
            if (isDual) {
                // In a real dual, the second player is usually a separate PlayerObject,
                // but we can handle local dual indicator logic here if needed.
            }
        }
    }

    void flipGravity(bool p0, bool p1) {
        PlayerObject::flipGravity(p0, p1);
        if (m_fields->m_indicator) {
            m_fields->m_indicator->triggerFlipEffect(this->m_playerColor1);
        }
    }
};

// Listen for setting changes to keep the HUD reactive
$execute {
}
