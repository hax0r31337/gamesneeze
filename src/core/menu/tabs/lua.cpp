#include "../menu.hpp"
#include "../../script/manager.hpp"

void Menu::drawLuaTab() {
  if (ImGui::Button("Test",  ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
    ScriptManager::Execute();
  };
}