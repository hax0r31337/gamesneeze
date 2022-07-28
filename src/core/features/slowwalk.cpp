#include "features.h"
#include "../../includes.hpp"

void Features::SlowWalk::createMove(CUserCmd *cmd) {
    if (Interfaces::engine->IsInGame() && Globals::localPlayer && Globals::localPlayer->health() > 0 && 
            Globals::localPlayer->flags() & (1 << 0) && cmd->buttons & IN_SPEED && CONFIGBOOL("Rage>Slow Walk") &&
            (Globals::localPlayer->velocity().Length2D() > CONFIGINT("Rage>Slow Walk Speed"))) {
        cmd->forwardmove = 0;
        cmd->sidemove = 0;
    }
}
