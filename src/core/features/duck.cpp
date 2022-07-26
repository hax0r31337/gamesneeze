#include "features.hpp"

void Features::FastDuck::createMove(CUserCmd* cmd) {
    if (CONFIGBOOL("Misc>Misc>Movement>Fast Duck") && Globals::localPlayer && Globals::localPlayer->health() > 0) {
        cmd->buttons |= IN_BULLRUSH;
    }
}

void Features::FakeDuck::createMove(CUserCmd *cmd) {
  if (!CONFIGBOOL("Misc>Misc>Movement>Fake Duck") || !Globals::localPlayer ||
      Globals::localPlayer->health() <= 0 || Globals::localPlayer->gunGameImmunity() ||
      Globals::localPlayer->flags() & FL_FROZEN || cmd->buttons & IN_JUMP ||
      !(Globals::localPlayer->flags() & FL_ONGROUND)) {
    return;
  }

  cmd->buttons |= IN_BULLRUSH;
  static int chokes = 0;

  if (Menu::CustomWidgets::isKeyDown(
          CONFIGINT("Misc>Misc>Movement>Fake Duck Key"))) {
    if (chokes <= 6) {
        cmd->buttons &= ~IN_DUCK;
    } else {
        cmd->buttons |= IN_DUCK;
    }
    *Globals::sendPacket = chokes >= 14;
  }

  if (*Globals::sendPacket) {
    chokes = 0;
  } else {
    chokes++;
  }
}