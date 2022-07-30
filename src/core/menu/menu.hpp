#pragma once
#include "../../includes.hpp"
#include "imgui/imgui.h"
#include <cstdint>
#include <filesystem>
#include <string>

namespace Menu {
    inline bool open = true;
    inline bool initialised = false;
    inline int tabSelected = 0;

    inline bool devWindow = false;

    inline char clantag[128] = "gamesneeze";
    inline char playerName[128] = "NachoNeko";
    void drawMenu();
    void drawLegitTab();
    void drawRageTab();
    void drawVisualsTab();
    void drawMiscTab();
    void drawLuaTab();
    void drawDevWindow();
    void drawOverlay(ImDrawList* drawList);
    void onPollEvent(SDL_Event* event, const int result);
    void onSwapWindow(SDL_Window *window);

    void drawWaterMarkOverlay();
    void drawBombTimerOverlay();

    std::vector<char> loadBinaryFile(const std::string &path);
    bool decodeVFONT(std::vector<char> &buffer);
    ImWchar *getFontGlyphRanges();
    ImFont *addFontFromVFONT(const std::string &path, float size,
                         const ImWchar *glyphRanges, bool merge);

    void hitboxSelectBox(const char *configVarName, bool hasText = true, const char *label = "##HitBoxes");
    namespace CustomWidgets {
        void drawKeyBinder(const char* label, int* key, bool* toggled);
        bool isKeyDown(int key);
    }
}