#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <vector>
#include <cmath>

using namespace geode::prelude;

/**
 * EliteIndicatorNode: A custom CCNode for high-performance vector rendering.
 * Features: Layered vector arrow, expanding shockwave on gravity flip,
 * and per-player color matching.
 */
class EliteIndicatorNode : public cocos2d::CCNode {
protected:
    cocos2d::CCDrawNode* m_drawNode = nullptr;
    cocos2d::CCDrawNode* m_shockwave = nullptr;
    
    float m_baseScale = 0.6f;
    float m_vOffset = 20.0f;
    bool m_isSecondPlayer = false;
    
    // Shockwave Animation State
    float m_swRadius = 0.0f;
    float m_swOpacity = 0.0f;
    bool m_swActive = false;

    bool init(bool isSecondPlayer) {
        if (!cocos2d::CCNode::init()) return false;
        
        m_isSecondPlayer = isSecondPlayer;
        m_drawNode = cocos2d::CCDrawNode::create();
        m_shockwave = cocos2d::CCDrawNode::create();
        
        if (m_drawNode) this->addChild(m_drawNode);
        if (m_shockwave) this->addChild(m_shockwave);
        
        this->updateSettings();
        this->scheduleUpdate();
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
        m_baseScale = static_cast<float>(Mod::get()->getSettingValue<double>("arrow-scale"));
        m_vOffset = static_cast<float>(Mod::get()->getSettingValue<double>("v-offset"));
        this->setVisible(Mod::get()->getSettingValue<bool>("show-arrow"));
        this->setScale(m_baseScale);
    }

    void triggerFlipEffect() {
        m_swRadius = 0.0f;
        m_swOpacity = 1.0f;
        m_swActive = true;
    }

    void update(float dt) {
        if (m_swActive && m_shockwave) {
            m_shockwave->clear();
            m_swRadius += dt * 150.0f;
            m_swOpacity -= dt * 3.5f;
            
            if (m_swOpacity <= 0) {
                m_swActive = false;
            } else {
                cocos2d::ccColor4F swColor = {1.0f, 1.0f, 1.0f, m_swOpacity * 0.4f};
                const int segments = 32;
                std::vector<cocos2d::CCPoint> points;
                for (int i = 0; i < segments; i++) {
                    float angle = 6.28318f * i / segments;
                    points.push_back(ccp(cosf(angle) * m_swRadius, sinf(angle) * m_swRadius));
                }
                m_shockwave->drawPolygon(points.data(), segments, swColor, 0.0f, swColor);
            }
        }
    }

    void updateState(PlayerObject* player) {
        if (!player || !m_drawNode) return;

        m_drawNode->clear();
        bool isSwing = player->m_isSwing;
        this->setVisible(isSwing && Mod::get()->getSettingValue<bool>("show-arrow"));
        if (!this->isVisible()) return;

        float yPos = player->m_isUpsideDown ? -m_vOffset : m_vOffset;
        this->setPosition(ccp(0, yPos));
        
        cocos2d::ccColor3B pColor = m_isSecondPlayer ? player->m_playerColor2 : player->m_playerColor1;
        cocos2d::ccColor4F colorMain = ccc4FFromccc3B(pColor);
        cocos2d::ccColor4F colorGlow = colorMain;
        colorGlow.a = 0.3f;
        
        float direction = player->m_isUpsideDown ? -1.0f : 1.0f;

        cocos2d::CCPoint pointsOuter[] = {
            ccp(-14, 0), ccp(14, 0), ccp(0, 20 * direction)
        };
        m_drawNode->drawPolygon(pointsOuter, 3, colorGlow, 0.5f, colorGlow);

        cocos2d::CCPoint pointsInner[] = {
            ccp(-9, 0), ccp(9, 0), ccp(0, 13 * direction)
        };
        m_drawNode->drawPolygon(pointsInner, 3, colorMain, 1.0f, colorMain);

        if (Mod::get()->getSettingValue<bool>("h-tracker")) {
            float velY = player->m_yVelocity;
            m_drawNode->drawSegment(ccp(-16, 0), ccp(-16, velY * 2.8f), 1.5f, colorMain);
        }
    }
};

class $modify(ElitePlayer, PlayerObject) {
    struct Fields {
        EliteIndicatorNode* m_indicator = nullptr;
    };

    void update(float dt) {
        PlayerObject::update(dt);
        
        if (!m_fields->m_indicator) {
            bool isSecondPlayer = (this == PlayLayer::get()->m_player2);
            m_fields->m_indicator = EliteIndicatorNode::create(isSecondPlayer);
            this->addChild(m_fields->m_indicator);
        }
        
        if (m_fields->m_indicator) {
            m_fields->m_indicator->updateState(this);
        }
    }

    void flipGravity(bool p0, bool p1) {
        PlayerObject::flipGravity(p0, p1);
        if (m_fields->m_indicator) {
            m_fields->m_indicator->triggerFlipEffect();
        }
    }
};

$execute {
    static auto listener = new EventListener<SettingChangedFilter>(+[](SettingChangedEvent* event) {
        auto pl = PlayLayer::get();
        if (!pl) return ListenerResult::Propagate;
        
        if (pl->m_player1) {
            if (auto field = static_cast<ElitePlayer*>(pl->m_player1)->m_fields.operator->()) {
                if (field->m_indicator) field->m_indicator->updateSettings();
            }
        }
        if (pl->m_player2) {
            if (auto field = static_cast<ElitePlayer*>(pl->m_player2)->m_fields.operator->()) {
                if (field->m_indicator) field->m_indicator->updateSettings();
            }
        }
        return ListenerResult::Propagate;
    }, SettingChangedFilter(Mod::get()->getID()));
}





