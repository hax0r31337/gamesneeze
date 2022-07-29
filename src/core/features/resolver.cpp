#include "features.hpp"

void Features::Resolver::frameStageNotify(FrameStage frame) {
    if (!CONFIGBOOL("Rage>Resolver")) {
        return;
    }
    if (frame == FRAME_RENDER_START) {
      static auto jigglebones = Interfaces::convar->FindVar("r_jiggle_bones");
      jigglebones->SetValue(0);
    }
    // if (frame == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
    //     for (int i = 1; i < Interfaces::globals->maxClients; i++) {
    //         Player* p = (Player*)Interfaces::entityList->GetClientEntity(i);
    //         if (p && p != Globals::localPlayer && p->health() > 0 && !p->dormant() && p->isEnemy()) {
    //             p->eyeAngles_ptr()->y = p->lbyTarget() + (rand() % 100) - 50; // p resolve
    //         }
    //     }
    // }
}