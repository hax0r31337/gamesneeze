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
  cmd->viewangles.z = CONFIGINT("Rage>AntiAim>Roll");

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
}