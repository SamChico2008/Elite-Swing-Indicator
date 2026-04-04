#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/loader/SettingEvent.hpp>

using namespace geode::prelude;

struct EliteIndicator : public CCNode {
    Ref<CCDrawNode> m_canvas;
    float m_targetRotation = 0.f;
    float m_currentRotation = 0.f;
    float m_velocitySmooth = 0.f;
    
    bool init() override {
        if (!CCNode::init()) return false;
        
        m_canvas = CCDrawNode::create();
        this->addChild(m_canvas);
        
        this->updateSettings();
        return true;
    }

    void updateSettings() {
        m_canvas->clear();
        this->setVisible(Mod::get()->getSettingValue<bool>("show-arrow"));
        this->setScale(Mod::get()->getSettingValue<double>("arrow-scale"));
        
        // Pre-draw static parts of the HUD if needed, 
        // but for dynamic HUD we mostly draw in update
    }

    void updateState(float dt, float velocity, bool isUpsideDown, float playerRot) {
        m_canvas->clear();
        
        float vOffset = static_cast<float>(Mod::get()->getSettingValue<double>("v-offset"));
        
        // 1. Gravity Arrow Logic
        m_targetRotation = isUpsideDown ? -90.f : 90.f;
        m_currentRotation = std::lerp(m_currentRotation, m_targetRotation, 15.f * dt);
        
        // Position relative to player center (0,0)
        CCPoint arrowPos = CCPoint{ 0, isUpsideDown ? (15.f + vOffset) : (-15.f - vOffset) };
        
        // Draw Triangle (Elite Style)
        float size = 8.f;
        std::array<CCPoint, 3> verts;
        verts[0] = { -size, -size * 0.6f };
        verts[1] = { size, -size * 0.6f };
        verts[2] = { 0, size * 0.8f };
        
        // Rotate vertices
        float rad = -m_currentRotation * M_PI / 180.f;
        auto rotateVec = [&](CCPoint p) {
            return CCPoint {
                p.x * cos(rad) - p.y * sin(rad),
                p.x * sin(rad) + p.y * cos(rad)
            };
        };
        
        for (auto& v : verts) v = rotateVec(v) + arrowPos;
        
        m_canvas->drawPolygon(verts.data(), 3, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 1.f, 1.f, 1.f, 0.4f });

        // 2. Velocity Tracker Logic
        if (Mod::get()->getSettingValue<bool>("h-tracker")) {
            m_velocitySmooth = std::lerp(m_velocitySmooth, velocity, 10.f * dt);
            float hudScale = static_cast<float>(Mod::get()->getSettingValue<double>("hud-scale"));
            
            float barH = 30.f * hudScale;
            float barW = 3.f * hudScale;
            CCPoint barBase = { 25.f, 0.f };
            
            // Background
            m_canvas->drawRect(
                barBase - CCPoint{barW/2, barH/2}, 
                barBase + CCPoint{barW/2, barH/2}, 
                {0,0,0,0.3f}, 0, {0,0,0,0}
            );
            
            // Indicator
            float valNorm = std::clamp(m_velocitySmooth / 15.f, -1.f, 1.f);
            float indicatorY = (barH / 2.f) * valNorm;
            
            m_canvas->drawRect(
                barBase + CCPoint{-barW, indicatorY - 1.f}, 
                barBase + CCPoint{barW, indicatorY + 1.f}, 
                {1,1,1,1}, 0.5f, {1,1,1,0.5f}
            );
        }
        
        // Global rotation compensation (parent is PlayerObject)
        this->setRotation(-playerRot);
    }

    static EliteIndicator* create() {
        auto ret = new EliteIndicator();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(MyPlayer, PlayerObject) {
    struct Fields {
        EliteIndicator* m_indicator = nullptr;
        EventListener<SettingEventFilter> m_settingsListener;
    };

    bool init(int p0, int p1, GJBaseGameLayer* p2, Cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

        m_fields->m_indicator = EliteIndicator::create();
        this->addChild(m_fields->m_indicator, 100);

        m_fields->m_settingsListener.bind([this](SettingEvent* event) {
            m_fields->m_indicator->updateSettings();
        });

        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);
        
        if (m_fields->m_indicator) {
            bool isSwing = this->m_isSwingcopter || (this->m_isBird && Mod::get()->getSettingValue<bool>("show-arrow")); // Simple logic for HUD
            m_fields->m_indicator->setVisible(isSwing);
            
            if (isSwing) {
                m_fields->m_indicator->updateState(
                    dt, 
                    this->m_yVelocity, 
                    this->m_isUpsideDown, 
                    this->getRotation()
                );
            }
        }
    }
};
