#include "features.hpp"

void Features::AutoDefuse::createMove(CUserCmd* cmd) {
    if (!CONFIGBOOL("Misc>Misc>Misc>Auto Defuse") || !Globals::localPlayer || !Interfaces::engine->IsInGame() || Globals::bombData.blowTime != 0
     || getDistanceNoSqrt(Globals::localPlayer->origin(), Globals::bombData.origin) < 5625) { // Could also check whether you're looking at the bomb but distance check should be good enough
        float timeRemaining = Globals::bombData.blowTime - (Interfaces::globals->curtime + ((float)playerResource->GetPing(Globals::localPlayer->index())/1000.f));
        if (timeRemaining < (Globals::localPlayer->defuser() ? 5.1f: 10.1f)) {
          cmd->buttons |= (1 << 5);
          if (CONFIGBOOL("Misc>Misc>Misc>Auto Defuse Rotate")) {
            cmd->viewangles.x = 89;
            cmd->viewangles.y = 180;
          }
        }
    }
}