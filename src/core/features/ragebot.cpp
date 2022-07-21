#include "../../includes.hpp"
#include "features.hpp"
#include <algorithm>
#include <cmath>


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

  activeWeapon->UpdateAccuracyPenalty();
  float weap_spread = activeWeapon->GetSpread();
  float weap_inaccuracy = activeWeapon->GetInaccuracy();

  for (int i = 0; i < 100; i++) {
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

    // TraceFilter filter;
    // filter.pSkip = Globals::localPlayer;
    // //   hitbox  |  monster  | solid
    // Interfaces::trace->TraceRay(ray, (0x40000000 | 0x40000 | 0x1), &filter, &tr);

    if (tr.m_pEntityHit == enemy) {
      hitCount++;
    }

    if (hitCount >= hitChance) {
      // Notifications::addNotification(ImColor(255, 255, 255), "HITS OK");
      return true;
    } else if (100 - i + hitCount < hitChance) {
      return false;
    }
  }

  return false;
}

void Features::RageBot::bestHeadPoint(Player *player,
                                      int &Damage, Vector &Spot,
                                      float headScale, WeaponInfo *weaponData,
                                      bool friendlyFire) {
  matrix3x4_t matrix[128];
  if (!player->setupBones(matrix, 128, 0x100, 0.f))
    return;
  model_t *pModel = player->model();
  if (!pModel)
    return;

  studiohdr_t *hdr = Interfaces::modelInfo->GetStudioModel(pModel);
  if (!hdr)
    return;
  mstudiobbox_t *bbox = hdr->pHitbox((int)HitboxModel::HITBOX_HEAD, 0);
  if (!bbox)
    return;

  Vector mins, maxs;
  vectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
  vectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
  auto local = Globals::localPlayer;

  Vector center = (mins + maxs) * 0.5f;
  static Vector points[7] = {center, center, center, center,
                             center, center, center};
  // 0 - center, 1 - skullcap, 3 - upperbackofhead
  // 4 - leftear, 5 - rightear, 6 - backofhead
  for (int i = 0; i < 7;
       i++) // set all points initially to center mass of head.
    points[i] = center;
  float scale = headScale;
  auto final_radius = bbox->radius * scale;

  auto pitch_down = normalizePitch(player->eyeAngles().x) > 85.0f;
  auto originY = local->origin().y;
  float stuff = calcAngle(player->eyePos(), Vector{originY, originY, originY}).y;
  auto backward = fabs(player->eyeAngles().y - stuff) > 120.0f;

  points[1] = Vector(bbox->bbmax.x + 0.70710678f * final_radius,
                     bbox->bbmax.y - 0.70710678f * final_radius, bbox->bbmax.z);
  points[3] =
      Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius);
  points[4] =
      Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius);
  points[5] =
      Vector(bbox->bbmax.x, bbox->bbmax.y - final_radius, bbox->bbmax.z);

  if (pitch_down && backward)
    points[6] =
        Vector(bbox->bbmax.x - final_radius, bbox->bbmax.y, bbox->bbmax.z);

  for (int i = 0; i < 7; i++) {
    float bestDamage = getDamageDeal(player, points[i], weaponData, friendlyFire);
    if (bestDamage >= player->health()) {
      Damage = bestDamage;
      Spot = points[i];
      return;
    } else if (bestDamage > Damage) {
      Damage = bestDamage;
      Spot = points[i];
    }
  }
}

void Features::RageBot::bestMultiPoint(Player *player, int &BoneIndex,
                                        int &Damage, Vector &Spot, float bodyScale,
                                        WeaponInfo *weaponData, bool friendlyFire) {
  model_t *pModel = player->model();
  if (!pModel)
    return;
  studiohdr_t *hdr = Interfaces::modelInfo->GetStudioModel(pModel);
  if (!hdr)
    return;
  mstudiobbox_t *bbox = hdr->pHitbox((int)BoneIndex, 0);
  if (!bbox)
    return;

  matrix3x4_t matrix[128];
  Vector mins, maxs;
  vectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
  vectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
  // 0 - center 1 - left, 2 - right, 3 - back
  Vector center = (mins + maxs) * 0.5f;
  Vector points[4] = {center, center, center, center};
  auto final_radius = bbox->radius * bodyScale;

  points[1] =
      Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius);
  points[2] =
      Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius);
  points[3] = Vector(center.x, bbox->bbmax.y - final_radius, center.z);

  for (int i = 0; i < 4; i++) {
    int bestDamage = getDamageDeal(player, points[i], weaponData, friendlyFire);
    if (bestDamage >= player->health()) {
      Damage = bestDamage;
      Spot = points[i];
      return;
    } else if (bestDamage > Damage) {
      Damage = bestDamage;
      Spot = points[i];
    }
  }
}

void Features::RageBot::applyAutoSlow(CUserCmd *cmd, Weapon *weapon) {
  if (Interfaces::globals->interval_per_tick == FP_NAN ||
      Interfaces::globals->interval_per_tick == 0 ||
      Interfaces::globals->interval_per_tick == FP_INFINITE) 
      return;

  const float maxSpeed =
      (Globals::localPlayer->scoped()
           ? weapon->GetWeaponInfo()->GetMaxPlayerSpeedScoped()
           : weapon->GetWeaponInfo()->GetMaxPlayerSpeed());

  if (Globals::localPlayer->velocity().Length() > maxSpeed / 3) {
    cmd->forwardmove = -cmd->forwardmove;
    cmd->sidemove = -cmd->sidemove;
    cmd->upmove = 0;
    cmd->buttons |= IN_WALK;
  }
}

void Features::RageBot::createMove(CUserCmd *cmd) {
  if (!((Menu::CustomWidgets::isKeyDown(CONFIGINT("Rage>RageBot>Key")) ||
         CONFIGBOOL("Rage>RageBot>Always on")) &&
        Interfaces::engine->IsInGame() && Globals::localPlayer &&
        Globals::localPlayer->health() > 0))
    return;
  if (Globals::localPlayer->moveType() != MOVETYPE_WALK)
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
  float headScale = CONFIGINT("Rage>RageBot>Default>Head Scale") / 100.f;
  float bodyScale = CONFIGINT("Rage>RageBot>Default>Head Scale") / 100.f;
  bool friendlyFire = CONFIGBOOL("Rage>RageBot>Default>Friendly Fire");
  bool ignoreBlind = CONFIGBOOL("Rage>RageBot>Default>Ignore Blind");
  bool ignoreSmoke = CONFIGBOOL("Rage>RageBot>Default>Ignore Smoke");
  bool scopedOnly = CONFIGBOOL("Rage>RageBot>Default>Scoped Only");
  bool autoShot = CONFIGBOOL("Rage>RageBot>Default>Auto Shot");
  bool autoScope = CONFIGBOOL("Rage>RageBot>Default>Auto Scope");
  bool visibleOnly = CONFIGBOOL("Rage>RageBot>Default>Visible Only");
  bool autoSlow = CONFIGBOOL("Rage>RageBot>Default>Auto Slow");
  bool killShot = CONFIGBOOL("Rage>RageBot>Default>Kill Shot");

  bool useableWeapon = false;

  if ((std::find(std::begin(pistols), std::end(pistols),
                 activeWeapon->itemIndex() & 0xFFF) != std::end(pistols))) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>Pistol>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>Pistol>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>Pistol>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>Pistol>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>Pistol>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>Pistol>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>Pistol>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>Pistol>Kill Shot");
    }
  } else if ((std::find(std::begin(heavyPistols), std::end(heavyPistols),
                        activeWeapon->itemIndex() & 0xFFF) != std::end(heavyPistols))) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>Heavy Pistol>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>Heavy Pistol>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>Heavy Pistol>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>Heavy Pistol>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>Heavy Pistol>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>Heavy Pistol>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>Heavy Pistol>Kill Shot");
    }
  } else if ((std::find(std::begin(rifles), std::end(rifles),
                        activeWeapon->itemIndex() & 0xFFF) != std::end(rifles))) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>Rifle>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>Rifle>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>Rifle>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>Rifle>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>Rifle>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>Rifle>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>Rifle>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>Rifle>Kill Shot");
    }
  } else if ((std::find(std::begin(smgs), std::end(smgs),
                        activeWeapon->itemIndex() & 0xFFF) != std::end(smgs))) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>SMG>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>SMG>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>SMG>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>SMG>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>SMG>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>SMG>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>SMG>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>SMG>Kill Shot");
    }
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_SSG08)) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>Scout>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>Scout>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>Scout>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>Scout>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>Scout>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>Scout>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>Scout>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>Scout>Kill Shot");
    }
  } else if (((activeWeapon->itemIndex() & 0xFFF) == WEAPON_AWP)) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>AWP>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>AWP>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>AWP>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>AWP>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>AWP>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>AWP>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>AWP>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>AWP>Kill Shot");
    }
  } else if ((std::find(std::begin(heavyWeapons), std::end(heavyWeapons),
                        activeWeapon->itemIndex() & 0xFFF) !=
              std::end(heavyWeapons))) {
    useableWeapon = true;
    if (CONFIGBOOL("Rage>RageBot>Heavy>Override")) {
      hitboxes = CONFIGINT("Rage>RageBot>Heavy>Hitboxes");
      hitChance = CONFIGINT("Rage>RageBot>Heavy>Hit Chance");
      minDamage = CONFIGINT("Rage>RageBot>Heavy>Min Damage") * 1.f;
      headScale = CONFIGINT("Rage>RageBot>Heavy>Head Scale") / 100.f;
      bodyScale = CONFIGINT("Rage>RageBot>Heavy>Head Scale") / 100.f;
      autoSlow = CONFIGBOOL("Rage>RageBot>Heavy>Auto Slow");
      killShot = CONFIGBOOL("Rage>RageBot>Heavy>Kill Shot");
    }
  }

  if (!useableWeapon)
    return;

  if (activeWeapon->nextPrimaryAttack() > Globals::serverTime())
    return;
    
  if (!ignoreBlind && Globals::localPlayer->maxFlashAlpha() > 75.f)
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
      QAngle aimPunch = Globals::localPlayer->aimPunch() * 2;
      bool hasTarget = false;

      // Enumerate over players and get angle to the closest player to crosshair
      for (int i = 1; i < Interfaces::globals->maxClients; i++) {
        Player *p = (Player *)Interfaces::entityList->GetClientEntity(i);
        if (p && p != Globals::localPlayer) {
          if (p->health() > 0 && !p->dormant() && (p->isEnemy() || friendlyFire) && (p->visible() || !visibleOnly)) {
            matrix3x4_t boneMatrix[128];
            if (p->getAnythingBones(boneMatrix)) {
              Vector localPlayerEyePos = Globals::localPlayer->eyePos();

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

                Vector targetBonePos = Vector{0, 0, 0};
                int damageDeal = -1;
                if (bone == 8) {
                  if  (headScale == 0) {
                    targetBonePos = p->getBonePos(bone);
                    damageDeal = getDamageDeal(p, targetBonePos, weapon->GetWeaponInfo(), friendlyFire);
                  } else {
                    bestHeadPoint(p, damageDeal, targetBonePos, headScale,
                                  weapon->GetWeaponInfo(), friendlyFire);
                  }
                } else {
                  if (bodyScale == 0) {
                    targetBonePos = p->getBonePos(bone);
                    damageDeal =
                        getDamageDeal(p, targetBonePos, weapon->GetWeaponInfo(),
                                      friendlyFire);
                  } else {
                    int hitbox = (int) ((1 << i & (int)HitBoxes::HEAD)      ? HitboxModel::HITBOX_HEAD
                             : (1 << i & (int)HitBoxes::NECK)    ? HitboxModel::HITBOX_NECK
                             : (1 << i & (int)HitBoxes::CHEST)   ? HitboxModel::HITBOX_SPINE
                             : (1 << i & (int)HitBoxes::STOMACH) ? HitboxModel::HITBOX_LEGS
                             : (1 << i & (int)HitBoxes::PELVIS)  ? HitboxModel::HITBOX_PELVIS
                                                                 : HitboxModel::HITBOX_SPINE);
                    bestMultiPoint(p, hitbox, damageDeal, targetBonePos,
                                   bodyScale, weapon->GetWeaponInfo(),
                                   friendlyFire);
                  }
                }
                // auto damageDeal = getDamageDeal(p, targetBonePos, weapon->GetWeaponInfo(), friendlyFire);
                if (damageDeal <= 0 || damageDeal < (killShot ? p->health() : (p->health() < minDamage ? p->health() : minDamage))) {
                  continue;
                }

                if (!ignoreSmoke &&
                    Offsets::lineGoesThroughSmoke(
                        Globals::localPlayer->eyePos(), targetBonePos, 1))
                  continue;

                QAngle directAngle = calcAngle(localPlayerEyePos, targetBonePos);
                QAngle angleTarget = directAngle - cmd->viewangles;
                normalizeAngles(angleTarget);

                if (angleTarget.Length() > FOV) {
                  continue;
                }

                if (damageDeal > bestDamage) {
                  if (!canShoot(weapon, &directAngle, p, hitChance)) {
                    hasTarget = true;
                    continue;
                  }
                  bestDamage = damageDeal;
                  bestPlayerAngle = directAngle - aimPunch;
                  bestPlayer = p;
                }
              }
            }
          }
        }
      }
      if (bestDamage > 0 && bestPlayer != nullptr) {
        if (autoSlow) {
          applyAutoSlow(cmd, weapon);
        }
        if (autoScope &&
            activeWeapon->isSniperRifle() && !Globals::localPlayer->scoped()) {
          cmd->buttons |= IN_ATTACK2;
          return;
        }
        if (autoShot &&
            activeWeapon->nextPrimaryAttack() <= Globals::serverTime()) {
          cmd->buttons |= IN_ATTACK;
        }

        cmd->viewangles = bestPlayerAngle;
        normalizeAngles(cmd->viewangles);
      } else if (hasTarget) {
        if (autoSlow) {
          applyAutoSlow(cmd, weapon);
        }
        if (autoScope && activeWeapon->isSniperRifle() &&
            !Globals::localPlayer->scoped()) {
          cmd->buttons |= IN_ATTACK2;
        }
      }
    }
  }
}