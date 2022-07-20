#include "../../includes.hpp"
#include "features.hpp"
#include <algorithm>
#include <cmath>

void bhop(CUserCmd *cmd) {
  if (!Interfaces::engine->IsInGame())
    return;
  if (!Globals::localPlayer)
    return;
  if (Globals::localPlayer->moveType() == MOVETYPE_NOCLIP ||
      Globals::localPlayer->moveType() == MOVETYPE_LADDER)
    return;

  if (CONFIGBOOL("Misc>Misc>Movement>Bunny Hop")) {
    static bool jumped_last_tick = false;
    static bool should_fake_jump = false;

    if (!jumped_last_tick && should_fake_jump) {
      should_fake_jump = false;
      cmd->buttons |= IN_JUMP;
    } else if (cmd->buttons & IN_JUMP) {
      if (Globals::localPlayer->flags() & FL_ONGROUND) {
        jumped_last_tick = true;
        should_fake_jump = true;
      } else {
        cmd->buttons &= ~IN_JUMP;
        jumped_last_tick = false;
      }
    } else {
      jumped_last_tick = false;
      should_fake_jump = false;
    }
  }
}

void autoStrafe(CUserCmd *cmd) {
  if (!CONFIGBOOL("Misc>Misc>Movement>Auto Strafe")) 
    return;
  if (!Interfaces::engine->IsInGame())
    return;
  if (!Globals::localPlayer)
    return;
  if (Globals::localPlayer->moveType() == MOVETYPE_NOCLIP || Globals::localPlayer->moveType() == MOVETYPE_LADDER)
    return;

  if (!(Globals::localPlayer->flags() & FL_ONGROUND)) {
    if (cmd->mousedx > 1 || cmd->mousedx < -1) {
      cmd->sidemove = cmd->mousedx < 0.f ? -400.f : 400.f;
    } else {
      auto l2d = Globals::localPlayer->velocity().Length2D();
      if (l2d == 0 || l2d == INFINITY || l2d == NAN) {
        cmd->forwardmove = 400;
        return;
      }
      cmd->forwardmove = std::clamp(5850.f / l2d,
                                    -400.0f, 400.0f);
      cmd->sidemove = (cmd->command_number % 2) == 0 ? -400.f : 400.f;
    }
  }
}

void edgeJump(CUserCmd *cmd) {
    if (CONFIGBOOL("Misc>Misc>Movement>Edge Jump") &&
        Menu::CustomWidgets::isKeyDown(CONFIGINT("Misc>Misc>Movement>Edge Jump Key")) &&
        Features::Movement::flagsBackup & FL_ONGROUND &&
        !(Globals::localPlayer->flags() & FL_ONGROUND))
        cmd->buttons |= IN_JUMP;
}

void jumpBug(CUserCmd *cmd) {
    static bool shouldSkip = false;
    if (shouldSkip) {
        shouldSkip = false;
        return;
    }
    if (CONFIGBOOL("Misc>Misc>Movement>JumpBug") &&
        Menu::CustomWidgets::isKeyDown(CONFIGINT("Misc>Misc>Movement>JumpBug Key")) &&
        !(Features::Movement::flagsBackup & FL_ONGROUND ||
          Features::Movement::flagsBackup & FL_PARTIALGROUND) &&
        (Globals::localPlayer->flags() & FL_ONGROUND ||
         Globals::localPlayer->flags() & FL_PARTIALGROUND)) {
        cmd->buttons |= IN_DUCK;
        cmd->buttons &= ~IN_JUMP;
        shouldSkip = true;
    }
}

bool checkEdgebug() {
    static ConVar *sv_gravity = Interfaces::convar->FindVar("sv_gravity");
    float edgebugZVel =
       (sv_gravity->GetFloat() * 0.5f * Interfaces::globals->interval_per_tick);

    return Features::Movement::velBackup.z < -edgebugZVel &&
           round(Globals::localPlayer->velocity().z) == -round(edgebugZVel) &&
           Globals::localPlayer->moveType() != MOVETYPE_LADDER;
}

void Features::Movement::prePredCreateMove(CUserCmd *cmd) {
    if (!Globals::localPlayer)
        return;

    flagsBackup = Globals::localPlayer->flags();
    velBackup = Globals::localPlayer->velocity();
    bhop(cmd);
    autoStrafe(cmd);

    if (shouldEdgebug && shouldDuckNext)
        cmd->buttons |= IN_DUCK;
}

void Features::Movement::postPredCreateMove(CUserCmd *cmd) {
    if (!Globals::localPlayer || Globals::localPlayer->moveType() == MOVETYPE_LADDER ||
        Globals::localPlayer->moveType() == MOVETYPE_NOCLIP)
        return;

    edgeJump(cmd);
    jumpBug(cmd);
}

void Features::Movement::edgeBugPredictor(CUserCmd *cmd) {
    if (!CONFIGBOOL("Misc>Misc>Movement>EdgeBug") ||
        !Menu::CustomWidgets::isKeyDown(CONFIGINT("Misc>Misc>Movement>EdgeBug Key")) ||
        !Globals::localPlayer->health())
        return;

    struct MovementVars {
        QAngle viewangles;
        QAngle view_delta;
        float forwardmove;
        float sidemove;
        int buttons;
    };
    static MovementVars backup_move;
    MovementVars original_move;
    original_move.viewangles = cmd->viewangles;
    original_move.view_delta = (cmd->viewangles - Globals::oldViewangles);
    original_move.forwardmove = cmd->forwardmove;
    original_move.sidemove = cmd->sidemove;
    original_move.buttons = cmd->buttons;
    if (!shouldEdgebug)
        backup_move = original_move;

    int nCmdsPred = Interfaces::prediction->Split->nCommandsPredicted;

    int predictAmount = 128; // TODO: make amount configurable
    for (int t = 0; t < 4; t++) {
        Features::Prediction::restoreEntityToPredictedFrame(nCmdsPred - 1);

        bool doStrafe = (t % 2 == 0);
        bool doDuck = t > 1;

        cmd->viewangles = backup_move.viewangles;

        for (int i = 0; i < predictAmount; i++) {
            if (doStrafe) {
                cmd->viewangles += backup_move.view_delta;
                cmd->forwardmove = backup_move.forwardmove;
                cmd->sidemove = backup_move.sidemove;
            } else {
                cmd->forwardmove = 0.f;
                cmd->sidemove = 0.f;
            }
            if (doDuck)
                cmd->buttons |= IN_DUCK;
            else
                cmd->buttons &= ~IN_DUCK;

            Features::Prediction::start(cmd);
            shouldEdgebug = checkEdgebug();
            velBackup = Globals::localPlayer->velocity();
            edgebugPos = Globals::localPlayer->origin();
            Features::Prediction::end();
            if (Globals::localPlayer->flags() & FL_ONGROUND || Globals::localPlayer->moveType() == MOVETYPE_LADDER) {
                break;
            }
            if (shouldEdgebug) {
                shouldDuckNext = doDuck;
                if (doStrafe) {
                    cmd->viewangles = backup_move.viewangles + backup_move.view_delta;
                    backup_move.viewangles = cmd->viewangles;
                }
                if (i == 1)
                    Interfaces::engine->ExecuteClientCmd(
                       "play buttons/blip1.wav"); // TODO: play sound via a better method
                return;
            }
        }
    }

    cmd->viewangles = original_move.viewangles;
    cmd->forwardmove = original_move.forwardmove;
    cmd->sidemove = original_move.sidemove;
    cmd->buttons = original_move.buttons;
}

void Features::Movement::draw() {
    if (Features::Movement::shouldEdgebug) {
        Globals::drawList->AddText(
           ImVec2((Globals::screenSizeX / 2) - (ImGui::CalcTextSize("EdgeBug").x / 2) + 1,
                  (Globals::screenSizeY / 2) + 31),
           ImColor(0, 0, 0, 255), "EdgeBug");
        Globals::drawList->AddText(
           ImVec2((Globals::screenSizeX / 2) - (ImGui::CalcTextSize("EdgeBug").x / 2),
                  (Globals::screenSizeY / 2) + 30),
           ImColor(255, 255, 255, 255), "EdgeBug");

        Vector edgebugPos2D;
        if (worldToScreen(edgebugPos, edgebugPos2D)) {
            Globals::drawList->AddText(
               ImVec2(edgebugPos2D.x - (ImGui::CalcTextSize("gaming").x / 2) + 1,
                      edgebugPos2D.y + 1),
               ImColor(0, 0, 0, 255), "gaming");
            Globals::drawList->AddText(
               ImVec2(edgebugPos2D.x - (ImGui::CalcTextSize("gaming").x / 2),
                      edgebugPos2D.y),
               ImColor(255, 255, 255, 255), "gaming");
        }
    }
}