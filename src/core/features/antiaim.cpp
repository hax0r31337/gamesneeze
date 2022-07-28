#include "../../includes.hpp"
#include "features.hpp"

Player* Features::AntiAim::getNearestTarget(bool check) {
  Player* best_ent = nullptr;
  float best_dist = FLT_MAX;
  const int x = Globals::screenSizeX / 2, y = Globals::screenSizeY / 2;

  for (int i = 1; i <= Interfaces::globals->maxClients; ++i) {
    const auto entity = (Player*)Interfaces::entityList->GetClientEntity(i);

    if (!entity || i == Globals::localPlayer->index() ||
      entity->health() <= 0 || !entity->isEnemy()) {
      continue;
    }

    Vector origin_2d;
    if (!worldToScreen(entity->origin(), origin_2d) && check) {
      continue;
    }

    float dist = Vector(x, y, 0).DistTo(origin_2d);
    if (dist < best_dist) {
      best_ent = entity;
      best_dist = dist;
    }
  }

  return best_ent;
}

void Features::AntiAim::createMove(CUserCmd* cmd) {
  if (!CONFIGBOOL("Rage>AntiAim>Enabled") || !Interfaces::engine->IsInGame() || !Globals::localPlayer || Globals::localPlayer->flags() & FL_FROZEN ||
      Globals::localPlayer->health() <= 0 || Globals::localPlayer->gunGameImmunity() ||
      Globals::localPlayer->moveType() != MOVETYPE_WALK ||
      !Interfaces::engine->IsInGame() || cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE) {
    return;
  }

  const auto weapon = (Weapon *)Interfaces::entityList->GetClientEntity(
      (uintptr_t)Globals::localPlayer->activeWeapon() & 0xFFF);
  if (!weapon) {
    return;
  }

  cmd->viewangles.x = CONFIGINT("Rage>AntiAim>Pitch");

  bool check = false/*
      vars.antiaim.ignore_attarget &&
      (g_Binds[bind_manual_back].active || g_Binds[bind_manual_right].active ||
       g_Binds[bind_manual_left].active || g_Binds[bind_manual_forward].active)*/;

  if (CONFIGBOOL("Rage>AntiAim>At Target") && !check) {
    auto best_ent = getNearestTarget(true);
    if (best_ent) {
      cmd->viewangles.y = calcAngle(Globals::localPlayer->origin(), best_ent->origin()).y;
    }
  }

  static bool sw = false;
  // static bool avoid_overlap_side = false;
  // static float last_angle = 0.f;
  bool invertAA = Menu::CustomWidgets::isKeyDown(CONFIGINT("Rage>AntiAim>Invert Key"));

  int side = invertAA ? 1 : -1;

  if (CONFIGINT("Rage>AntiAim>Desync Mode") == 3) {
    side = sw ? -1 : 1;
  } else if (CONFIGINT("Rage>AntiAim>Desync Mode") != 0) {
    static int last_side = -1;
    auto best_ent = getNearestTarget();
    if (best_ent && weapon) {
      QAngle ang{};
      Interfaces::engine->GetViewAngles(ang);

      Vector src, dst, sc1, sc2, fw1;

      src = Globals::localPlayer->getBonePos(8); // head
      QAngle ang1 = QAngle(0, ang.y - 90.f, 0);
      angleVectors(ang1, fw1);

      Vector left_side = src + (fw1 * 40);
      Vector right_side = src - (fw1 * 40);

      side = last_side;

      float damage_left = Features::RageBot::getDamageDeal(best_ent, left_side, weapon, false);
      float damage_right = Features::RageBot::getDamageDeal(best_ent, right_side, weapon, false);

      if (abs(damage_left - damage_right) > 10) {
        if (damage_left > damage_right)
          side = CONFIGINT("Rage>AntiAim>Desync Mode") == 1 ? -1 : 1;
        else if (damage_left < damage_right)
          side = CONFIGINT("Rage>AntiAim>Desync Mode") == 1 ? 1 : -1;

        last_side = side;
      }
    }
  }

  const float desync_amount = CONFIGINT("Rage>AntiAim>Desync");
  const float yaw_amount = CONFIGINT("Rage>AntiAim>Yaw");
  // csgo->should_sidemove = true;

  if (desync_amount != 0) {
    if (!(*Globals::sendPacket)) {
      cmd->viewangles.y += invertAA ? desync_amount : -desync_amount;
    } else {
      cmd->viewangles.y += invertAA ? yaw_amount : -yaw_amount;
    }
  }
  cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);


  if (*Globals::sendPacket) {
    sw = !sw;
  } else {
    // last_angle = cmd->viewangles.y;
  }

  // Weapon *weapon = (Weapon *)Interfaces::entityList->GetClientEntity(
  //     (uintptr_t)Globals::localPlayer->activeWeapon() & 0xFFF);
  // if (weapon) {
  //   if (!(weapon->itemIndex() == ItemIndex::WEAPON_DECOY ||
  //         weapon->itemIndex() == ItemIndex::WEAPON_HEGRENADE ||
  //         weapon->itemIndex() == ItemIndex::WEAPON_FLASHBANG ||
  //         weapon->itemIndex() == ItemIndex::WEAPON_SMOKEGRENADE ||
  //         weapon->itemIndex() == ItemIndex::WEAPON_MOLOTOV ||
  //         weapon->itemIndex() == ItemIndex::WEAPON_INCGRENADE)) {
  //     if (!((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_USE))) {
  //       if (CONFIGINT("Rage>AntiAim>Type")) {
  //         // TODO: for some reason it refuses to desync when looking
  //         // forward???!?!?

  //         int real = 0;
  //         int fake = 0;

  //         switch (CONFIGINT("Rage>AntiAim>Type")) {
  //         case 1: { // Static
  //           real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset");
  //           fake = CONFIGINT("Rage>AntiAim>Static>Desync");
  //           break;
  //         }
  //         case 2: { // Jitter
  //           int jitterAmt =
  //               (((cmd->tick_count %
  //                  CONFIGINT("Rage>AntiAim>Jitter>Jitter Delay")) <
  //                 CONFIGINT("Rage>AntiAim>Jitter>Jitter Delay") / 2)
  //                    ? 1
  //                    : -1) *
  //               (CONFIGINT("Rage>AntiAim>Jitter>Jitter Amount") -
  //                (CONFIGINT("Rage>AntiAim>Jitter>Jitter Amount") / 2));
  //           real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset") +
  //                  jitterAmt;
  //           fake = CONFIGINT("Rage>AntiAim>Jitter>Desync");
  //           break;
  //         }
  //         case 3: { // Fake Jitter
  //           int jitterAmt =
  //               (((cmd->tick_count %
  //                  CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Delay")) <
  //                 CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Delay") / 2)
  //                    ? 1
  //                    : -1) *
  //               (CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Amount") -
  //                (CONFIGINT("Rage>AntiAim>Fake Jitter>Jitter Amount") / 2));
  //           real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset");
  //           fake = jitterAmt;
  //           break;
  //         }
  //         case 4: { // Real Jitter
  //           int jitterAmt =
  //               (((cmd->tick_count %
  //                  CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Delay")) <
  //                 CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Delay") / 2)
  //                    ? 1
  //                    : -1) *
  //               (CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Amount") -
  //                (CONFIGINT("Rage>AntiAim>Real Jitter>Jitter Amount") / 2));
  //           real = cmd->viewangles.y + CONFIGINT("Rage>AntiAim>Offset") +
  //                  jitterAmt;
  //           fake = CONFIGINT("Rage>AntiAim>Static>Desync") - jitterAmt;
  //           break;
  //         }
  //         case 5: { // Spin
  //           real = cmd->viewangles.y +
  //                  (cmd->tick_count * CONFIGINT("Rage>AntiAim>Offset"));
  //           fake = CONFIGINT("Rage>AntiAim>Spin>Desync");
  //           break;
  //         }
  //         case 6: { // Legit
  //           float delta = Globals::localPlayer->getMaxDesyncAngle() * 2;
  //           static bool invert = true;
  //           auto realtime = Interfaces::globals->realtime;
  //           static float lastTime{0.0f};

  //           if (realtime - lastTime >
  //               CONFIGINT("Rage>AntiAim>FakeLag") / 0.125) {
  //             invert = !invert;
  //             lastTime = realtime;
  //             // *Globals::sendPacket = false;
  //           }

  //           if (updatingLby()) {
  //             Notifications::addNotification(ImColor(255, 0, 0), "LBY");
  //             *Globals::sendPacket = false;
  //             invert ? cmd->viewangles.y -= delta
  //                    : cmd->viewangles.y += delta;
  //           }

  //           delta = Globals::localPlayer->getMaxDesyncAngle();

  //           if (Globals::sendPacket) {
  //             invert ? cmd->viewangles.y += delta
  //                    : cmd->viewangles.y -= delta;
  //           } else {
  //             cmd->viewangles.y += 180.f;
  //           }

  //           // normalizeAngles(cmd->viewangles);

  //           cmd->buttons &=
  //               ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);

  //           return;
  //         }
  //         }

  //         // TODO Check for net channel group 9 so we can desync and yell at
  //         // nn's at the same time
  //         int fakelag = CONFIGINT("Rage>AntiAim>FakeLag");
  //         if (Interfaces::engine->IsVoiceRecording()) {
  //           fakelag = 0;
  //         } else {
  //           fakelag = CONFIGINT("Rage>AntiAim>FakeLag");
  //         }
  //         cmd->viewangles.x = CONFIGINT("Rage>AntiAim>Pitch");

  //         *Globals::sendPacket =
  //             cmd->tick_count %
  //             (CONFIGINT("Rage>AntiAim>Type") + fakelag + 1);

  //         if (updatingLby()) {
  //           cmd->viewangles.y = real + (fake * 2);
  //           *Globals::sendPacket = false;
  //         } else if (*Globals::sendPacket) {
  //           cmd->viewangles.y = real;
  //           fakeYaw = real;
  //         }
  //       }
  //     }
  //   }
  // }
}