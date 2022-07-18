#include "../../includes.hpp"
#include "menu.hpp"
#include <cstring>
#include <unistd.h>
#include <pwd.h>

void Menu::drawOverlay(ImDrawList* drawList) {
    Globals::drawList = drawList;

    drawWaterMarkOverlay();

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
      (Interfaces::engine->IsInGame() && playerResource)
          ? playerResource->GetPing(Interfaces::engine->GetLocalPlayer())
          : 0);
  ImGui::PopStyleVar();
  ImGui::End();
}

// void Menu::drawBombTimerOverlay() {
//   if (!CONFIGBOOL("Visuals>World>Items>Bomb Timer")) {
//     return;
//   }

//   GameData::Lock lock;

//   const auto &plantedC4 = GameData::plantedC4();
//   if (plantedC4.blowTime == 0.0f && !Menu::open)
//     return;

//   if (!Menu::open) {
//     ImGui::SetNextWindowBgAlpha(0.3f);
//   }

//   static float windowWidth = 200.0f;
//   ImGui::SetNextWindowPos(
//       {(ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 60.0f}, ImGuiCond_Once);
//   ImGui::SetNextWindowSize({windowWidth, 0}, ImGuiCond_Once);

//   if (!Menu::open)
//     ImGui::SetNextWindowSize({windowWidth, 0});

//   ImGui::SetNextWindowSizeConstraints({0, -1}, {FLT_MAX, -1});
//   ImGui::Begin("Bomb Timer", nullptr,
//                ImGuiWindowFlags_NoTitleBar |
//                    (Menu::open ? 0
//                                   : ImGuiWindowFlags_NoInputs |
//                                         ImGuiWindowFlags_NoDecoration));

//   std::ostringstream ss;
//   ss << "Bomb on " << (!plantedC4.bombsite ? 'A' : 'B') << " : " << std::fixed
//      << std::showpoint << std::setprecision(3)
//      << (std::max)(plantedC4.blowTime - Interfaces::globals->curtime, 0.0f)
//      << " s";

//   ImGui::TextC(ss.str().c_str());

//   ImGui::PushStyleColor(
//       ImGuiCol_PlotHistogram,
//       Helpers::calculateColor(miscConfig.bombTimer.asColor3()));
//   ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.2f, 0.2f, 0.2f, 1.0f});
//   ImGui::progressBarFullWidth(
//       (plantedC4.blowTime - memory->globalVars->currenttime) /
//           plantedC4.timerLength,
//       5.0f);

//   if (plantedC4.defuserHandle != -1) {
//     const bool canDefuse = plantedC4.blowTime >= plantedC4.defuseCountDown;

//     if (plantedC4.defuserHandle == GameData::local().handle) {
//       if (canDefuse) {
//         ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
//         ImGui::textUnformattedCentered("You can defuse!");
//       } else {
//         ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
//         ImGui::textUnformattedCentered("You can not defuse!");
//       }
//       ImGui::PopStyleColor();
//     } else if (const auto defusingPlayer =
//                    GameData::playerByHandle(plantedC4.defuserHandle)) {
//       std::ostringstream ss;
//       ss << defusingPlayer->name << " is defusing: " << std::fixed
//          << std::showpoint << std::setprecision(3)
//          << (std::max)(plantedC4.defuseCountDown -
//                            memory->globalVars->currenttime,
//                        0.0f)
//          << " s";

//       ImGui::textUnformattedCentered(ss.str().c_str());

//       ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
//                             canDefuse ? IM_COL32(0, 255, 0, 255)
//                                       : IM_COL32(255, 0, 0, 255));
//       ImGui::progressBarFullWidth(
//           (plantedC4.defuseCountDown - memory->globalVars->currenttime) /
//               plantedC4.defuseLength,
//           5.0f);
//       ImGui::PopStyleColor();
//     }
//   }

//   windowWidth = ImGui::GetCurrentWindow()->SizeFull.x;

//   ImGui::PopStyleColor(2);
//   ImGui::End();
// }
