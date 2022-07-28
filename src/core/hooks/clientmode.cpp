#include "../../includes.hpp"
#include "hooks.hpp"
#include <algorithm>
#include <cstdint>

bool Hooks::CreateMove::hook(void *thisptr, float flInputSampleTime,
                             CUserCmd *cmd) {
  original(thisptr, flInputSampleTime, cmd);
  if (cmd->tick_count != 0) {
    uintptr_t rbp;
    asm volatile("mov %%rbp, %0" : "=r"(rbp));
    Globals::sendPacket =
        ((*(bool **)rbp) -
         0x18); // https://www.unknowncheats.me/forum/counterstrike-global-offensive/290258-updating-bsendpacket-linux.html

    if (Interfaces::engine->IsInGame()) {
      static ConVar *mat_postprocess_enable =
          Interfaces::convar->FindVar("mat_postprocess_enable");
      if (mat_postprocess_enable) {
        mat_postprocess_enable->SetValue(
            !CONFIGBOOL("Misc>Misc>Misc>Disable Post Processing"));
      }
    }
    bool noMovementFix = CONFIGBOOL("Misc>Misc>Misc>No Movement Fix");
    Globals::serverTime(cmd);

    if (!noMovementFix) {
      startMovementFix(cmd);
    }
    Features::RankReveal::createMove(cmd);
    Features::FastDuck::createMove(cmd);
    Features::FakeDuck::createMove(cmd);
    Features::UseSpam::createMove(cmd);
    Features::Movement::prePredCreateMove(cmd);

    Features::Prediction::start(cmd);
    // if (CONFIGBOOL("Rage>Enabled")) {
    Features::RageBot::createMove(cmd);
    Features::FakeLag::createMove(cmd);
    Features::AntiAim::createMove(cmd);
    // } else {
    Features::LegitBot::createMove(cmd);
    Features::Triggerbot::createMove(cmd);
    Features::Backtrack::store(cmd);
    Features::Backtrack::createMove(cmd);
    Features::Forwardtrack::createMove(cmd);
    // }
    Features::Prediction::end();

    Features::Movement::postPredCreateMove(cmd);

    Features::AutoDefuse::createMove(cmd);
    if (!noMovementFix) {
      endMovementFix(cmd);
    }
    Features::QuickPeek::createMove(cmd);
    Features::SlowWalk::createMove(cmd);
    Features::ReportBot::createMove(cmd);

    auto view_backup = cmd->viewangles;
    Features::Movement::edgeBugPredictor(cmd);
    if (!noMovementFix) {
      startMovementFix(cmd);
    }
    cmd->viewangles = view_backup;
    if (!noMovementFix) {
      endMovementFix(cmd);
    }

    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);
    cmd->upmove = std::clamp(cmd->upmove, -320.0f, 320.0f);

    cmd->viewangles.normalize();
    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;

    Globals::oldViewangles = cmd->viewangles;
  }

  // return !(CONFIGBOOL("Rage>Enabled"));
  return false;
}

void Hooks::OverrideView::hook(void* thisptr, ViewSetup* setup) {
    if (Interfaces::engine->IsInGame() && Globals::localPlayer && Globals::localPlayer->health() > 0 && !Globals::localPlayer->scoped()) {
        static bool thirdPerson = false;
        static bool toggleThirdPerson = false;
        
        setup->fov = CONFIGINT("Visuals>World>World>FOV");
        // Third Person from Fuzion
        QAngle viewAngles;
        Interfaces::engine->GetViewAngles(viewAngles);
        Trace tr;
        Ray traceRay;
        Vector eyePos = Globals::localPlayer->eyePos();

        Vector camOff = Vector(cos(DEG2RAD(viewAngles.y)) * 100,sin(DEG2RAD(viewAngles.y)) * 100,sin(DEG2RAD(-viewAngles.x)) * 100);

        traceRay.Init(eyePos, (eyePos - camOff));
        TraceFilter traceFilter;
        traceFilter.pSkip = Globals::localPlayer;
        Interfaces::trace->TraceRay(traceRay, 0x1, &traceFilter, &tr);

        if (thirdPerson)
            Interfaces::input->m_vecCameraOffset = Vector(viewAngles.x, viewAngles.y, 100 * ((tr.fraction < 1.0f) ? tr.fraction : 1.0f) );
        Interfaces::input->m_fCameraInThirdPerson = thirdPerson;

        if (CONFIGBOOL("Visuals>World>World>Third Person") && Menu::CustomWidgets::isKeyDown(CONFIGINT("Visuals>World>World>Third Person Key"))) {
            if (!toggleThirdPerson) {
                toggleThirdPerson = true;
                thirdPerson = !thirdPerson;
            }
        } else {
            toggleThirdPerson = false;
        }

        Weapon *weapon = (Weapon *) Interfaces::entityList->GetClientEntity((uintptr_t) Globals::localPlayer->activeWeapon() & 0xFFF);
        if(weapon) {
            if (weapon->itemIndex() == ItemIndex::WEAPON_DECOY || weapon->itemIndex() == ItemIndex::WEAPON_HEGRENADE || weapon->itemIndex() == ItemIndex::WEAPON_FLASHBANG || weapon->itemIndex() == ItemIndex::WEAPON_SMOKEGRENADE || weapon->itemIndex() == ItemIndex::WEAPON_MOLOTOV || weapon->itemIndex() == ItemIndex::WEAPON_INCGRENADE) {
                Interfaces::input->m_fCameraInThirdPerson = false;
            }
        }
    }
    Globals::FOV = setup->fov;
    original(thisptr, setup);
}

float Hooks::GetViewmodelFOV::hook(void* thisptr) {
    if (CONFIGINT("Visuals>World>World>Viewmodel FOV") != 0) {
        if (Globals::localPlayer && Globals::localPlayer->health() > 0) {
            return CONFIGINT("Visuals>World>World>Viewmodel FOV");
        }
    }

    return original(thisptr);
}