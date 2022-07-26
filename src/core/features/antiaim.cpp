#include "../../includes.hpp"
#include "features.hpp"

bool updatingLby() {
    // cred: Somewhere from UC, can't remember
    // AnimState* animState = Globals::localPlayer->animState();
    // float curtime = Globals::localPlayer->tickbase() * Interfaces::globals->interval_per_tick;
    // static float lbyTime;
     
    // if (animState->verticalVelocity > 0.1f || fabs(animState->horizontalVelocity) > 100.0f) {
    //     lbyTime = curtime + 0.22f;
    //     return false;
    // }
    // else if (curtime > lbyTime) {
    //     lbyTime = curtime + 1.1f;
    //     return true;
    // }
    // return false;

    // cred: https://github.com/danielkrupinski/Osiris/pull/2065/
    auto serverTime = Globals::serverTime();
    static float nextLby, lastLby;

    if (!(Globals::localPlayer->flags() & FL_ONGROUND)) {
      return false;
    }
    if (Globals::localPlayer->velocity().notNull() > 0.1f) {
      nextLby = 0.22f;
      return false;
    }
    if (serverTime - lastLby - nextLby >= 0) {
      nextLby = 1.125f;
      lastLby = serverTime;
      return true;
    } else {
      return false;
    }
}

void Features::AntiAim::createMove(CUserCmd* cmd) {
    if (CONFIGINT("Rage>AntiAim>Type") != 0 && Globals::localPlayer && Globals::localPlayer->health() > 0 && Globals::localPlayer->moveType() == MOVETYPE_WALK && Interfaces::engine->IsInGame()) {
      Weapon *weapon = (Weapon *)Interfaces::entityList->GetClientEntity(
          (uintptr_t)Globals::localPlayer->activeWeapon() & 0xFFF);
      if (weapon) {
        if (!(weapon->itemIndex() == ItemIndex::WEAPON_DECOY ||
              weapon->itemIndex() == ItemIndex::WEAPON_HEGRENADE ||
              weapon->itemIndex() == ItemIndex::WEAPON_FLASHBANG ||
              weapon->itemIndex() == ItemIndex::WEAPON_SMOKEGRENADE ||
              weapon->itemIndex() == ItemIndex::WEAPON_MOLOTOV ||
              weapon->itemIndex() == ItemIndex::WEAPON_INCGRENADE)) {
          if (!((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_USE))) {
            if (CONFIGINT("Rage>AntiAim>Type")) {
              // TODO: for some reason it refuses to desync when looking
              // forward???!?!?

              int real = 0;
              int fake = 0;

              switch (CONFIGINT("Rage>AntiAim>Type")) {
              case 1: { // Static
                real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset");
                fake = CONFIGINT("Rage>AntiAim>Static>Desync");
                break;
              }
              case 2: { // Jitter
                int jitterAmt =
                    (((cmd->tick_count %
                       CONFIGINT("Rage>AntiAim>Jitter>Jitter Delay")) <
                      CONFIGINT("Rage>AntiAim>Jitter>Jitter Delay") / 2)
                         ? 1
                         : -1) *
                    (CONFIGINT("Rage>AntiAim>Jitter>Jitter Amount") -
                     (CONFIGINT("Rage>AntiAim>Jitter>Jitter Amount") / 2));
                real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset") +
                       jitterAmt;
                fake = CONFIGINT("Rage>AntiAim>Jitter>Desync");
                break;
              }
              case 3: { // Fake Jitter
                int jitterAmt =
                    (((cmd->tick_count %
                       CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Delay")) <
                      CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Delay") / 2)
                         ? 1
                         : -1) *
                    (CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Amount") -
                     (CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Amount") / 2));
                real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset");
                fake = jitterAmt;
                break;
              }
              case 4: { // Real Jitter
                int jitterAmt =
                    (((cmd->tick_count %
                       CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Delay")) <
                      CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Delay") / 2)
                         ? 1
                         : -1) *
                    (CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Amount") -
                     (CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Amount") / 2));
                real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset") +
                       jitterAmt;
                fake = CONFIGINT("Rage>AntiAim>Static>Desync") - jitterAmt;
                break;
              }
              case 5: { // Spin
                real = cmd->viewangles.y +
                       (cmd->tick_count * CONFIGINT("Rage>AntiAim>Offset"));
                fake = CONFIGINT("Rage>AntiAim>Spin>Desync");
                break;
              }
              case 6: { // Legit
                float delta = Globals::localPlayer->getMaxDesyncAngle() * 2;
                static bool invert = true;
                auto realtime = Interfaces::globals->realtime;
                static float lastTime{0.0f};

                if (realtime - lastTime >
                    CONFIGINT("Rage>AntiAim>FakeLag") / 0.125) {
                  invert = !invert;
                  lastTime = realtime;
                }

                if (updatingLby()) {
                  Notifications::addNotification(ImColor(255, 0, 0), "LBY");
                  *Globals::sendPacket = false;
                  invert ? cmd->viewangles.y -= delta
                         : cmd->viewangles.y += delta;
                }

                delta = Globals::localPlayer->getMaxDesyncAngle();

                if (Globals::sendPacket) {
                  invert ? cmd->viewangles.y += delta
                         : cmd->viewangles.y -= delta;
                } else {
                  	cmd->viewangles.y += 180.f;
                }

                cmd->buttons &=
                    ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);

                return;
              }
              }

              // TODO Check for net channel group 9 so we can desync and yell at
              // nn's at the same time
              int fakelag = CONFIGINT("Rage>AntiAim>FakeLag");
              if (Interfaces::engine->IsVoiceRecording()) {
                fakelag = 0;
              } else {
                fakelag = CONFIGINT("Rage>AntiAim>FakeLag");
              }
              cmd->viewangles.x = CONFIGINT("Rage>AntiAim>Pitch");

              *Globals::sendPacket =
                  cmd->tick_count %
                  (CONFIGINT("Rage>AntiAim>Type") + fakelag + 1);

              if (updatingLby()) {
                cmd->viewangles.y = real + (fake * 2);
                *Globals::sendPacket = false;
              } else if (*Globals::sendPacket) {
                cmd->viewangles.y = real;
                fakeYaw = real;
              }
            }
          }
        }
      }
    }
}