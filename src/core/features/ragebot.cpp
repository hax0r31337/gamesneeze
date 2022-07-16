#include "../../includes.hpp"
#include "features.hpp"

int aimTarget = -1;

bool Features::RageBot::traceToExit(const Trace &enterTrace, const Vector &start,
                        const Vector &direction, Vector &end,
                        Trace &exitTrace) {
  float distance = 0.0f;

  while (distance <= 90.0f) {
    distance += 4.0f;
    end = start + direction * distance;

    int point_contents = Interfaces::trace->GetPointContents(
        end, MASK_SHOT_HULL | CONTENTS_HITBOX);

    if (point_contents & MASK_SHOT_HULL &&
        !(point_contents & CONTENTS_HITBOX)) {
      continue;
    }

    auto new_end = end - (direction * 4.0f);

    auto ray = Ray{};
    ray.Init(end, new_end);

    Interfaces::trace->TraceRay(ray, MASK_SHOT, 0, &exitTrace);

    if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX) {
      ray.Init(end, start);
      TraceFilter tf;
      tf.pSkip = exitTrace.m_pEntityHit;
      Interfaces::trace->TraceRay(ray, 0x600400B, &tf, &exitTrace);

      if ((exitTrace.fraction < 1.0f || exitTrace.allsolid) &&
          !exitTrace.startsolid) {
        end = exitTrace.endpos;
        return true;
      }

      continue;
    }

    if (!(exitTrace.fraction < 1.0 || exitTrace.allsolid ||
          exitTrace.startsolid) ||
        exitTrace.startsolid) {
      if (exitTrace.m_pEntityHit) {
        if (enterTrace.m_pEntityHit &&
            (uintptr_t)enterTrace.m_pEntityHit ==
                Interfaces::entityList->GetClientEntity(aimTarget)) {
          return true;
        }
      }

      continue;
    }

    if (exitTrace.surface.flags >> 7 & 1 &&
        !(enterTrace.surface.flags >> 7 & 1)) {
      continue;
    }

    if (exitTrace.plane.normal.dotProduct(direction) <= 1.0f) {
      auto fraction = exitTrace.fraction * 4.0f;
      end = end - (direction * fraction);

      return true;
    }
  }

  return false;
}

float Features::RageBot::handleBulletPenetration(SurfaceData *enterSurfaceData,
                                     const Trace &enterTrace,
                                     const Vector &direction, Vector &result,
                                     float penetration, float damage) {
  Vector end;
  Trace exitTrace;

  if (!traceToExit(enterTrace, enterTrace.endpos, direction, end, exitTrace))
    return -1.0f;

  SurfaceData *exitSurfaceData =
      Interfaces::physicsSurfaceProps->GetSurfaceData(
          exitTrace.surface.surfaceProps);

  float damageModifier = 0.16f;
  float penetrationModifier = (enterSurfaceData->game.flPenetrationModifier +
                               exitSurfaceData->game.flPenetrationModifier) /
                              2.0f;

  if (enterSurfaceData->game.material == 71 || enterSurfaceData->game.material == 89) {
    damageModifier = 0.05f;
    penetrationModifier = 3.0f;
  } else if (enterTrace.contents >> 3 & 1 ||
             enterTrace.surface.flags >> 7 & 1) {
    penetrationModifier = 1.0f;
  }

  if (enterSurfaceData->game.material == exitSurfaceData->game.material) {
    if (exitSurfaceData->game.material == 85 || exitSurfaceData->game.material == 87)
      penetrationModifier = 3.0f;
    else if (exitSurfaceData->game.material == 76)
      penetrationModifier = 2.0f;
  }

  damage -= 11.25f / penetration / penetrationModifier +
            damage * damageModifier +
            (exitTrace.endpos - enterTrace.endpos).squareLength() / 24.0f /
                penetrationModifier;

  result = exitTrace.endpos;
  return damage;
}

bool Features::RageBot::canScan(Entity *entity, const Vector &destination,
                    WeaponInfo *weaponData, int minDamage,
                    bool allowFriendlyFire) {
  if (!Globals::localPlayer)
    return false;

  float damage{static_cast<float>(weaponData->GetDamage())};

  Vector start{Globals::localPlayer->eyePos()};
  Vector direction{destination - start};
  direction /= direction.length();

  int hitsLeft = 4;

  while (damage >= 1.0f && hitsLeft > 0) {
    Trace trace;
    Ray ray;
    ray.Init(start, destination);
    TraceFilter traceFilter;
    traceFilter.pSkip = Globals::localPlayer;
    Interfaces::trace->TraceRay(ray, 0x4600400B, &traceFilter, &trace);

    Features::Notifications::addNotification(ImColor(255, 255, 255),
                                                   "TRACE");

    if (!allowFriendlyFire && trace.m_pEntityHit &&
        trace.m_pEntityHit->isPlayer() && !trace.m_pEntityHit->isEnemy()) {
      return false;
    }

    if (trace.fraction == 1.0f) {
      return false;
    }

    if (trace.m_pEntityHit == entity &&
        trace.hitgroup > HitGroups::HITGROUP_GENERIC &&
        trace.hitgroup <= HitGroups::HITGROUP_RIGHTLEG) {
      damage = HitGroupsHelper::getDamageMultiplier(trace.hitgroup) * damage *
               std::pow(weaponData->GetRangeModifier(),
                        trace.fraction * weaponData->GetRange() / 500.0f);

      if (float armorRatio{weaponData->GetWeaponArmorRatio() / 2.0f};
          HitGroupsHelper::isArmored(trace.hitgroup,
                                     trace.m_pEntityHit->helmet()))
        damage -= (trace.m_pEntityHit->armor() < damage * armorRatio / 2.0f
                       ? trace.m_pEntityHit->armor() * 4.0f
                       : damage) *
                  (1.0f - armorRatio);

      return damage >= minDamage;
    }
    const auto surfaceData = Interfaces::physicsSurfaceProps->GetSurfaceData(
        trace.surface.surfaceProps);

    if (surfaceData->game.flPenetrationModifier < 0.1f) {
      break;
    }

    damage = handleBulletPenetration(surfaceData, trace, direction, start,
                                     weaponData->GetPenetration(), damage);
    hitsLeft--;
  }
  return false;
}

void Features::RageBot::createMove(CUserCmd *cmd) {
  if (!CONFIGBOOL("Rage>Enabled"))
    return;
  if (!((Menu::CustomWidgets::isKeyDown(CONFIGINT("Rage>RageBot>Key")) ||
         CONFIGBOOL("Rage>RageBot>Always on")) &&
        Interfaces::engine->IsInGame() && Globals::localPlayer &&
        Globals::localPlayer->health() > 0))
    return;

  const auto activeWeapon = (Weapon *)Globals::localPlayer->GetWeapon();
  if (!activeWeapon || !activeWeapon->clip())
    return;

  if (Globals::localPlayer->shotsFired() > 0 && !activeWeapon->GetWeaponInfo()->GetFullAuto())
    return;

  int hitboxes = CONFIGINT("Rage>RageBot>Default>Hitboxes");
  float smoothing = 1.f + (CONFIGINT("Rage>RageBot>Default>Smoothing") / 5.f);
  float FOV = CONFIGINT("Rage>RageBot>Default>FOV") / 10.f;
  float maxShotInaccuracy =
      CONFIGINT("Rage>RageBot>Default>Max Shot Inaccuracy") * 1.f;
  float maxAimInaccuracy =
      CONFIGINT("Rage>RageBot>Default>Max Aim Inaccuracy") * 1.f;
  float minDamage = CONFIGINT("Rage>RageBot>Default>Min Damage") * 1.f;
  bool friendlyFire = CONFIGBOOL("Rage>RageBot>Default>Friendly Fire");
  bool aimWhileBlind = CONFIGBOOL("Rage>RageBot>Default>Ignore Blind");
  bool ignoreSmoke = CONFIGBOOL("Rage>RageBot>Default>Ignore Smoke");
  bool scopedOnly = CONFIGBOOL("Rage>RageBot>Default>Scoped Only");
  bool autoShot = CONFIGBOOL("Rage>RageBot>Default>Auto Shot");
  bool autoScope = CONFIGBOOL("Rage>RageBot>Default>Auto Scope");
  bool visibleOnly = CONFIGBOOL("Rage>RageBot>Default>Visible Only");
  bool killShot = CONFIGBOOL("Rage>RageBot>Default>Kill Shot");

  if ((std::find(std::begin(pistols), std::end(pistols),
                 activeWeapon->itemIndex() & 0xFFF) != std::end(pistols)) &&
      CONFIGBOOL("Rage>RageBot>Pistol>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Pistol>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>Pistol>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>Pistol>FOV") / 10.f;
    maxShotInaccuracy =
        CONFIGINT("Rage>RageBot>Pistol>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy =
        CONFIGINT("Rage>RageBot>Pistol>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>Pistol>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>Pistol>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>Pistol>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>Pistol>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>Pistol>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>Pistol>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>Pistol>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>Pistol>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>Pistol>Kill Shot");
  } else if ((std::find(std::begin(heavyPistols), std::end(heavyPistols),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(heavyPistols)) &&
             CONFIGBOOL("Rage>RageBot>Heavy Pistol>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Heavy Pistol>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>Heavy Pistol>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>Heavy Pistol>FOV") / 10.f;
    maxShotInaccuracy =
        CONFIGINT("Rage>RageBot>Heavy Pistol>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>Heavy>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>Heavy Pistol>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Kill Shot");
  } else if ((std::find(std::begin(rifles), std::end(rifles),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(rifles)) &&
             CONFIGBOOL("Rage>RageBot>Rifle>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Rifle>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>Rifle>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>Rifle>FOV") / 10.f;
    maxShotInaccuracy =
        CONFIGINT("Rage>RageBot>Rifle>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>Rifle>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>Rifle>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>Rifle>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>Rifle>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>Rifle>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>Rifle>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>Rifle>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>Rifle>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>Rifle>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>Rifle>Kill Shot");
  } else if ((std::find(std::begin(smgs), std::end(smgs),
                        activeWeapon->itemIndex() & 0xFFF) != std::end(smgs)) &&
             CONFIGBOOL("Rage>RageBot>SMG>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>SMG>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>SMG>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>SMG>FOV") / 10.f;
    maxShotInaccuracy = CONFIGINT("Rage>RageBot>SMG>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>SMG>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>SMG>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>SMG>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>SMG>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>SMG>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>SMG>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>SMG>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>SMG>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>SMG>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>SMG>Kill Shot");
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_SSG08) &&
             CONFIGBOOL("Rage>RageBot>Scout>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Scout>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>Scout>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>Scout>FOV") / 10.f;
    maxShotInaccuracy =
        CONFIGINT("Rage>RageBot>Scout>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>Scout>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>Scout>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>Scout>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>Scout>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>Scout>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>Scout>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>Scout>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>Scout>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>Scout>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>Scout>Kill Shot");
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_AWP) &&
             CONFIGBOOL("Rage>RageBot>AWP>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>AWP>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>AWP>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>AWP>FOV") / 10.f;
    maxShotInaccuracy = CONFIGINT("Rage>RageBot>AWP>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>AWP>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>AWP>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>AWP>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>AWP>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>AWP>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>AWP>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>AWP>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>AWP>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>AWP>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>AWP>Kill Shot");
  } else if ((std::find(std::begin(heavyWeapons), std::end(heavyWeapons),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(heavyWeapons)) &&
             CONFIGBOOL("Rage>RageBot>Heavy>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Heavy>Hitboxes");
    smoothing = 1.f + (CONFIGINT("Rage>RageBot>Heavy>Smoothing") / 5.f);
    FOV = CONFIGINT("Rage>RageBot>Heavy>FOV") / 10.f;
    maxShotInaccuracy =
        CONFIGINT("Rage>RageBot>Heavy>Max Shot Inaccuracy") * 1.f;
    maxAimInaccuracy = CONFIGINT("Rage>RageBot>Heavy>Max Aim Inaccuracy") * 1.f;
    minDamage = CONFIGINT("Rage>RageBot>Heavy>Min Damage") * 1.f;
    friendlyFire = CONFIGBOOL("Rage>RageBot>Heavy>Friendly Fire");
    aimWhileBlind = CONFIGBOOL("Rage>RageBot>Heavy>Ignore Blind");
    ignoreSmoke = CONFIGBOOL("Rage>RageBot>Heavy>Ignore Smoke");
    scopedOnly = CONFIGBOOL("Rage>RageBot>Heavy>Scoped Only");
    autoShot = CONFIGBOOL("Rage>RageBot>Heavy>Auto Shot");
    autoScope = CONFIGBOOL("Rage>RageBot>Heavy>Auto Scope");
    visibleOnly = CONFIGBOOL("Rage>RageBot>Heavy>Visible Only");
    killShot = CONFIGBOOL("Rage>RageBot>Heavy>Kill Shot");
  }

  if (!aimWhileBlind && Globals::localPlayer->maxFlashAlpha() > 75.f)
    return;

  if ((cmd->buttons & (1 << 0) || autoShot) &&
      activeWeapon->GetInaccuracy() <= maxAimInaccuracy) {

    if (scopedOnly && activeWeapon->isSniperRifle() &&
        !Globals::localPlayer->scoped())
      return;

    Vector bestTarget{};
    const auto eyePos = Globals::localPlayer->eyePos();

    const auto aimPunch = Globals::localPlayer->aimPunch() * 2;

    for (int i = 1; i <= Interfaces::globals->maxClients; i++) {
      Player *entity = (Player *)Interfaces::entityList->GetClientEntity(i);
      if (!entity || i == Globals::localPlayer->index() || entity->dormant() ||
          entity->health() <= 0 || (!entity->isEnemy() && !friendlyFire) ||
          entity->gunGameImmunity())
        continue;

      for (int i = 0; i < 5; i++) {
        if (!(hitboxes & 1 << i))
          continue;
        // map hitboxes enum to "actual" hitboxes
        int bone = (1 << i & (int)HitBoxes::HEAD)      ? 8
                   : (1 << i & (int)HitBoxes::NECK)    ? 7
                   : (1 << i & (int)HitBoxes::CHEST)   ? 6
                   : (1 << i & (int)HitBoxes::STOMACH) ? 5
                   : (1 << i & (int)HitBoxes::PELVIS)  ? 3
                                                       : 5; // fallback
        auto bonePosition = entity->getBonePos(bone);
        auto angle = calcAngle(eyePos, bonePosition) - cmd->viewangles - aimPunch;
        normalizeAngles(angle);

        const auto fov = angle.Length();
        if (fov > FOV)
          continue;

        if (!ignoreSmoke &&
            Offsets::lineGoesThroughSmoke(Globals::localPlayer->eyePos(),
                                          bonePosition, 1))
          continue;

        if ((!entity->visible() && visibleOnly) ||
            !canScan(entity, bonePosition, activeWeapon->GetWeaponInfo(),
                     killShot ? entity->health() : minDamage, friendlyFire))
          continue;

        if (fov < FOV) {
          FOV = fov;
          bestTarget = bonePosition;
          aimTarget = i;
          // Features::Notifications::addNotification(ImColor(255, 255, 255),
          //                                          "SELECTED BONE");
        }
      }
    }

    if (bestTarget.notNull() && bestTarget.x != 0 && bestTarget.y != 0 && bestTarget.z != 0) {
      static QAngle lastAngles{cmd->viewangles};
      static int lastCommand{};

      if (lastCommand == cmd->command_number - 1 && lastAngles.notNull())
        cmd->viewangles = lastAngles;

      auto angle = calcAngle(Globals::localPlayer->eyePos(), bestTarget) - cmd->viewangles - aimPunch;
      normalizeAngles(angle);
      bool clamped{false};

      if (std::abs(angle.x) > 255.f || std::abs(angle.y) > 255.f) {
        angle.x = std::clamp(angle.x, -255.f, 255.f);
        angle.y = std::clamp(angle.y, -255.f, 255.f);
        clamped = true;
      }

      angle /= smoothing;
      cmd->viewangles += angle;

      if (autoScope &&
          activeWeapon->nextPrimaryAttack() <=
              Globals::serverTime() &&
          activeWeapon->isSniperRifle() && !Globals::localPlayer->scoped()) {
        cmd->buttons |= IN_ATTACK2;
        return;
      }

      if (autoShot &&
          activeWeapon->nextPrimaryAttack() <=
              Globals::serverTime() &&
          !clamped && activeWeapon->GetInaccuracy() <= maxShotInaccuracy)
        cmd->buttons |= IN_ATTACK;

      if (clamped)
        cmd->buttons &= ~IN_ATTACK;

      if (clamped || smoothing > 1.0f)
        lastAngles = cmd->viewangles;
      else
        lastAngles = QAngle{};

      lastCommand = cmd->command_number;
    }
  }
}
