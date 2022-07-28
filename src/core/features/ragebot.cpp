#include "../../includes.hpp"
#include "features.hpp"
#include <algorithm>
#include <cmath>

void Features::RageBot::scaleDamage(HitGroups hitgroup, Player *enemy,
                 float weapon_armor_ratio, float &current_damage) {
  current_damage *= HitGroupsHelper::getDamageMultiplier(hitgroup);
  int ArmorValue = enemy->armor();
  if (ArmorValue > 0) {
    float Damage = current_damage;
    float v47 = 1.f, ArmorBonusRatio = 0.5f,
          ArmorRatio = weapon_armor_ratio * 0.5f;
    auto NewDamage = Damage * ArmorRatio;

    if (((Damage - (Damage * ArmorRatio)) * (v47 * ArmorBonusRatio)) >
        ArmorValue) {
      NewDamage = Damage - (ArmorValue / ArmorBonusRatio);
    }

    current_damage = NewDamage;
    if (hitgroup == HitGroups::HITGROUP_HEAD) {
      if (enemy->helmet())
        current_damage *= weapon_armor_ratio * 0.5f;
    } else
      current_damage *= weapon_armor_ratio * 0.5f;
  }
}

bool Features::RageBot::traceToExit(Vector &end, Trace *enter_trace, Vector start, Vector dir,
                 Trace *exit_trace) {
  float distance = 0.0f;

  while (distance <= 90.0f) {
    distance += 4.0f;
    end = start + dir * distance;

    auto point_contents =
        Interfaces::trace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX);

    if (point_contents & MASK_SHOT_HULL && !(point_contents & CONTENTS_HITBOX))
      continue;

    auto new_end = end - (dir * 4.0f);

    Ray ray;
    ray.Init(end, new_end);
    Interfaces::trace->TraceRay(ray, MASK_SHOT, 0, exit_trace);

    if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
      ray.Init(end, start);

      TraceFilter filter;
      filter.pSkip = exit_trace->m_pEntityHit;

      Interfaces::trace->TraceRay(ray, 0x600400B, &filter, exit_trace);

      if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) &&
          !exit_trace->startsolid) {
        end = exit_trace->endpos;
        return true;
      }

      continue;
    }

    if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid ||
          exit_trace->startsolid) ||
        exit_trace->startsolid) {
      if (exit_trace->m_pEntityHit && enter_trace->m_pEntityHit) {
        return true;
      }

      continue;
    }

    if (exit_trace->surface.flags >> 7 & 1 &&
        !(enter_trace->surface.flags >> 7 & 1))
      continue;

    if (exit_trace->plane.normal.Dot(dir) <= 1.0f) {
      auto fraction = exit_trace->fraction * 4.0f;
      end = end - (dir * fraction);

      return true;
    }
  }

  return false;
}

bool Features::RageBot::handleBulletPenetration(
    WeaponInfo *weaponInfo, Features::RageBot::FireBulletData &data) {
  SurfaceData *enter_surface_data =
      Interfaces::physicsSurfaceProps->GetSurfaceData(
          data.enter_trace.surface.surfaceProps);
  int enter_material = enter_surface_data->game.material;
  float enter_surf_penetration_mod =
      enter_surface_data->game.flPenetrationModifier;

  data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
  data.current_damage *=
      powf(weaponInfo->GetRangeModifier(), data.trace_length * 0.002f);

  if (data.trace_length > 3000.f || enter_surf_penetration_mod < 0.1f)
    data.penetrate_count = 0;

  if (data.penetrate_count <= 0)
    return false;

  Vector dummy;
  Trace trace_exit;

  if (!traceToExit(dummy, &data.enter_trace, data.enter_trace.endpos,
                   data.direction, &trace_exit))
    return false;

  SurfaceData *exit_surface_data = Interfaces::physicsSurfaceProps->GetSurfaceData(trace_exit.surface.surfaceProps);
  int exit_material = exit_surface_data->game.material;

  float exit_surf_penetration_mod =
      exit_surface_data->game.flPenetrationModifier;

  float final_damage_modifier = 0.16f;
  float combined_penetration_modifier = 0.0f;

  if ((data.enter_trace.contents & CONTENTS_GRATE) != 0 ||
      enter_material == 89 || enter_material == 71) {
    combined_penetration_modifier = 3.0f;
    final_damage_modifier = 0.05f;
  } else
    combined_penetration_modifier =
        (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;

  if (enter_material == exit_material) {
    if (exit_material == 87 || exit_material == 85)
      combined_penetration_modifier = 3.0f;
    else if (exit_material == 76)
      combined_penetration_modifier = 2.0f;
  }

  float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
  float v35 =
      (data.current_damage * final_damage_modifier) +
      v34 * 3.0f * fmaxf(0.0f, (3.0f / weaponInfo->GetPenetration()) * 1.25f);
  float thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

  thickness *= thickness;
  thickness *= v34;
  thickness /= 24.0f;

  float lost_damage = fmaxf(0.0f, v35 + thickness);

  if (lost_damage > data.current_damage)
    return false;

  if (lost_damage >= 0.0f)
    data.current_damage -= lost_damage;

  if (data.current_damage < 1.0f)
    return false;

  data.src = trace_exit.endpos;
  data.penetrate_count--;

  return true;
}

void Features::RageBot::traceLine(Vector vecAbsStart, Vector vecAbsEnd,
                                  unsigned int mask, Player *ignore,
                                  Trace *ptr) {
  Ray ray;
  ray.Init(vecAbsStart, vecAbsEnd);
  TraceFilter filter;
  filter.pSkip = ignore;

  Interfaces::trace->TraceRay(ray, mask, &filter, ptr);
}

bool Features::RageBot::simulateFireBullet(Weapon *pWeapon, bool allowFriendlyFire,
                               Features::RageBot::FireBulletData &data) {
  WeaponInfo *weaponInfo = pWeapon->GetWeaponInfo();

  data.penetrate_count = 4;
  data.trace_length = 0.0f;
  data.current_damage = (float)weaponInfo->GetDamage();

  while (data.penetrate_count > 0 && data.current_damage >= 1.0f) {
    data.trace_length_remaining = weaponInfo->GetRange() - data.trace_length;

    Vector end = data.src + data.direction * data.trace_length_remaining;

    // data.enter_trace
    traceLine(data.src, end, MASK_SHOT, Globals::localPlayer,
              &data.enter_trace);

    Ray ray;
    ray.Init(data.src, end + data.direction * 40.f);

    Interfaces::trace->TraceRay(ray, MASK_SHOT, &data.filter, &data.enter_trace);

    traceLine(data.src, end + data.direction * 40.f, MASK_SHOT, Globals::localPlayer,
              &data.enter_trace);

    if (data.enter_trace.fraction == 1.0f)
      break;

    if (data.enter_trace.hitgroup <= HitGroups::HITGROUP_RIGHTLEG &&
        data.enter_trace.hitgroup > HitGroups::HITGROUP_GENERIC) {
      data.trace_length +=
          data.enter_trace.fraction * data.trace_length_remaining;
      data.current_damage *=
          powf(weaponInfo->GetRangeModifier(), data.trace_length * 0.002f);

      if (!allowFriendlyFire && data.enter_trace.m_pEntityHit &&
        data.enter_trace.m_pEntityHit->isPlayer() && !data.enter_trace.m_pEntityHit->isEnemy()) 
        return false;

      Player *player = (Player *)data.enter_trace.m_pEntityHit;

      scaleDamage(data.enter_trace.hitgroup, player,
                  weaponInfo->GetWeaponArmorRatio(), data.current_damage);

      return true;
    }

    if (!handleBulletPenetration(weaponInfo, data))
      break;
  }

  return false;
}

int Features::RageBot::getDamageDeal(Player *player, const Vector &point,
                                     Weapon *weapon, bool allowFriendlyFire) {
  Vector dst = point;
  if (!Globals::localPlayer || Globals::localPlayer->health() <= 0)
    return -1;

  Features::RageBot::FireBulletData data;
  data.src = Globals::localPlayer->eyePos();
  data.filter.pSkip = Globals::localPlayer;

  QAngle angles = calcAngle(data.src, dst);
  angleVectors(angles, data.direction);

  data.direction.NormalizeInPlace();

  if (simulateFireBullet(weapon, allowFriendlyFire, data) && data.enter_trace.m_pEntityHit == player)
    return (int)data.current_damage;

  return -1.0f;
}

bool Features::RageBot::canShoot(Weapon *weapon, QAngle *angle, 
              Player *enemy, int hitChance, int minDamage) {
  if (hitChance == 0)
    return true;

  Vector src = Globals::localPlayer->eyePos();
  Vector forward, right, up;
  angleVectors(*angle, &forward, &right, &up);

  int hitCount = 0;

  weapon->UpdateAccuracyPenalty();
  float weap_spread = weapon->GetSpread();
  float weap_inaccuracy = weapon->GetInaccuracy();

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
    viewAnglesSpread.normalize();

    Vector viewForward;
    angleVectors(viewAnglesSpread, viewForward);
    viewForward.NormalizeInPlace();

    // viewForward = (viewForward * weapon->GetWeaponInfo()->GetRange());

    Features::RageBot::FireBulletData data;
    data.src = src;
    data.filter.pSkip = Globals::localPlayer;
    data.direction = viewForward;

    if (!simulateFireBullet(weapon, false, data)) {
      continue;
    }

    if (data.enter_trace.m_pEntityHit == enemy && data.current_damage >= minDamage) {
      hitCount++;
    }

    if (hitCount >= hitChance) {
      return true;
    } else if (100 - i + hitCount < hitChance) {
      return false;
    }
  }

  return false;
}

std::vector<Vector> Features::RageBot::getPoints(Player *player, int iHitbox,
                                                 float headScale,
                                                 float bodyScale) {
  std::vector<Vector> points;

  model_t *pModel = player->model();
  if (!pModel)
    return points;

  studiohdr_t *hdr = Interfaces::modelInfo->GetStudioModel(pModel);
  if (!hdr)
    return points;
  mstudiobbox_t *bbox = hdr->pHitbox(iHitbox, 0);
  if (!bbox)
    return points;

  matrix3x4_t matrix[128];
  if (!player->getHitboxBones(matrix))
    return points;

  Vector mins, maxs;
  vectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
  vectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
  Vector center = (mins + maxs) * 0.5f;

  if (bbox->radius <= 0.f) {
    Notifications::addNotification(ImColor(255, 255, 255), "negative radius");
    matrix3x4_t rot;
    angleMatrix(bbox->rotation, rot);

    auto origin = rot.GetOrigin();

    Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

    points.emplace_back(Vector(center.Dot(rot[0]), center.Dot(rot[1]), center.Dot(rot[2])) + origin);
  } else {
    points.emplace_back(center);

    if (iHitbox == (int)HitboxModel::HITBOX_HEAD) {
      auto pitch_down = normalizePitch(player->eyeAngles().x) > 85.0f;
      auto originY = Globals::localPlayer->origin().y;
      float stuff =
          calcAngle(player->eyePos(), Vector{originY, originY, originY}).y;
      auto backward = fabs(player->eyeAngles().y - stuff) > 120.0f;
      auto final_radius = bbox->radius * headScale;

      points.emplace_back(Vector(bbox->bbmax.x + 0.70710678f * final_radius,
                              bbox->bbmax.y - 0.70710678f * final_radius,
                              bbox->bbmax.z));
      points.emplace_back(
          Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius));
      points.emplace_back(
          Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius));
      points.emplace_back(
          Vector(bbox->bbmax.x, bbox->bbmax.y - final_radius, bbox->bbmax.z));

      if (pitch_down && backward)
        points.emplace_back(
            Vector(bbox->bbmax.x - final_radius, bbox->bbmax.y, bbox->bbmax.z));
    } else {
      auto final_radius = bbox->radius * bodyScale;

      if (iHitbox == (int)HitboxModel::HITBOX_STOMACH) {
        points.emplace_back(Vector(center.x, center.y, bbox->bbmin.z + final_radius));
        points.emplace_back(Vector(center.x, center.y, bbox->bbmax.z - final_radius));
        points.emplace_back(Vector{center.x, bbox->bbmax.y - final_radius, center.z});
      } else if (iHitbox == (int)HitboxModel::HITBOX_PELVIS || iHitbox == (int)HitboxModel::HITBOX_UPPER_CHEST) {
        points.emplace_back(Vector(center.x, center.y, bbox->bbmax.z + final_radius));
        points.emplace_back(Vector(center.x, center.y, bbox->bbmin.z - final_radius));
      } else if (iHitbox == (int)HitboxModel::HITBOX_LOWER_CHEST || iHitbox == (int)HitboxModel::HITBOX_CHEST) {
        points.emplace_back(Vector(center.x, center.y, bbox->bbmax.z + final_radius));
        points.emplace_back(Vector(center.x, center.y, bbox->bbmin.z - final_radius));
        points.emplace_back(Vector{center.x, bbox->bbmax.y - final_radius, center.z});
      } else {
        points.emplace_back(Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + final_radius));
        points.emplace_back(Vector(bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - final_radius));
        points.emplace_back(Vector(center.x, bbox->bbmax.y - final_radius, center.z));
      }
    }
  }

  return points;
}

void Features::RageBot::bestMultiPoint(Player *player, int &BoneIndex,
                                        int &Damage, Vector &Spot, float headScale, float bodyScale,
                                        Weapon *weapon, bool friendlyFire) {
  for (const auto& point : getPoints(player, BoneIndex, headScale, bodyScale)) {
    int bestDamage = getDamageDeal(player, point, weapon, friendlyFire);
    if (bestDamage > Damage) {
      Damage = bestDamage;
      Spot = point;
    }
  }
}

void Features::RageBot::applyAutoSlow(CUserCmd *cmd, Weapon *weapon) {
  if (Interfaces::globals->interval_per_tick == FP_NAN ||
      Interfaces::globals->interval_per_tick == 0 ||
      Interfaces::globals->interval_per_tick == FP_INFINITE) 
      return;

  const float speed = Globals::localPlayer->velocity().Length2D();
  const float maxSpeed =
      (Globals::localPlayer->scoped()
           ? weapon->GetWeaponInfo()->GetMaxPlayerSpeedScoped()
           : weapon->GetWeaponInfo()->GetMaxPlayerSpeed());

  if (speed > maxSpeed / 3) {
    static const auto cl_forwardspeed = Interfaces::convar->FindVar("cl_forwardspeed");

    QAngle direction;
    vectorAngles(Globals::localPlayer->velocity(), direction);
    QAngle viewAngle;
    Interfaces::engine->GetViewAngles(viewAngle);
    direction.y = viewAngle.y - direction.y;
    Vector forward;
    angleVectors(direction, forward);
    Vector negated_direction = forward * -(cl_forwardspeed->GetFloat() / speed);
    cmd->forwardmove = negated_direction.x;
    cmd->sidemove = negated_direction.y;
  } else {
    float sped = 0.1f;
    float ratio = maxSpeed / 255.0f;
    sped *= ratio;

    cmd->forwardmove *= sped;
    cmd->sidemove *= sped;
  }
  cmd->buttons |= IN_WALK;
}

void Features::RageBot::createMove(CUserCmd *cmd) {
  if (!(Menu::CustomWidgets::isKeyDown(CONFIGINT("Rage>RageBot>Key")) || CONFIGBOOL("Rage>RageBot>Always on")) ||
        !Interfaces::engine->IsInGame() || !Globals::localPlayer || Globals::localPlayer->gunGameImmunity() ||
        Globals::localPlayer->flags() & FL_FROZEN || Globals::localPlayer->health() <= 0 ||
        Globals::localPlayer->moveType() != MOVETYPE_WALK)
    return;

  const auto weapon = 
      (Weapon *)Interfaces::entityList->GetClientEntity(
          (uintptr_t)Globals::localPlayer->activeWeapon() & 0xFFF);
  if (!weapon || !weapon->clip())
    return;

  if (Globals::localPlayer->shotsFired() > 0 && !weapon->GetWeaponInfo()->GetFullAuto())
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
                 weapon->itemIndex() & 0xFFF) != std::end(pistols))) {
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
                        weapon->itemIndex() & 0xFFF) != std::end(heavyPistols))) {
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
                        weapon->itemIndex() & 0xFFF) != std::end(rifles))) {
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
                        weapon->itemIndex() & 0xFFF) != std::end(smgs))) {
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
  } else if (((weapon->itemIndex() & 0xFFF) == WEAPON_SSG08)) {
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
  } else if (((weapon->itemIndex() & 0xFFF) == WEAPON_AWP)) {
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
                        weapon->itemIndex() & 0xFFF) !=
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

  if (weapon->nextPrimaryAttack() > Globals::serverTime())
    return;
    
  if (!ignoreBlind && Globals::localPlayer->maxFlashAlpha() > 75.f)
    return;

  if (cmd->buttons & (1 << 0) || autoShot) {
    if (scopedOnly && weapon->isSniperRifle() &&
        !Globals::localPlayer->scoped())
      return;

    int bestDamage = -1;
    QAngle bestPlayerAngle = {0, 0, 0};
    QAngle aimPunch = Globals::localPlayer->aimPunch() * 2;
    bool hasTarget = false;
    bool hasSelectedTarget = false;

    // Enumerate over players and get angle to the closest player to crosshair
    for (int i = 1; i < Interfaces::globals->maxClients; i++) {
      Player *p = (Player *)Interfaces::entityList->GetClientEntity(i);
      if (p && p != Globals::localPlayer) {
        if (p->health() > 0 && !p->dormant() &&
            (p->isEnemy() || friendlyFire) && (p->visible() || !visibleOnly) && !p->gunGameImmunity()) {
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
              if ((headScale == 0 && bone == 8) || bodyScale == 0) {
                targetBonePos = p->getBonePos(bone);
                damageDeal =
                    getDamageDeal(p, targetBonePos, weapon, friendlyFire);
              } else {
                int hitbox = (int)((1 << i & (int)HitBoxes::HEAD)
                                       ? HitboxModel::HITBOX_HEAD
                                   : (1 << i & (int)HitBoxes::NECK)
                                       ? HitboxModel::HITBOX_NECK
                                   : (1 << i & (int)HitBoxes::CHEST)
                                       ? HitboxModel::HITBOX_CHEST
                                   : (1 << i & (int)HitBoxes::STOMACH)
                                       ? HitboxModel::HITBOX_STOMACH
                                   : (1 << i & (int)HitBoxes::PELVIS)
                                       ? HitboxModel::HITBOX_PELVIS
                                       : HitboxModel::HITBOX_STOMACH);
                bestMultiPoint(p, hitbox, damageDeal, targetBonePos, headScale, bodyScale,
                               weapon, friendlyFire);
              }

              if (damageDeal <= 0 ||
                  damageDeal < (killShot
                                    ? p->health()
                                    : (p->health() < minDamage ? p->health()
                                                               : minDamage))) {
                continue;
              }

              if (!ignoreSmoke && Offsets::lineGoesThroughSmoke(localPlayerEyePos, targetBonePos, 1))
                continue;

              QAngle directAngle = calcAngle(localPlayerEyePos, targetBonePos);
              QAngle angleTarget = (directAngle - cmd->viewangles).normalize();

              if (angleTarget.Length() > FOV) {
                continue;
              }

              if (damageDeal > bestDamage) {
                hasTarget = true;
                if (!canShoot(weapon, &directAngle, p, hitChance, minDamage)) {
                  continue;
                }
                bestDamage = damageDeal;
                bestPlayerAngle = directAngle - aimPunch;
                hasSelectedTarget = true;
              }
            }
          }
        }
      }
    }
    if (hasTarget) {
      if (autoSlow) {
        applyAutoSlow(cmd, weapon);
      }
      if (autoScope && weapon->isSniperRifle() &&
          !Globals::localPlayer->scoped()) {
        cmd->buttons |= IN_ATTACK2;
        return;
      }
      if (hasSelectedTarget) {
        if (autoShot) {
          cmd->buttons |= IN_ATTACK;
        }

        cmd->viewangles = bestPlayerAngle;
      }
    }
  }
}