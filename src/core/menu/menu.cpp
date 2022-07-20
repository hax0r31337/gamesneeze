#include "menu.hpp"
#include "imgui/imgui.h"

void style() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.24f, 0.24f, 0.24f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.41f, 0.06f, 0.62f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.13f, 0.84f, 0.15f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.12f, 0.80f, 0.06f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.12f, 0.50f, 0.04f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.24f, 0.81f, 0.19f, 0.40f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.05f, 0.95f, 0.04f, 0.40f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.11f, 0.50f, 0.04f, 0.62f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.32f, 0.81f, 0.19f, 0.62f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.75f, 0.15f, 0.62f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_Header]                 = ImVec4(0.05f, 0.41f, 0.06f, 0.62f);



    ImGui::GetStyle().ScrollbarSize = 8;
    ImGui::GetStyle().ScrollbarRounding = 0;
    ImGui::GetStyle().WindowRounding = 6;
    ImGui::GetStyle().ChildRounding = 3;
    ImGui::GetStyle().FrameRounding = 4;
    ImGui::GetStyle().TabRounding = 2;
}

void Menu::onPollEvent(SDL_Event* event, const int result) {
    if (result && ImGui_ImplSDL2_ProcessEvent(event) && Menu::open) {
        event->type = 0;
    }
}

std::vector<char> Menu::loadBinaryFile(const std::string &path) {
  std::vector<char> result;
  std::ifstream in{path, std::ios::binary};
  if (!in)
    return result;
  in.seekg(0, std::ios_base::end);
  result.resize(static_cast<std::size_t>(in.tellg()));
  in.seekg(0, std::ios_base::beg);
  in.read(result.data(), result.size());
  return result;
}

bool Menu::decodeVFONT(std::vector<char> &buffer) {
  constexpr std::string_view tag = "VFONT1";
  unsigned char magic = 0xA7;

  if (buffer.size() <= tag.length())
    return false;

  const auto tagIndex = buffer.size() - tag.length();
  if (std::memcmp(tag.data(), &buffer[tagIndex], tag.length()))
    return false;

  unsigned char saltBytes = buffer[tagIndex - 1];
  const auto saltIndex = tagIndex - saltBytes;
  --saltBytes;

  for (std::size_t i = 0; i < saltBytes; ++i)
    magic ^= (buffer[saltIndex + i] + 0xA7) % 0x100;

  for (std::size_t i = 0; i < saltIndex; ++i) {
    unsigned char xored = buffer[i] ^ magic;
    magic = (buffer[i] + 0xA7) % 0x100;
    buffer[i] = xored;
  }

  buffer.resize(saltIndex);
  return true;
}

ImWchar *Menu::getFontGlyphRanges() {
  static ImVector<ImWchar> ranges;
  if (ranges.empty()) {
    ImFontGlyphRangesBuilder builder;
    constexpr ImWchar baseRanges[]{
        0x0100, 0x024F, // Latin Extended-A + Latin Extended-B
        0x0300, 0x03FF, // Combining Diacritical Marks + Greek/Coptic
        0x0600, 0x06FF, // Arabic
        0x0E00, 0x0E7F, // Thai
        0};
    builder.AddRanges(baseRanges);
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    builder.AddRanges(
        ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
    builder.AddText("\u9F8D\u738B\u2122");
    builder.BuildRanges(&ranges);
  }
  return ranges.Data;
}

ImFont *Menu::addFontFromVFONT(const std::string &path, float size,
                         const ImWchar *glyphRanges, bool merge) {
  auto file = loadBinaryFile(path);
  if (!decodeVFONT(file))
    return nullptr;

  ImFontConfig cfg;
  cfg.FontData = file.data();
  cfg.FontDataSize = file.size();
  cfg.FontDataOwnedByAtlas = false;
  cfg.MergeMode = merge;
  cfg.GlyphRanges = glyphRanges;
  cfg.SizePixels = size;

  return ImGui::GetIO().Fonts->AddFont(&cfg);
}

void Menu::onSwapWindow(SDL_Window* window) {
    if (!initialised) {
        gl3wInit();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init("#version 100");
        ImGui_ImplSDL2_InitForOpenGL(window, nullptr);
        style();

        ImFont *normalFont = addFontFromVFONT("csgo/panorama/fonts/notosans-bold.vfont", 15.0f, getFontGlyphRanges(), false);
        if (!normalFont) {
          ImFontConfig cfg;
          cfg.SizePixels = 14.0f;
          ImGui::GetIO().Fonts->AddFontDefault(&cfg);
        }
        addFontFromVFONT("csgo/panorama/fonts/notosanskr-bold.vfont", 15.0f, ImGui::GetIO().Fonts->GetGlyphRangesKorean(), true);
        addFontFromVFONT("csgo/panorama/fonts/notosanssc-bold.vfont", 17.0f, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull(), true);
        addFontFromVFONT("csgo/panorama/fonts/notosansjp-bold.vfont", 15.0f, ImGui::GetIO().Fonts->GetGlyphRangesJapanese(), true);

        initialised = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    ImGuiIO& io = ImGui::GetIO();
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    Globals::screenSizeX = w;
    Globals::screenSizeY = h;
    io.DisplaySize = ImVec2((float)w, (float)h);

    ImGui::NewFrame();

    Menu::drawOverlay(ImGui::GetBackgroundDrawList());
    if (Menu::open) {
        io.MouseDrawCursor = true; // TODO: find workaround at some point because ugly
        Menu::drawMenu();
        if (devWindow) {
            drawDevWindow();
        }
    } else {
        io.MouseDrawCursor = false;
    }
    if (ImGui::IsKeyPressed(SDL_SCANCODE_INSERT, false) ||
        (ImGui::IsKeyPressed(SDL_SCANCODE_ESCAPE, false) && Menu::open)) {
      Config::reloadCfgList();
      Menu::open = !Menu::open;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Menu::drawMenu() {
    ImGui::SetNextWindowSize(ImVec2{900, 627});
    ImGui::Begin("gamesneeze-linux", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
    ImGui::Text("game"); ImGui::SameLine(); ImGui::PopStyleVar();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(410, 5));
    ImGui::TextColored(ImColor(108, 195, 18, 255), "sneeze");
    ImGui::SameLine();
    ImGui::PopStyleVar();

    auto bWidth = ImVec2(80, 20);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 5));

    ImGui::GetStyle().Colors[ImGuiCol_Button] = (Menu::tabSelected == 0) ? ImVec4(0.05f, 0.41f, 0.06f, 0.62f) : ImVec4(0.03f, 0.23f, 0.04f, 0.62f);
    if (ImGui::Button("Legit", bWidth)) {
        Menu::tabSelected = 0;
    } ImGui::SameLine();

    ImGui::GetStyle().Colors[ImGuiCol_Button] = (Menu::tabSelected == 1) ? ImVec4(0.05f, 0.41f, 0.06f, 0.62f) : ImVec4(0.03f, 0.23f, 0.04f, 0.62f);
    if (ImGui::Button("Rage", bWidth)) {
      Menu::tabSelected = 1;
    } ImGui::SameLine();

    ImGui::GetStyle().Colors[ImGuiCol_Button] = (Menu::tabSelected == 2) ? ImVec4(0.05f, 0.41f, 0.06f, 0.62f) : ImVec4(0.03f, 0.23f, 0.04f, 0.62f);
    if (ImGui::Button("Visuals", bWidth)) {
      Menu::tabSelected = 2;
    } ImGui::SameLine();

    ImGui::GetStyle().Colors[ImGuiCol_Button] = (Menu::tabSelected == 3) ? ImVec4(0.05f, 0.41f, 0.06f, 0.62f) : ImVec4(0.03f, 0.23f, 0.04f, 0.62f);
    if (ImGui::Button("Misc", bWidth)) {
      Menu::tabSelected = 3;
    } ImGui::SameLine();

    ImGui::GetStyle().Colors[ImGuiCol_Button] = (Menu::tabSelected == 4) ? ImVec4(0.05f, 0.41f, 0.06f, 0.62f) : ImVec4(0.03f, 0.23f, 0.04f, 0.62f);
    if (ImGui::Button("Lua", bWidth)) {
      Menu::tabSelected = 4;
    }

    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.05f, 0.41f, 0.06f, 0.62f);
    ImGui::PopStyleVar();

    ImGui::Separator();
    // ImGui::Separator();

    switch(tabSelected) {
        case 0: {
            Menu::drawLegitTab(); break;
        }
        case 1: {
            Menu::drawRageTab(); break;
        }
        case 2: {
            Menu::drawVisualsTab(); break;
        }
        case 3: {
            Menu::drawMiscTab();break;
        }
        case 4: {
            Menu::drawLuaTab();break;
        }
    }

    ImGui::End();
}