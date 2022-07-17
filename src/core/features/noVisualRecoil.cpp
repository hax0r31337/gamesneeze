#include "features.hpp"
#include "../../sdk/sdk.hpp"

 void Features::NoVisualRecoil::frameStageNotify(FrameStage frame) {
    if (frame != FRAME_RENDER_START || !Globals::localPlayer) {
        return;
    }

    if (CONFIGBOOL("Visuals>Players>LocalPlayer>No Aim Punch")) {
      Globals::localPlayer->aimPunch() = QAngle(0, 0, 0);
    }
    if (CONFIGBOOL("Visuals>Players>LocalPlayer>No View Punch")) {
      Globals::localPlayer->viewPunch() = QAngle(0, 0, 0);
    }
}