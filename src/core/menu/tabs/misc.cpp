#include "../menu.hpp"
#include <filesystem>
#include "../config.hpp"

void reportBotSelectBox(const char *configVarName) {
  ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());

  int curSelected = CONFIGINT(configVarName);

  std::stringstream selectedHitboxes;
  selectedHitboxes << (curSelected & (int)HitBoxes::HEAD ? "textabuse, " : "")
                   << (curSelected & (int)HitBoxes::NECK ? "grief, " : "")
                   << (curSelected & (int)HitBoxes::CHEST ? "wallhack, " : "")
                   << (curSelected & (int)HitBoxes::STOMACH ? "aimbot, " : "")
                   << (curSelected & (int)HitBoxes::PELVIS ? "speedhack, " : "");

  if (ImGui::BeginCombo("##ReportBot", selectedHitboxes.str().c_str())) {

    if (ImGui::Selectable("TextAbuse", curSelected & (int)HitBoxes::HEAD,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::HEAD;

    if (ImGui::Selectable("Grief", curSelected & (int)HitBoxes::NECK,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::NECK;

    if (ImGui::Selectable("WallHack", curSelected & (int)HitBoxes::CHEST,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::CHEST;

    if (ImGui::Selectable("AimBot", curSelected & (int)HitBoxes::STOMACH,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::STOMACH;

    if (ImGui::Selectable("SpeedHack", curSelected & (int)HitBoxes::PELVIS,
                          ImGuiSelectableFlags_DontClosePopups))
      CONFIGINT(configVarName) ^= (int)HitBoxes::PELVIS;

    ImGui::EndCombo();
  }
}

void Menu::drawMiscTab() {
    if (ImGui::BeginTabBar("##miscTabs")) {
        if (ImGui::BeginTabItem("Misc")) {
            ImGui::Columns(2, NULL, false);
            ImGui::SetColumnWidth(-1, ImGui::GetWindowContentRegionWidth() * 0.62);
            ImGui::BeginChild("Misc", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.61, ImGui::GetWindowHeight() * 0.64f), true); {
                ImGui::Text("Misc");
                ImGui::Separator();
                ImGui::Checkbox("Developer window", &devWindow);
                ImGui::Checkbox("Disable Watermark", &CONFIGBOOL("Misc>Misc>Misc>Disable Watermark"));
                ImGui::Checkbox("Force square radar", &CONFIGBOOL("Misc>Misc>Misc>Force square radar"));
                ImGui::Checkbox("Rank Revealer", &CONFIGBOOL("Misc>Misc>Misc>Rank Revealer"));

                ImGui::Checkbox("Spectators", &CONFIGBOOL("Misc>Misc>Misc>Spectators"));
                ImGui::Checkbox("Player List", &CONFIGBOOL("Misc>Misc>Misc>Player List"));
                if (CONFIGBOOL("Misc>Misc>Misc>Player List")) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Only When Menu Open", &CONFIGBOOL("Misc>Misc>Misc>Player List Only When Menu Open"));
                }
                ImGui::Checkbox("Flappy Birb", &CONFIGBOOL("Misc>Misc>Misc>Flappy Birb"));
                ImGui::Checkbox("Auto Accept", &CONFIGBOOL("Misc>Misc>Misc>Auto Accept"));
                ImGui::Checkbox("Auto Defuse", &CONFIGBOOL("Misc>Misc>Misc>Auto Defuse"));
                if (CONFIGBOOL("Misc>Misc>Misc>Auto Defuse")) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Rotate", &CONFIGBOOL("Misc>Misc>Misc>Auto Defuse Rotate"));
                }
                ImGui::Checkbox("Chat Filter Bypass", &CONFIGBOOL("Misc>Misc>Misc>Chat Filter Bypass"));
                if (CONFIGBOOL("Misc>Misc>Misc>Use Spam")) {
                    static bool toggled = false;
                    Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Misc>Misc>Misc>Use Spam Key"), &toggled);
                    ImGui::SameLine();
                }
                ImGui::Checkbox("Use Spam", &CONFIGBOOL("Misc>Misc>Misc>Use Spam"));
                ImGui::Checkbox("Disable Setting Cvars", &CONFIGBOOL("Misc>Misc>Misc>Disable Setting Cvars"));
                ImGui::Checkbox("Disable Post Processing", &CONFIGBOOL("Misc>Misc>Misc>Disable Post Processing"));
                ImGui::Checkbox("No Movement Fix", &CONFIGBOOL("Misc>Misc>Misc>No Movement Fix"));
                ImGui::Checkbox("AutoL", &CONFIGBOOL("Misc>Misc>AutoL"));
                ImGui::Checkbox("Report Bot", &CONFIGBOOL("Misc>Misc>ReportBot"));
                if (CONFIGBOOL("Misc>Misc>ReportBot")) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Team Mates", &CONFIGBOOL("Misc>Misc>ReportBot>TeamMate"));
                    ImGui::SameLine();
                    ImGui::Checkbox("InfiniReport", &CONFIGBOOL("Misc>Misc>ReportBot>InfiniReport"));
                    reportBotSelectBox("Misc>Misc>ReportBot>Report Flags");
                }
                static char musicFile[256];
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * 0.315);
                ImGui::InputText("##music-box", musicFile, IM_ARRAYSIZE(musicFile));
                ImGui::SameLine();
                if (ImGui::Button("PlayMusic", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Features::Notifications::addNotification(ImColor(255, 255, 255), "Music Player Started");
                    // Interfaces::engine->ExecuteClientCmd("voice_loopback 1");
                    Interfaces::engine->Voice_RecordStart(musicFile, nullptr, nullptr);
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Features::Notifications::addNotification(ImColor(255, 255, 255), "Stop Playing Music");
                    Interfaces::engine->ExecuteClientCmd("-voicerecord");
                    Interfaces::engine->Voice_RecordStop();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("?");
                if (ImGui::IsItemHovered())
                  ImGui::SetTooltip("Music must be WAV file!");
                ImGui::EndChild();
            }

            ImGui::BeginChild("Hitmarkers", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.61, ImGui::GetWindowHeight() * 0.21f), true); {
                ImGui::Text("Hitmarkers");
                ImGui::Separator();
                ImGui::Checkbox("Hitlogs", &CONFIGBOOL("Misc>Misc>Hitmarkers>Hitlogs"));
                ImGui::Checkbox("Hitmarkers", &CONFIGBOOL("Misc>Misc>Hitmarkers>Hitmarkers"));
                ImGui::Checkbox("Hitsound", &CONFIGBOOL("Misc>Misc>Hitmarkers>Hitsound"));
                ImGui::Checkbox("Damage Markers", &CONFIGBOOL("Misc>Misc>Hitmarkers>Damage Markers"));
                ImGui::EndChild();
            }

            ImGui::NextColumn();

            ImGui::BeginChild("Config", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.38, ImGui::GetWindowHeight() * 0.307), true); {
                ImGui::Text("Config");
                ImGui::Separator();

                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::ListBoxHeader("##configlist-lbox", ImVec2(0, 100));
                for (std::string file : Config::cfgFiles) {
                    if (ImGui::Button(file.c_str())) {
                        strcpy(Config::configFileName, file.c_str());
                    }
                }
                ImGui::ListBoxFooter();

                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::InputText("##configfile-tbox", Config::configFileName, IM_ARRAYSIZE(Config::configFileName));
                if (ImGui::Button("Save", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Config::save();
                }; ImGui::SameLine();
                if (ImGui::Button("Load", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Config::load();
                    // FULLUPDATE();
                    // Features::NameStealer::changeName(true, nullptr);
                }; ImGui::SameLine();
                if (ImGui::Button("Remove", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Config::remove();
                };
                ImGui::EndChild();
            }

            ImGui::BeginChild("Clantag", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.38, ImGui::GetWindowHeight() * 0.22f), true); {
                ImGui::Text("Clantag");
                ImGui::Separator();
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::InputText("##clantag-tbox", clantag, IM_ARRAYSIZE(clantag));
                ImGui::Checkbox("Clantag", &CONFIGBOOL("Misc>Misc>Clantag>Clantag"));
                ImGui::Checkbox("Marquee", &CONFIGBOOL("Misc>Misc>Clantag>Clantag Marquee"));
                ImGui::Checkbox("Bee Movie Clantag", &CONFIGBOOL("Misc>Misc>Clantag>Bee Movie Clantag"));
                ImGui::Checkbox("Brand Clantag", &CONFIGBOOL("Misc>Misc>Clantag>Brand Clantag"));
                ImGui::EndChild();
            }

            ImGui::BeginChild("Name", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.38, ImGui::GetWindowHeight() * 0.14f), true); {
                ImGui::Text("Name");
                ImGui::Separator();
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                ImGui::InputText("##name-tbox", playerName, IM_ARRAYSIZE(playerName));
                if (ImGui::Button("Set", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Features::NameStealer::setName(playerName);
                }; ImGui::SameLine();
                if (ImGui::Button("Set Banned", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.315, 0))) {
                    Features::NameStealer::setBannedName(playerName);
                }; ImGui::SameLine();
                ImGui::Checkbox("Name Stealer", &CONFIGBOOL("Misc>Misc>Name>Name Stealer"));
                ImGui::EndChild();
            }

            ImGui::BeginChild("Movement", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.38, ImGui::GetWindowHeight() * 0.176f), true); {
                ImGui::Text("Movement");
                ImGui::Separator();
                ImGui::Checkbox("Bunny Hop", &CONFIGBOOL("Misc>Misc>Movement>Bunny Hop"));
                ImGui::Checkbox("Auto Strafe", &CONFIGBOOL("Misc>Misc>Movement>Auto Strafe"));
                ImGui::Checkbox("Moon Walk", &CONFIGBOOL("Misc>Misc>Movement>Moon Walk"));
                if (CONFIGBOOL("Misc>Misc>Movement>Edge Jump")) {
                    static bool toggled = false;
                    Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Misc>Misc>Movement>Edge Jump Key"), &toggled);
                    ImGui::SameLine();
                }
                ImGui::Checkbox("Edge Jump", &CONFIGBOOL("Misc>Misc>Movement>Edge Jump"));
                if (CONFIGBOOL("Misc>Misc>Movement>JumpBug")) {
                    static bool toggled = false;
                    Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Misc>Misc>Movement>JumpBug Key"), &toggled);
                    ImGui::SameLine();
                }
                ImGui::Checkbox("JumpBug", &CONFIGBOOL("Misc>Misc>Movement>JumpBug"));
                if (CONFIGBOOL("Misc>Misc>Movement>EdgeBug")) {
                    static bool toggled = false;
                    Menu::CustomWidgets::drawKeyBinder("Key", &CONFIGINT("Misc>Misc>Movement>EdgeBug Key"), &toggled);
                    ImGui::SameLine();
                }
                ImGui::Checkbox("EdgeBug", &CONFIGBOOL("Misc>Misc>Movement>EdgeBug"));
                ImGui::Checkbox("Fast Duck", &CONFIGBOOL("Misc>Misc>Movement>Fast Duck"));
                ImGui::SameLine();
                ImGui::TextDisabled("?");
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("May cause untrusted, use at own risk!");
                ImGui::EndChild();
            }
            ImGui::Columns(1);
            if (ImGui::Button("Unload")) {
                Globals::selfUnload();
            }
            // ImGui::TextDisabled("Credits!");
            // if (ImGui::IsItemHovered())
            //     ImGui::SetTooltip("sekc (of course)\nAll other contributors on GitHub (xXx-sans-xXx, luk1337, cristeigabriel, crazily, dave (@dweee), keenan, u2ooS, tango1337, nigma1337, vampur, all and any other contributors, and allbombson)\nand ocornut for his great ImGui UI framework");
            // ImGui::SameLine();
            // ImGui::Text("Secret Hideout");
            // if (ImGui::IsItemClicked()) {
            //     system("xdg-open https://discord.gg/YnPscrrgDG &"); //if it works it works lmao
            // }
            // if (ImGui::IsItemHovered()) {
            //     ImGui::SetTooltip("Join the secret hideout discord! (win me a knife)");
            // }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Skins")) {
            static ItemIndex curWeaponSelected = ItemIndex::WEAPON_AK47;
            if (ImGui::BeginCombo("Weapon", itemIndexMap.at(curWeaponSelected))) {
                for (auto item : itemIndexMap) {
                    if (item.first != ItemIndex::INVALID) {
                        ImGui::PushID(item.second);
                        const bool is_selected = (itemIndexMap.at(curWeaponSelected) == item.second);
                        if (ImGui::Selectable(item.second, is_selected)) {
                            curWeaponSelected = item.first;
                            FULLUPDATE();
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::EndCombo();
            }
            if (curWeaponSelected != ItemIndex::INVALID) {
                char* buf = new char[256];
                snprintf(buf, 256, "Misc>Skins>Skins>%s>PaintKit", itemIndexMap.at(curWeaponSelected));
                if (ImGui::DragInt("PaintKit", &CONFIGINT(buf))) {
                    FULLUPDATE();
                }

                char* buf2 = new char[256];
                snprintf(buf2, 256, "Misc>Skins>Skins>%s>Wear", itemIndexMap.at(curWeaponSelected));
                if (ImGui::SliderInt("Wear", &CONFIGINT(buf2), 0, 100)) {
                    FULLUPDATE();
                }

                char* buf3 = new char[256];
                snprintf(buf3, 256, "Misc>Skins>Skins>%s>StatTrack", itemIndexMap.at(curWeaponSelected));
                if (ImGui::DragInt("StatTrack", &CONFIGINT(buf3))) {
                    FULLUPDATE();
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
