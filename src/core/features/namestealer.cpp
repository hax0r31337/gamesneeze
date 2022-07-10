#include "../../includes.hpp"
#include "features.hpp"
#include <cstring>
#include <string>

bool Features::NameStealer::changeName(bool reconnect, const char *newName) {
  static auto exploitInitialized{false};

  static auto name{Interfaces::convar->FindVar("name")};

  if (reconnect) {
    exploitInitialized = false;
    return false;
  }

  if (!exploitInitialized && Interfaces::engine->IsInGame()) {
    // if (player_info_t playerInfo;
    //     Globals::localPlayer &&
    //     Interfaces::engine->GetPlayerInfo(Globals::localPlayer->index(), &playerInfo) &&
    //     (!strcmp(playerInfo.name, "?empty") ||
    //      !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
    //   exploitInitialized = true;
    // } else {
    //   name->fnChangeCallback = 0;
    //   name->SetValue("\n\xAD\xAD\xAD");
    //   return false;
    // }
    exploitInitialized = true;
    name->fnChangeCallback = 0;
    name->SetValue("\n\xAD\xAD\xAD");
    return false;
  }

  name->SetValue(newName);
  return true;
}

void Features::NameStealer::setName(const char *name) {
  changeName(false, name);
}

void Features::NameStealer::setBannedName(const char *name) {
    std::string banText(name);
    banText += " has been permanently banned from official CS:GO servers.";
    std::string res = " \x01\x0B";
    res += (char)(Features::NameStealer::Colors::LIGHT_RED);
    res.append(banText);
    res.append("\230");
    changeName(false, res.c_str());
}

void Features::NameStealer::frameStageNotify() {
  if (!CONFIGBOOL("Misc>Misc>Name>Name Stealer")) {
    return;
  }

  if (!Interfaces::engine->IsInGame())
    return;
  if (!Globals::localPlayer)
    return;

  static std::vector<int> stolenIds;

  for (int i = 1; i <= Interfaces::globals->maxClients; ++i) {
    const auto entity = Interfaces::entityList->GetClientEntity(i);

    if (!entity || i == Globals::localPlayer->index())
      continue;

    player_info_t playerInfo;
    if (!Interfaces::engine->GetPlayerInfo(i, &playerInfo))
      continue;

    if (playerInfo.fakeplayer || !((*Player)entity)->isEnemy() ||
        std::ranges::find(stolenIds, playerInfo.userid) != stolenIds.cend())
      continue;

    if (changeName(false, (std::string{playerInfo.name} + '\x1').c_str())) {
    //   memcpy(Menu::playerName, (std::string{playerInfo.name} + '\x1').c_str(), 127);
      stolenIds.push_back(playerInfo.userid);
    }

    return;
  }
  stolenIds.clear();
}