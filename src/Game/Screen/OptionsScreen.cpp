#include "OptionsScreen.h"
#include "raylib.h"
#include "Engine/System/Screen/ScreenManager.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <cstdio>
OptionsScreen::OptionsScreen()
    : m_nextScreenState(static_cast<int>(ScreenStateID::NONE)),
      m_fullscreenToggle(false),
      m_resolutionDropdownActive(0),
      m_resolutionDropdown(false)
{
}
OptionsScreen::~OptionsScreen()
{
    OnExit();
}
void OptionsScreen::OnEnter()
{
    m_currentConfig.load("assets/config/engine_config.json");

    m_modifiedConfig = m_currentConfig;

    m_fullscreenToggle = m_modifiedConfig.fullScreen;
    for (int i = 0; i < 4; ++i)
    {
        if (m_modifiedConfig.screenWidth == RESOLUTION_WIDTHS[i] && m_modifiedConfig.screenHeight == RESOLUTION_HEIGHTS[i])
        {
            m_resolutionDropdownActive = i;
            break;
        }
    }
}
void OptionsScreen::OnExit()
{
    if (screenManager)
    {
        screenManager->ApplySettings(m_currentConfig);
    }
}
void OptionsScreen::FixedUpdate(float fixedDeltaTime) {}

void OptionsScreen::Update(float deltaTime)
{
    m_nextScreenState = static_cast<int>(ScreenStateID::NONE);

    if (IsKeyPressed(KEY_ESCAPE))
    {
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }
    m_modifiedConfig.fullScreen = m_fullscreenToggle;
    m_modifiedConfig.screenWidth = RESOLUTION_WIDTHS[m_resolutionDropdownActive];
    m_modifiedConfig.screenHeight = RESOLUTION_HEIGHTS[m_resolutionDropdownActive];
}


// --- 字体大小 ---
#define FONT_SIZE_TITLE    50  // 标题 "OPTIONS" 的字体大小
#define FONT_SIZE_LABEL    24  // "Resolution:" 等标签的字体大小
#define FONT_SIZE_UI       20  // 所有RayGUI控件（按钮、下拉框）的内部字体大小

// --- 布局尺寸 ---
#define OPTIONS_PANEL_WIDTH         700  // 面板加宽
#define OPTIONS_PANEL_HEIGHT        350  // 面板加高
#define OPTIONS_ITEM_HEIGHT         40   // 每个设置项的高度增加
#define OPTIONS_ITEM_SPACING        30   // 设置项之间的垂直间距增加
#define OPTIONS_PADDING             30   // 面板的内边距增加

void OptionsScreen::Draw()
{
    ClearBackground(BLACK);

    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE_UI);

    const char* title = "OPTIONS";
    int titleWidth = MeasureText(title, FONT_SIZE_TITLE);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, 70, FONT_SIZE_TITLE, RAYWHITE);

    Rectangle panelRect = {
        (GetScreenWidth() - OPTIONS_PANEL_WIDTH) / 2.0f,
        (GetScreenHeight() - OPTIONS_PANEL_HEIGHT) / 2.0f - 50,
        OPTIONS_PANEL_WIDTH,
        OPTIONS_PANEL_HEIGHT
    };
    DrawRectangleRounded(panelRect, 0.05f, 6, Fade(DARKGRAY, 0.8f));
    DrawRectangleRoundedLines(panelRect, 0.05f, 6, LIGHTGRAY);

    if (m_resolutionDropdown)
        GuiDisable();

    float currentY = panelRect.y + OPTIONS_PADDING;
    float labelStartX = panelRect.x + OPTIONS_PADDING;
    float labelWidth = 180;
    float controlStartX = labelStartX + labelWidth + 20;
    float controlWidth = panelRect.width - (OPTIONS_PADDING * 2) - labelWidth - 20;

#if !defined(PLATFORM_WEB)
    DrawText(TextFormat("%*s", (int)(labelWidth / 8), "Resolution:"), labelStartX, currentY + 8, FONT_SIZE_LABEL, RAYWHITE);
    currentY += OPTIONS_ITEM_HEIGHT + OPTIONS_ITEM_SPACING;
#endif

    DrawText(TextFormat("%*s", (int)(labelWidth / 8), "Fullscreen:"), labelStartX, currentY + 8, FONT_SIZE_LABEL, RAYWHITE);
    GuiCheckBox({ controlStartX, currentY + (OPTIONS_ITEM_HEIGHT - 20) / 2.0f, 20, 20 }, "", &m_fullscreenToggle);
    currentY += OPTIONS_ITEM_HEIGHT + OPTIONS_ITEM_SPACING;
    
    DrawText(TextFormat("%*s", (int)(labelWidth / 8), "Target FPS:"), labelStartX, currentY + 8, FONT_SIZE_LABEL, RAYWHITE);
    float sliderWidth = controlWidth - 100;
    GuiSlider(
        { controlStartX, currentY + (OPTIONS_ITEM_HEIGHT - 20) / 2.0f, sliderWidth, 20 },
        "", "", &m_modifiedConfig.targetFPS, 30, 240
    );
    DrawText(TextFormat("%d FPS", (int)m_modifiedConfig.targetFPS), controlStartX + sliderWidth + 15, currentY + 8, FONT_SIZE_LABEL, RAYWHITE);

    
    float buttonY = panelRect.y + panelRect.height + 40.0f;
    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;  
    float buttonSpacing = 20.0f;
    float buttonStartX = (GetScreenWidth() - (buttonWidth * 3 + buttonSpacing * 2)) / 2;

    if (GuiButton({ buttonStartX, buttonY, buttonWidth, buttonHeight }, "Apply")) {
        if (screenManager) screenManager->ApplySettings(m_modifiedConfig);
    }
    if (GuiButton({ buttonStartX + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight }, "Save & Return")) {
        if (screenManager) screenManager->ApplySettings(m_modifiedConfig);
        SaveConfig();
        m_currentConfig = m_modifiedConfig;
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }
    if (GuiButton({ buttonStartX + (buttonWidth + buttonSpacing) * 2, buttonY, buttonWidth, buttonHeight }, "Cancel")) {
        m_nextScreenState = static_cast<int>(ScreenStateID::MAIN_MENU);
    }

    if (m_resolutionDropdown)
        GuiEnable();
    
#if !defined(PLATFORM_WEB)
    float resolutionY = panelRect.y + OPTIONS_PADDING;
    if (GuiDropdownBox(
            { controlStartX, resolutionY, controlWidth, OPTIONS_ITEM_HEIGHT },
            m_resolutionOptionsText,
            &m_resolutionDropdownActive,
            m_resolutionDropdown
        ))
    {
        m_resolutionDropdown = !m_resolutionDropdown;
    }
#endif
}

int OptionsScreen::GetNextScreenState() const
{
    return m_nextScreenState;
}

int OptionsScreen::GetScreenState() const
{
    return static_cast<int>(ScreenStateID::OPTIONS);
}

#include <fstream>
void OptionsScreen::SaveConfig()
{
    json configJson;
    m_modifiedConfig.toJson(configJson);
    
    std::ofstream o("assets/config/engine_config.json");
    if (o.is_open()) {
        o << configJson.dump(4);
        o.close();
        printf("[OptionsScreen] Config saved to engine_config.json\n");
    }
    else {
        printf("[OptionsScreen] Failed to save config to engine_config.json\n");
    }
}