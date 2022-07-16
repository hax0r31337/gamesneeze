#include "../menu.hpp"

const char* antiAimTypes[] = {"None", "Static", "Jitter", "Fake Jitter", "Real Jitter", "SpingBot (p100)"};

void Menu::hitboxSelectBox(const char *configVarName) {
  ImGui::Text("Hitboxes");
  ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());

  int curSelected = CONFIGINT(configVarName);

  std::stringstream selectedHitboxes;
  selectedHitboxes << (curSelected & (int)HitBoxes::HEAD ? "Head, " : "")
                   << (curSelected & (int)HitBoxes::NECK ? "Neck, " : "")
                   << (curSelected & (int)HitBoxes::CHEST ? "Chest, " : "")
                   << (curSelected & (int)HitBoxes::STOMACH ? "Stomach, " : "")
                   << (curSelected & (int)HitBoxes::PELVIS ? "Pelvis, " : "");

  if (ImGui::BeginCombo("##HitBoxes", selectedHitboxes.str().c_str())) {

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
    ImGui::Checkbox("Enabled", &CONFIGBOOL("Rage>Enabled"));
    ImGui::SameLine();
    ImGui::Separator();

    ImGui::BeginChild("Rage", ImVec2((ImGui::GetWindowContentRegionWidth()/2) - 4, 520), true); {
        ImGui::Text("RageBot");
        ImGui::Separator();
        if (ImGui::BeginTabBar("Weapons Tabbar")) {
            if (ImGui::BeginTabItem("Default")) {
                static bool keybindToggled;
                Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Rage>RageBot>Key"), &keybindToggled);
                ImGui::SameLine();
                ImGui::Checkbox("Always On", &CONFIGBOOL("Rage>RageBot>Always on"));

                hitboxSelectBox("Rage>RageBot>Default>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Default>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Default>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Default>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Default>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Default>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Default>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Default>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Default>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Default>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Default>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Default>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Default>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Default>Kill Shot"));

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Pistol")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Pistol>Override"));
                hitboxSelectBox("Rage>RageBot>Pistol>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Pistol>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Pistol>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Pistol>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Pistol>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Pistol>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Pistol>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Pistol>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Pistol>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Pistol>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Pistol>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Pistol>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Pistol>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Pistol>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Heavy Pistol")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Override"));
                hitboxSelectBox("Rage>RageBot>Heavy Pistol>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Heavy Pistol>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Heavy Pistol>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Heavy Pistol>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Heavy Pistol>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Heavy Pistol>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Heavy Pistol>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Rifle")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Rifle>Override"));
                hitboxSelectBox("Rage>RageBot>Rifle>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Rifle>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Rifle>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Rifle>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Rifle>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Rifle>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Rifle>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Rifle>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Rifle>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Rifle>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Rifle>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Rifle>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Rifle>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Rifle>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("SMG")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>SMG>Override"));
                hitboxSelectBox("Rage>RageBot>SMG>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>SMG>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>SMG>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>SMG>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>SMG>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>SMG>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>SMG>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>SMG>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>SMG>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>SMG>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>SMG>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>SMG>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>SMG>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>SMG>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Scout")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Scout>Override"));
                hitboxSelectBox("Rage>RageBot>Scout>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Scout>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Scout>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Scout>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Scout>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Scout>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Scout>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Scout>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Scout>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Scout>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Scout>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Scout>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Scout>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Scout>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("AWP")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>AWP>Override"));
                hitboxSelectBox("Rage>RageBot>AWP>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>AWP>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>AWP>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>AWP>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>AWP>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>AWP>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>AWP>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>AWP>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>AWP>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>AWP>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>AWP>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>AWP>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>AWP>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>AWP>Kill Shot"));
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Heavy")) {
                ImGui::Checkbox("Override", &CONFIGBOOL("Rage>RageBot>Heavy>Override"));
                hitboxSelectBox("Rage>RageBot>Heavy>Hitboxes");
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Heavy>FOV"), 0, 1800);
                ImGui::Text("Smoothing");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Smoothing", &CONFIGINT("Rage>RageBot>Heavy>Smoothing"), 1, 100);
                ImGui::Text("Max Shot Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Shot Inaccuracy", &CONFIGINT("Rage>RageBot>Heavy>Max Shot Inaccuracy"), 0, 100);
                ImGui::Text("Max Aim Inaccuracy");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Max Aim Inaccuracy", &CONFIGINT("Rage>RageBot>Heavy>Max Aim Inaccuracy"), 0, 100);
                ImGui::Text("Min Damage");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Min Damage", &CONFIGINT("Rage>RageBot>Heavy>Min Damage"), 1, 100);
                ImGui::Checkbox("Friendly Fire", &CONFIGBOOL("Rage>RageBot>Heavy>Friendly Fire"));
                ImGui::Checkbox("Ignore Blind", &CONFIGBOOL("Rage>RageBot>Heavy>Ignore Blind"));
                ImGui::Checkbox("Ignore Smoke", &CONFIGBOOL("Rage>RageBot>Heavy>Ignore Smoke"));
                ImGui::Checkbox("Scoped Only", &CONFIGBOOL("Rage>RageBot>Heavy>Scoped Only"));
                ImGui::Checkbox("Auto Shot", &CONFIGBOOL("Rage>RageBot>Heavy>Auto Shot"));
                ImGui::Checkbox("Auto Scope", &CONFIGBOOL("Rage>RageBot>Heavy>Auto Scope"));
                ImGui::Checkbox("Visible Only", &CONFIGBOOL("Rage>RageBot>Heavy>Visible Only"));
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Heavy>Kill Shot"));
              ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::BeginChild("Others", ImVec2((ImGui::GetWindowContentRegionWidth()/2) - 4, 520), true); {
        ImGui::Text("Anti-Aim");
        ImGui::Separator();
        
        ImGui::Text("Type");
        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
        ImGui::Combo("##Type", &CONFIGINT("Rage>AntiAim>Type"), antiAimTypes, IM_ARRAYSIZE(antiAimTypes));

        if (CONFIGINT("Rage>AntiAim>Type")) {
            ImGui::Text("Pitch");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Pitch", &CONFIGINT("Rage>AntiAim>Pitch"), -89, 89);

            ImGui::Text("Yaw Offset");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Offset", &CONFIGINT("Rage>AntiAim>Offset"), 0, 360);

            ImGui::Text("FakeLag");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##FakeLag", &CONFIGINT("Rage>AntiAim>FakeLag"), 0, 16);
        }

        if (CONFIGINT("Rage>AntiAim>Type") == 1) { // Static
            ImGui::Text("Desync");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Desync", &CONFIGINT("Rage>AntiAim>Static>Desync"), -60, 60);
        }

        if (CONFIGINT("Rage>AntiAim>Type") == 2) { // Jitter
            ImGui::Text("Desync");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Desync", &CONFIGINT("Rage>AntiAim>Jitter>Desync"), -60, 60);

            ImGui::Text("Jitter Amount");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Amount", &CONFIGINT("Rage>AntiAim>Jitter>Jitter Amount"), 0, 180);

            ImGui::Text("Jitter Delay");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Delay", &CONFIGINT("Rage>AntiAim>Jitter>Jitter Delay"), 1, 256);

            ImGui::Checkbox("Random", &CONFIGBOOL("Rage>AntiAim>Jitter>Random"));
            if (CONFIGBOOL("Rage>AntiAim>Jitter>Random")) {
                ImGui::Text("Random Min");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Min", &CONFIGINT("Rage>AntiAim>Jitter>Random Min"), 0, 180);

                ImGui::Text("Random Max");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Max", &CONFIGINT("Rage>AntiAim>Jitter>Random Max"), 0, 180);
            }
        }

        if (CONFIGINT("Rage>AntiAim>Type") == 3) { // Fake Jitter
            ImGui::Text("Jitter Amount");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Amount", &CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Amount"), 0, 60);

            ImGui::Text("Jitter Delay");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Delay", &CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Delay"), 1, 256);

            ImGui::Checkbox("Random", &CONFIGBOOL("Rage>AntiAim>Fake Jitter>Random"));
            if (CONFIGBOOL("Rage>AntiAim>Fake Jitter>Random")) {
                ImGui::Text("Random Min");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Min", &CONFIGINT("Rage>AntiAim>Fake Jitter>Random Min"), 0, 60);

                ImGui::Text("Random Max");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Max", &CONFIGINT("Rage>AntiAim>Fake Jitter>Random Max"), 0, 60);
            }
        }

        if (CONFIGINT("Rage>AntiAim>Type") == 4) { // Real Jitter
            ImGui::Text("Jitter Amount");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Amount", &CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Amount"), 0, 60);

            ImGui::Text("Jitter Delay");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Jitter Delay", &CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Delay"), 1, 256);

            ImGui::Checkbox("Random", &CONFIGBOOL("Rage>AntiAim>Fake Jitter>Random"));
            if (CONFIGBOOL("Rage>AntiAim>Real Jitter>Random")) {
                ImGui::Text("Random Min");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Min", &CONFIGINT("Rage>AntiAim>Real Jitter>Random Min"), 0, 60);

                ImGui::Text("Random Max");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##Random Max", &CONFIGINT("Rage>AntiAim>Real Jitter>Random Max"), 0, 60);
            }
        }

        if (CONFIGINT("Rage>AntiAim>Type") == 5) { // Sping
            ImGui::Text("Desync");
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::SliderInt("##Desync", &CONFIGINT("Rage>AntiAim>Spin>Desync"), -60, 60);
        }
        ImGui::Checkbox("Slow Walk",&CONFIGBOOL("Rage>AntiAim>Slow Walk"));
        ImGui::SliderInt("Slow Walk Speed (%)", &CONFIGINT("Rage>AntiAim>Slow Walk Speed"), 0, 255);

        ImGui::Text("Others");
        ImGui::Separator();

        ImGui::Checkbox("Quick Peek", &CONFIGBOOL("Rage>Quick Peek>Enabled"));
        ImGui::SameLine();
        if (CONFIGBOOL("Rage>Quick Peek>Enabled")) {
          static bool toggled = false;
          Menu::CustomWidgets::drawKeyBinder(
              "Key", &CONFIGINT("Rage>Quick Peek>Key"), &toggled);
        }
        ImGui::Checkbox("Resolver", &CONFIGBOOL("Rage>Resolver"));
        
        ImGui::EndChild();
    }
}