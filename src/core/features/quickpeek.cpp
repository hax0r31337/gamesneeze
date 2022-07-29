#include "features.hpp"

static bool hasShot;
static Vector quickpeekstartpos = Vector{ 0, 0, 0 };

void Features::QuickPeek::gotoStart(CUserCmd *cmd) {
  float yaw = cmd->viewangles.y;
  Vector VecForward = Globals::localPlayer->origin() - quickpeekstartpos;

  Vector translatedVelocity =
      Vector{(float)(VecForward.x * cos(yaw / 180 * (float)M_PI) +
                     VecForward.y * sin(yaw / 180 * (float)M_PI)),
             (float)(VecForward.y * cos(yaw / 180 * (float)M_PI) -
                     VecForward.x * sin(yaw / 180 * (float)M_PI)),
             VecForward.z};
  cmd->forwardmove = -translatedVelocity.x * 20.f;
  cmd->sidemove = translatedVelocity.y * 20.f;
}

void Features::QuickPeek::createMove(CUserCmd *cmd) {
  if (!CONFIGBOOL("Rage>Quick Peek>Enabled") || !Globals::localPlayer || !Interfaces::engine->IsInGame())
    return;
  if (Menu::CustomWidgets::isKeyDown(CONFIGINT("Rage>Quick Peek>Key"))) {
    if (quickpeekstartpos == Vector{0, 0, 0}) {
      quickpeekstartpos = Globals::localPlayer->origin();
    } else {
      if (cmd->buttons & IN_ATTACK)
        hasShot = true;
      if (hasShot) {
        gotoStart(cmd);
      }
    }
  } else {
    hasShot = false;
    quickpeekstartpos = Vector{0, 0, 0};
  }
}