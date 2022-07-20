#include "../../includes.hpp"
#include "imgui/imgui.h"
#include "menu.hpp"
#include <cstring>
#include <unistd.h>
#include <pwd.h>

void BombData::update() noexcept {
  if (Interfaces::engine->IsInGame()) {
    for (auto i : entityDistanceMap) {
      if (!Globals::localPlayer ||
          i.second == Interfaces::engine->GetLocalPlayer()) {
        continue;
      }
      Entity *ent = (Entity *)Interfaces::entityList->GetClientEntity(i.second);
      if (!ent) {
        continue;
      }
      ClientClass *clientClass = ent->clientClass();

      if (clientClass->m_ClassID == EClassIds::CPlantedC4) {
        auto bomb = (PlantedC4 *)ent;
        blowTime = bomb->time();
        timerLength = bomb->timerLength();
        defuserHandle = bomb->defuser();
        if (defuserHandle != -1) {
          defuseCountDown = bomb->defuseCountDown();
          defuseLength = bomb->defuseLength();
        }
        bombsite = bomb->site();
        return;
      }
    }
  }
  blowTime = 0.0f;
  timerLength = 1.f;
  defuserHandle = -1;
  bombsite = false;
}

void Menu::drawOverlay(ImDrawList* drawList) {
    Globals::drawList = drawList;

    drawWaterMarkOverlay();
    drawBombTimerOverlay();

    Features::ESP::draw();
    Features::RecoilCrosshair::draw();
    Features::Spectators::draw();
    Features::PlayerList::draw();
    Features::FlappyBird::draw();
    Features::Notifications::draw();
    Features::Hitmarkers::draw();

    Features::Movement::draw();
}

void Menu::drawWaterMarkOverlay() {
  if (CONFIGBOOL("Misc>Misc>Misc>Disable Watermark")) {
    return;
  }

  ImGuiWindowFlags windowFlags =
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
  if (!Menu::open)
    windowFlags |= ImGuiWindowFlags_NoInputs;

  ImGui::SetNextWindowBgAlpha(0.3f);
  ImGui::Begin("Watermark", nullptr, windowFlags);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
  ImGui::Text("game");
  ImGui::SameLine();
  ImGui::TextColored(ImColor(108, 195, 18, 255), "sneeze");
  ImGui::SameLine();
  ImGui::Text(
      " | %.1f FPS | %i ms", ImGui::GetIO().Framerate,
      Interfaces::engine->IsInGame() ? playerResource->GetPing(Interfaces::engine->GetLocalPlayer()) : 0);
  ImGui::PopStyleVar();
  ImGui::End();
}

void Menu::drawBombTimerOverlay() {
  if (!CONFIGBOOL("Visuals>World>Items>Bomb Timer")) {
    return;
  }

  const auto &plantedC4 = Menu::bombData;
  if (plantedC4.blowTime == 0.0f && !Menu::open)
    return;

  ImGui::SetNextWindowBgAlpha(0.3f);

  static float windowWidth = 200.0f;
  ImGui::SetNextWindowPos({(ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 60.0f}, ImGuiCond_Once);
  ImGui::SetNextWindowSize({windowWidth, 0}, ImGuiCond_Once);

  if (!Menu::open)
    ImGui::SetNextWindowSize({windowWidth, 0});

  ImGui::SetNextWindowSizeConstraints({0, -1}, {FLT_MAX, -1});
  ImGui::Begin("Bomb Timer", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   (Menu::open ? 0
                                  : ImGuiWindowFlags_NoInputs |
                                        ImGuiWindowFlags_NoDecoration));

  std::ostringstream ss;
  ss << "Bomb on site " << (plantedC4.bombsite == 0 ? 'A' : 'B') << " : " << std::fixed
     << std::showpoint << std::setprecision(3)
     << (std::max)(plantedC4.blowTime - Interfaces::globals->curtime, 0.0f)
     << " s";

  ImGui::Text("%s", ss.str().c_str());

  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(108, 195, 18, 255));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
  ImGui::ProgressBar((plantedC4.blowTime - Interfaces::globals->curtime) /
                         plantedC4.timerLength,
                     ImVec2(windowWidth - 20, 5.0f));

  if (plantedC4.defuserHandle != -1) {
    const bool canDefuse = plantedC4.blowTime >= plantedC4.defuseCountDown;

    std::ostringstream ss;
    ss << "Defusing  " << std::fixed << std::showpoint << std::setprecision(3)
       << (std::max)(plantedC4.defuseCountDown - Interfaces::globals->curtime,
                     0.0f)
       << " s";

    ImGui::Text("%s", ss.str().c_str());

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                          canDefuse ? IM_COL32(255, 255, 0, 255)
                                    : IM_COL32(255, 0, 0, 255));
    ImGui::ProgressBar(
        (plantedC4.defuseCountDown - Interfaces::globals->curtime) /
            plantedC4.defuseLength,
        ImVec2(windowWidth - 20, 5.0f));
    ImGui::PopStyleColor();
  }

  ImGui::PopStyleColor(2);
  ImGui::End();
}
