#include "features.hpp"

void Features::FakeLag::createMove(CUserCmd *cmd) {
  if (!Globals::localPlayer || Globals::localPlayer->health() <= 0 || !CONFIGBOOL("Rage>Fake Lag>Enabled"))
    return;
  if (Interfaces::engine->IsVoiceRecording())
    return;
  int velocity2d = Globals::localPlayer->velocity().Length2D();
  int max_choke;
  const auto netchannel = Offsets::getLocalClient(-1)->m_NetChannel;

  static int ticks = 0;

  if (cmd->buttons & IN_ATTACK) {
    *Globals::sendPacket = false;

    if (CONFIGBOOL("Rage>Fake Lag>Choke On Shot")) {
      max_choke = 25;
    }
  }

  if (CONFIGBOOL("Rage>Fake Lag>Adaptive")) {
    if (velocity2d >= 5.0f) {
      auto dynamic_factor =
          std::ceil(64.0f / (velocity2d * Interfaces::globals->interval_per_tick));

      if (dynamic_factor > 16) {
        dynamic_factor = CONFIGINT("Rage>Fake Lag>Amount");
      }

      max_choke = dynamic_factor;
    } else {
      max_choke = CONFIGINT("Rage>Fake Lag>Amount");
    }
  } else if (!(cmd->buttons & IN_ATTACK) || !CONFIGBOOL("Rage>Fake Lag>Choke On Shot")) {
    max_choke = CONFIGINT("Rage>Fake Lag>Amount");
  }

  if (ticks >= max_choke ||
      /*((*csGameRules)->IsValveDS() &&*/ netchannel->m_nChokedPackets > 6) {
    *Globals::sendPacket = true;
    ticks = -1;
  } else {
    *Globals::sendPacket = false;
  }
  ticks++;
}