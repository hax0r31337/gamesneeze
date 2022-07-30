#include "../menu.hpp"

const char* antiAimTypes[] = {"Manual", "Peek real", "Peek desync", "Jitter"};

void Menu::hitboxSelectBox(const char *configVarName, bool hasText, const char *label) {
  if (hasText) {
    ImGui::Text("Hitboxes");
  }
  ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());

  int curSelected = CONFIGINT(configVarName);

  std::stringstream selectedHitboxes;
  selectedHitboxes << (curSelected & (int)HitBoxes::HEAD ? "Head, " : "")
                   << (curSelected & (int)HitBoxes::NECK ? "Neck, " : "")
                   << (curSelected & (int)HitBoxes::CHEST ? "Chest, " : "")
                   << (curSelected & (int)HitBoxes::STOMACH ? "Stomach, " : "")
                   << (curSelected & (int)HitBoxes::PELVIS ? "Pelvis, " : "");

  if (ImGui::BeginCombo(label, selectedHitboxes.str().c_str())) {

    if (ImGui::Selectable("Head", curSelected & (int)HitBoxes::HEAD,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::HEAD;

    if (ImGui::Selectable("Neck", curSelected & (int)HitBoxes::NECK,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::NECK;

    if (ImGui::Selectable("Chest", curSelected & (int)HitBoxes::CHEST,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::CHEST;

    if (ImGui::Selectable("Stomach", curSelected & (int)HitBoxes::STOMACH,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::STOMACH;

    if (ImGui::Selectable("Pelvis", curSelected & (int)HitBoxes::PELVIS,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::PELVIS;

    ImGui::EndCombo();
  }
}

void Menu::drawRageTab() {
    // ImGui::Checkbox("Enabled", &CONFIGBOOL("Rage>Enabled"));
    // ImGui::SameLine();
    // ImGui::Separator();

    ImGui::BeginChild("Rage", ImVec2((ImGui::GetWindowContentRegionWidth()/2) - 4, 575), true); {
        ImGui::Text("RageBot");
        ImGui::Separator();
        if (ImGui::BeginTabBar("Weapons Tabbar")) {
            if (ImGui::BeginTabItem("Default")) {
                static bool keybindToggled;
                Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Rage>RageBot>Key"), &keybindToggled);
                ImGui::SameLine();
                ImGui::Checkbox("Always On", &CONFIGBOOL("Rage>RageBot>Always on"));

                ImGui::Separator();

                hitboxSelectBox("Rage>RageBot>Default>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Default>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Default>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Default>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Default>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Default>Auto Slow"));
                ImGui::SameLine();
                ImGui::TextDisabled("?");
                if (ImGui::IsItemHovered())
                  ImGui::SetTooltip("May cause untrusted, use at own risk!");
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Default>Kill Shot"));

                ImGui::Separator();

                ImGui::Checkbox("Force Safe Points", &CONFIGBOOL("Rage>RageBot>Default>Force Safe Point"));
                if (CONFIGBOOL("Rage>RageBot>Default>Force Safe Point")) {
                  ImGui::SameLine();
                  static bool toggled = false;
                  Menu::CustomWidgets::drawKeyBinder(
                      "FSP_Key", &CONFIGINT("Rage>RageBot>Default>Force Safe Point Key"), &toggled);
                  hitboxSelectBox("Rage>RageBot>Default>Force Safe Point Flags", false, "##FSP_HitBoxes");
                }

                ImGui::Checkbox("Min Damage Overwrite", &CONFIGBOOL("Rage>RageBot>Default>Min Damage Overwrite"));
                if (CONFIGBOOL("Rage>RageBot>Default>Min Damage Overwrite")) {
                  ImGui::SameLine();
                  static bool toggled = false;
                  Menu::CustomWidgets::drawKeyBinder(
                      "MDO_Key", &CONFIGINT("Rage>RageBot>Default>Min Damage Overwrite Key"), &toggled);
                  ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                  ImGui::SliderInt("##Min Damage Overwrite", &CONFIGINT("Rage>RageBot>Default>Min Damage Overwrite Value"), 1, 100);
                }
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Default>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Default>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Default>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Default>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Default>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Default>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Default>Visible Only"));

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Pistol")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Pistol>Override"));
                hitboxSelectBox("Rage>RageBot>Pistol>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Pistol>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Pistol>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Pistol>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Pistol>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Pistol>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Pistol>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Heavy Pistol")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Override"));
                hitboxSelectBox("Rage>RageBot>Heavy Pistol>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Heavy Pistol>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Heavy Pistol>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Heavy Pistol>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Heavy Pistol>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Rifle")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Rifle>Override"));
                hitboxSelectBox("Rage>RageBot>Rifle>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Rifle>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Rifle>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Rifle>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Rifle>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Rifle>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Rifle>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("SMG")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>SMG>Override"));
                hitboxSelectBox("Rage>RageBot>SMG>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>SMG>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>SMG>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>SMG>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>SMG>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>SMG>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>SMG>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Scout")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Scout>Override"));
                hitboxSelectBox("Rage>RageBot>Scout>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Scout>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Scout>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Scout>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Scout>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Scout>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Scout>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("AWP")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>AWP>Override"));
                hitboxSelectBox("Rage>RageBot>AWP>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>AWP>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>AWP>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>AWP>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>AWP>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>AWP>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>AWP>Kill Shot"));
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Heavy")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Heavy>Override"));
                hitboxSelectBox("Rage>RageBot>Heavy>Hitboxes");
                ImGui::Text("Hit Chance");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Hit Chance", &CONFIGINT("Rage>RageBot>Heavy>Hit Chance"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Heavy>Min Damage"), 1, 100);
                ImGui::Text("Head Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Head Scale", &CONFIGINT("Rage>RageBot>Heavy>Head Scale"), 0, 100);
                ImGui::Text("Body Scale");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Heavy>Body Scale"), 0, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Heavy>Auto Slow"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Heavy>Kill Shot"));
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::BeginChild("Others", ImVec2((ImGui::GetWindowContentRegionWidth()/2) - 4, 575), true); {
        ImGui::Text("Anti Aim");
        ImGui::Separator();

        ImGui::Checkbox("Enabled", &CONFIGBOOL("Rage>AntiAim>Enabled"));
        ImGui::Text("Desync");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::SliderInt("##Desync", &CONFIGINT("Rage>AntiAim>Desync"), 0, 180);
        ImGui::Text("Yaw");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::SliderInt("##Yaw", &CONFIGINT("Rage>AntiAim>Yaw"), 0, 180);
        ImGui::Text("Pitch");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::SliderInt("##Pitch", &CONFIGINT("Rage>AntiAim>Pitch"), -89, 180);
        ImGui::Text("Roll");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::SliderInt("##Roll", &CONFIGINT("Rage>AntiAim>Roll"), -89, 89);
        ImGui::Checkbox("At Target", &CONFIGBOOL("Rage>AntiAim>At Target"));
        static bool toggled = false;
        Menu::CustomWidgets::drawKeyBinder("Invert Key", &CONFIGINT("Rage>AntiAim>Invert Key"),&toggled);
        ImGui::Text("Desync Mode");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::Combo("##Desync Mode", &CONFIGINT("Rage>AntiAim>Desync Mode"), antiAimTypes, IM_ARRAYSIZE(antiAimTypes));


        ImGui::Text("Slow Walk");
        ImGui::Separator();

        ImGui::Checkbox("Slow Walk",&CONFIGBOOL("Rage>Slow Walk"));
        ImGui::SliderInt("Slow Walk Speed (%)", &CONFIGINT("Rage>Slow Walk Speed"), 0, 255);

        ImGui::Text("Fake Lag");
        ImGui::Separator();

        ImGui::Checkbox("Fake Lag", &CONFIGBOOL("Rage>Fake Lag>Enabled"));
        ImGui::Checkbox("Choke On Shot", &CONFIGBOOL("Rage>Fake Lag>Choke On Shot"));
        ImGui::Checkbox("Adaptive", &CONFIGBOOL("Rage>Fake Lag>Adaptive"));
        ImGui::Text("Amount");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::SliderInt("##Amount", &CONFIGINT("Rage>Fake Lag>Amount"), 0, 10);

        ImGui::Text("Others");
        ImGui::Separator();

        ImGui::Checkbox("Quick Peek", &CONFIGBOOL("Rage>Quick Peek>Enabled"));
        if (CONFIGBOOL("Rage>Quick Peek>Enabled")) {
          ImGui::SameLine();
          static bool toggled = false;
          Menu::CustomWidgets::drawKeyBinder(
              "Key", &CONFIGINT("Rage>Quick Peek>Key"), &toggled);
        }
        ImGui::Checkbox("Resolver", &CONFIGBOOL("Rage>Resolver"));

        ImGui::EndChild();
    }
}