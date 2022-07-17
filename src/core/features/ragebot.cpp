#include "../../includes.hpp"
#include "features.hpp"


bool Features::RageBot::traceToExit(const Trace &enterTrace,
                                    const Vector &start,
                                    const Vector &direction, Vector &end,
                                    Trace &exitTrace, Entity *target) {
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
        if (enterTrace.m_pEntityHit && enterTrace.m_pEntityHit == target) {
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

float Features::RageBot::handleBulletPenetration(
    SurfaceData *enterSurfaceData, const Trace &enterTrace,
    const Vector &direction, Vector &result, float penetration, float damage, Entity *target) {
  Vector end;
  Trace exitTrace;

  if (!traceToExit(enterTrace, enterTrace.endpos, direction, end, exitTrace, target))
    return -1.0f;

  SurfaceData *exitSurfaceData =
      Interfaces::physicsSurfaceProps->GetSurfaceData(
          exitTrace.surface.surfaceProps);

  float damageModifier = 0.16f;
  float penetrationModifier = (enterSurfaceData->game.flPenetrationModifier +
                               exitSurfaceData->game.flPenetrationModifier) /
                              2.0f;

  if (enterSurfaceData->game.material == 71 ||
      enterSurfaceData->game.material == 89) {
    damageModifier = 0.05f;
    penetrationModifier = 3.0f;
  } else if (enterTrace.contents >> 3 & 1 ||
             enterTrace.surface.flags >> 7 & 1) {
    penetrationModifier = 1.0f;
  }

  if (enterSurfaceData->game.material == exitSurfaceData->game.material) {
    if (exitSurfaceData->game.material == 85 ||
        exitSurfaceData->game.material == 87)
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

int Features::RageBot::getDamageDeal(Player *entity, const Vector &destination,
                                     WeaponInfo *weaponData, bool allowFriendlyFire) {
  if (!Globals::localPlayer)
    return -1;

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

    if (!allowFriendlyFire && trace.m_pEntityHit &&
        trace.m_pEntityHit->isPlayer() && !trace.m_pEntityHit->isEnemy()) {
      return -1;
    }

    if (trace.fraction == 1.0f) {
      return -1;
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

      return damage;
    }
    const auto surfaceData = Interfaces::physicsSurfaceProps->GetSurfaceData(
        trace.surface.surfaceProps);

    if (surfaceData->game.flPenetrationModifier < 0.1f) {
      break;
    }

    damage = handleBulletPenetration(surfaceData, trace, direction, start,
                                     weaponData->GetPenetration(), damage, entity);
    hitsLeft--;
  }
  return 0;
}

bool Features::RageBot::canShoot(Weapon *activeWeapon, QAngle *angle, 
              Player *enemy, int hitChance) {
  if (hitChance == 0)
    return true;

  Vector src = Globals::localPlayer->eyePos();
  Vector forward, right, up;
  angleVectors(*angle, &forward, &right, &up);

  int hitCount = 0;
  int NeededHits =
      static_cast<int>(255.f * (hitChance / 100.f));

  activeWeapon->UpdateAccuracyPenalty();
  float weap_spread = activeWeapon->GetSpread();
  float weap_inaccuracy = activeWeapon->GetInaccuracy();

  for (int i = 0; i < 255; i++) {
    static float val1 = (2.0 * M_PI);

    double b = randFloat(0.f, val1);
    double spread = weap_spread * randFloat(0.f, 1.0f);
    double d = randFloat(0.f, 1.0f);
    double inaccuracy = weap_inaccuracy * randFloat(0.f, 1.0f);

    Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread),
                      (sin(b) * inaccuracy) + (sin(d) * spread), 0),
        direction;

    direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
    direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
    direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
    direction.Normalize();

    QAngle viewAnglesSpread;
    vectorAngles(direction, up, viewAnglesSpread);
    normalizeAngles(viewAnglesSpread);

    Vector viewForward;
    angleVectors(viewAnglesSpread, viewForward);
    viewForward.NormalizeInPlace();

    viewForward =
        src + (viewForward * activeWeapon->GetWeaponInfo()->GetRange());

    Trace tr;
    Ray ray;
    ray.Init(src, viewForward);
    Interfaces::trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, enemy, &tr);

    // TraceFilter traceFilter;
    // traceFilter.pSkip = Globals::localPlayer;
    // Interfaces::trace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

    if (tr.m_pEntityHit == enemy) {
      // Notifications::addNotification(ImColor(255, 255, 255), "HITS++");
      hitCount++;
    }

    if (static_cast<int>((hitCount / 255.f) * 100.f) >= hitChance) {
      // Notifications::addNotification(ImColor(255, 255, 255), "HITS OK");
      return true;
    }

    if ((255 - i + hitCount) < NeededHits) {
      // Notifications::addNotification(ImColor(255, 255, 255), "FAILED HITS");
      return false;
    }
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
  float FOV = CONFIGINT("Rage>RageBot>Default>FOV") / 10.f;
  int hitChance = CONFIGINT("Rage>RageBot>Default>Hit Chance");
  float minDamage = CONFIGINT("Rage>RageBot>Default>Min Damage") * 1.f;
  bool friendlyFire = CONFIGBOOL("Rage>RageBot>Default>Friendly Fire");
  bool aimWhileBlind = CONFIGBOOL("Rage>RageBot>Default>Ignore Blind");
  bool ignoreSmoke = CONFIGBOOL("Rage>RageBot>Default>Ignore Smoke");
  bool scopedOnly = CONFIGBOOL("Rage>RageBot>Default>Scoped Only");
  bool autoShot = CONFIGBOOL("Rage>RageBot>Default>Auto Shot");
  bool autoScope = CONFIGBOOL("Rage>RageBot>Default>Auto Scope");
  bool visibleOnly = CONFIGBOOL("Rage>RageBot>Default>Visible Only");
  bool autoSlow = CONFIGBOOL("Rage>RageBot>Default>Auto Slow");
  bool killShot = CONFIGBOOL("Rage>RageBot>Default>Kill Shot");

  if ((std::find(std::begin(pistols), std::end(pistols),
                 activeWeapon->itemIndex() & 0xFFF) != std::end(pistols)) &&
      CONFIGBOOL("Rage>RageBot>Pistol>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Pistol>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>Pistol>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>Pistol>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>Pistol>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>Pistol>Kill Shot");
  } else if ((std::find(std::begin(heavyPistols), std::end(heavyPistols),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(heavyPistols)) &&
             CONFIGBOOL("Rage>RageBot>Heavy Pistol>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Heavy Pistol>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>Heavy Pistol>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>Heavy Pistol>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Kill Shot");
  } else if ((std::find(std::begin(rifles), std::end(rifles),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(rifles)) &&
             CONFIGBOOL("Rage>RageBot>Rifle>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Rifle>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>Rifle>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>Rifle>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>Rifle>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>Rifle>Kill Shot");
  } else if ((std::find(std::begin(smgs), std::end(smgs),
                        activeWeapon->itemIndex() & 0xFFF) != std::end(smgs)) &&
             CONFIGBOOL("Rage>RageBot>SMG>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>SMG>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>SMG>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>SMG>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>SMG>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>SMG>Kill Shot");
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_SSG08) &&
             CONFIGBOOL("Rage>RageBot>Scout>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Scout>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>Scout>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>Scout>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>Scout>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>Scout>Kill Shot");
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_AWP) &&
             CONFIGBOOL("Rage>RageBot>AWP>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>AWP>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>AWP>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>AWP>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>AWP>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>AWP>Kill Shot");
  } else if ((std::find(std::begin(heavyWeapons), std::end(heavyWeapons),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(heavyWeapons)) &&
             CONFIGBOOL("Rage>RageBot>Heavy>Override")) {
    hitboxes = CONFIGINT("Rage>RageBot>Heavy>Hitboxes");
    hitChance = CONFIGINT("Rage>RageBot>Heavy>Hit Chance");
    minDamage = CONFIGINT("Rage>RageBot>Heavy>Min Damage") * 1.f;
    autoSlow = CONFIGBOOL("Rage>RageBot>Heavy>Auto Slow");
    killShot = CONFIGBOOL("Rage>RageBot>Heavy>Kill Shot");
  }

  if (!aimWhileBlind && Globals::localPlayer->maxFlashAlpha() > 75.f)
    return;

  if (cmd->buttons & (1 << 0) || autoShot) {
    if (scopedOnly && activeWeapon->isSniperRifle() &&
        !Globals::localPlayer->scoped())
      return;

    Weapon *weapon = (Weapon *)Interfaces::entityList->GetClientEntity(
        (uintptr_t)Globals::localPlayer->activeWeapon() &
        0xFFF); // GetClientEntityFromHandle is being gay
    if (weapon) {
      int bestDamage = -1;
      QAngle bestPlayerAngle = {0, 0, 0};
      Player *bestPlayer = nullptr;
      QAngle aimPunch = weapon->requiresRecoilControl() ?  Globals::localPlayer->aimPunch() * 2 : QAngle{0, 0, 0};

      // Enumerate over players and get angle to the closest player to crosshair
      for (int i = 1; i < Interfaces::globals->maxClients; i++) {
        Player *p = (Player *)Interfaces::entityList->GetClientEntity(i);
        if (p && p != Globals::localPlayer) {
          if (p->health() > 0 && !p->dormant() && (p->isEnemy() || friendlyFire) && (p->visible() || !visibleOnly)) {
            matrix3x4_t boneMatrix[128];
            if (p->getAnythingBones(boneMatrix)) {
              Vector localPlayerEyePos = Globals::localPlayer->eyePos();
              Vector targetBonePos;

              // TODO check which bone would be exposed sooner with engine
              // prediction and which would do more damage.
              for (int i = 0; i < 5; i++) {
                if (!(hitboxes & 1 << i))
                  continue;
                // map hitboxes enum to "actual" hitboxes
                int bone = (1 << i & (int)HitBoxes::HEAD)      ? 8
                           : (1 << i & (int)HitBoxes::NECK)    ? 7
                           : (1 << i & (int)HitBoxes::CHEST)   ? 6
                           : (1 << i & (int)HitBoxes::STOMACH) ? 5
                           : (1 << i & (int)HitBoxes::PELVIS)  ? 3
                                                               : 5;
                targetBonePos = p->getBonePos(bone);
                // if (!canScan(p, targetBonePos, weapon->GetWeaponInfo(),
                //              killShot ? p->health() : minDamage,
                //              friendlyFire)) {
                //               continue;
                // }

                if (!ignoreSmoke &&
                    Offsets::lineGoesThroughSmoke(
                        Globals::localPlayer->eyePos(), targetBonePos, 1))
                  continue;

                auto damageDeal = getDamageDeal(p, targetBonePos, weapon->GetWeaponInfo(), friendlyFire);
                if (damageDeal <= 0 || damageDeal < (killShot ? p->health() : minDamage)) {
                  continue;
                }

                QAngle angle = calcAngle(localPlayerEyePos, targetBonePos) - aimPunch;
                QAngle angleTarget = angle - cmd->viewangles;
                normalizeAngles(angle);
                normalizeAngles(angleTarget);

                if (angleTarget.Length() > FOV) {
                  continue;
                }

                if (damageDeal > bestDamage) {
                  bestDamage = damageDeal;
                  bestPlayerAngle = angle;
                  bestPlayer = p;
                }
              }
            }
          }
        }
      }
      if (bestDamage > 0 && bestPlayer != nullptr) {
        if (autoScope &&
            activeWeapon->nextPrimaryAttack() <= Globals::serverTime() &&
            activeWeapon->isSniperRifle() && !Globals::localPlayer->scoped()) {
          cmd->buttons |= IN_ATTACK2;
          return;
        }
        if (!canShoot(weapon, &bestPlayerAngle, bestPlayer, hitChance)) {
          return;
        }
        if (autoShot &&
            activeWeapon->nextPrimaryAttack() <= Globals::serverTime()) {
          cmd->buttons |= IN_ATTACK;
        }

        QAngle angleTarget = bestPlayerAngle - cmd->viewangles;
        normalizeAngles(angleTarget);
        cmd->viewangles += angleTarget;
      }
    }
  }
}