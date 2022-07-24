#include "../../includes.hpp"
#include "hooks.hpp"

void Hooks::FrameStageNotify::hook(void* thisptr, FrameStage frame) {
    Globals::localPlayer = (Player*)Interfaces::entityList->GetClientEntity(Interfaces::engine->GetLocalPlayer());

    if (Interfaces::engine->IsConnected() && !Interfaces::engine->IsInGame()) {
      Features::NameStealer::changeName(true, "");
    }

    Features::ClantagChanger::frameStageNotify(frame);
    Features::NameStealer::frameStageNotify(frame);
    Features::SkyboxChanger::frameStageNotify(frame);
    Features::RecoilCrosshair::frameStageNotify(frame);
    Features::SkinChanger::frameStageNotify(frame);
    Features::NoFlash::frameStageNotify(frame);
    Features::RagdollGravity::frameStageNotify(frame);
    Features::NoVisualRecoil::frameStageNotify(frame);
    Features::RainbowHud::frameStageNotify(frame);

    if (frame == FRAME_RENDER_START) {
        cachePlayers();
        Globals::bombData.update();
        if (Globals::localPlayer && Globals::localPlayer->health() > 0 && Interfaces::input->m_fCameraInThirdPerson) {
          Globals::localPlayer->viewAngles()->y = Globals::oldViewangles.y;
          Globals::localPlayer->viewAngles()->x = Globals::oldViewangles.x;
        }
    }

    Features::Resolver::frameStageNotify(frame);

    original(thisptr, frame);
}