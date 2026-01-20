#include "ScreenFactory.h"

void ScreenFactory::Register(int state, ScreenCreator creator) {
    m_creators[state] = creator;
}

std::unique_ptr<GameScreen> ScreenFactory::Create(int state,ScreenManager* manager) {
    auto it = m_creators.find(state);
    if (it != m_creators.end()) {
        auto screen = (it->second)(manager);
        screen->screenManager = manager;
        return screen;
    }
    return nullptr;
}