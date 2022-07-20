#include "../menu.hpp"

const char* antiAimTypes[] = {"None", "Static", "Jitter", "Fake Jitter", "Real Jitter", "SpingBot (p100)", "Legit"};

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
                ImGui::Text("FOV (x10)");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::SliderInt("##FOV", &CONFIGINT("Rage>RageBot>Default>FOV"), 0, 1800);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Default>Body Scale"), 1, 100);
                ImGui::Checkbox("Auto Slow", &CONFIGBOOL("Rage>RageBot>Default>Auto Slow"));
                ImGui::SameLine();
                ImGui::TextDisabled("?");
                if (ImGui::IsItemHovered())
                  ImGui::SetTooltip("May cause untrusted, use at own risk!");
                ImGui::Checkbox("Kill Shot", &CONFIGBOOL("Rage>RageBot>Default>Kill Shot"));

                ImGui::Separator();

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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Pistol>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Heavy Pistol>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Rifle>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>SMG>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Scout>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>AWP>Body Scale"), 1, 100);
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
                ImGui::SliderInt("##Body Scale", &CONFIGINT("Rage>RageBot>Heavy>Body Scale"), 1, 100);
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