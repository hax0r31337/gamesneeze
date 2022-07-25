#include "features.hpp"

void Features::VoteRevealer::event(IGameEvent *event) {
  if (!CONFIGBOOL("Misc>Misc>Misc>Reveal Vote")) {
    return;
  }

  const auto entity = (Player*)Interfaces::entityList->GetClientEntity(event->GetInt("entityid"));
  if (!entity || !entity->isPlayer()) {
    return;
  }

  player_info_t info;
  if (!Interfaces::engine->GetPlayerInfo(entity->index(), &info)) {
    return;
  }

  const auto votedYes = event->GetInt("vote_option") == 0;
  const auto isLocal = Globals::localPlayer && entity->index() == Globals::localPlayer->index();

  Notifications::addNotification(votedYes ? ImColor(0, 255, 0) : ImColor(255, 0, 0), "[vote] %s voted %s",
                                 isLocal ? "You" : info.name, votedYes ? "Yes" : "No");
}