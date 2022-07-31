#pragma once
#include "../netvars.hpp"
#include "vector.h"
#include "../../utils/utils.hpp"
#include <functional>

inline std::map<float, int, std::greater<float>> entityDistanceMap; // Map containing entity distance
void cachePlayers();

class ICollideable {
public:
	virtual void pad0();
	virtual Vector& OBBMins();
	virtual Vector& OBBMaxs();
};

class Entity {
public:
	void* networkable() {
		return reinterpret_cast<void*>(uintptr_t(this) + 16);
	}

	void* renderable() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x8);
	}

	ClientClass* clientClass() {
		typedef ClientClass*(*Fn)(void*);
		return getVirtualFunc<Fn>(networkable(), 2)(networkable());
	}

	bool dormant() {
		typedef bool(*Fn)(void*);
		return getVirtualFunc<Fn>(networkable(), 9)(networkable());
	}

	int index() {
		typedef int(*Fn)(void*);
		return getVirtualFunc<Fn>(networkable(), 10)(networkable());
	}

	model_t* model() {
		typedef model_t* (*Fn)(void*);
		return getVirtualFunc<Fn>(renderable(), 8)(renderable());
	}

	bool setupBones(matrix3x4_t* pBoneMatrix, int nMaxBones, int nBoneMask, float flCurTime = 0) {
		typedef bool (*Fn)(void*, matrix3x4_t*, int, int, float);
		return getVirtualFunc<Fn>(renderable(), 13)(renderable(), pBoneMatrix, nMaxBones, nBoneMask, flCurTime);
	}

	bool shouldDraw() {
		typedef bool (*Fn)(void*);
		return getVirtualFunc<Fn>(renderable(), 5)(renderable());
	}

	const Vector& origin()
	{
		typedef const Vector& (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 12)(this);
	}

	bool isPlayer() {
		typedef bool (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 157)(this);
	}

	Entity* GetWeapon() {
		typedef Entity* (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 331)(this);
	}

	NETVAR("DT_BaseEntity", "m_Collision", collideable, ICollideable);
	NETVAR("DT_BaseEntity", "m_iTeamNum", team, int);
	NETVAR("DT_BaseEntity", "m_bSpotted", spotted, bool);
};

class Player : public Entity {
public:
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", velocity, Vector);
	NETVAR("DT_BasePlayer", "m_nTickBase", tickbase, unsigned int);
	NETVAR("DT_CSPlayer", "m_iAccount", money, int);
    NETVAR("DT_BasePlayer", "m_iHealth", health, int);
    NETVAR("DT_CSPlayer", "m_fFlags", flags, int);
    NETVAR("DT_BasePlayer", "m_aimPunchAngle", aimPunch, QAngle);
    NETVAR("DT_BasePlayer", "m_viewPunchAngle", viewPunch, QAngle);
    NETVAR("DT_CSPlayer", "m_hActiveWeapon", activeWeapon, void *);
    NETVAR("DT_CSPlayer", "m_hObserverTarget", observerTarget, Player *);
    NETVAR("DT_CSPlayer", "m_bHasDefuser", defuser, bool);
    NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", viewOffset, Vector);
    NETVAR("DT_CSPlayer", "m_angEyeAngles[0]", eyeAngles, QAngle);
	NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget", lbyTarget, float);
	NETVAR("DT_CSPlayer", "m_bIsScoped", scoped, bool);
	NETVAR("DT_CSPlayer", "m_bGunGameImmunity", gunGameImmunity, bool);
	NETVAR("DT_BasePlayer", "deadflag", deadflag, bool);
    NETVAR("DT_CSPlayer", "m_flFlashDuration", flashDuration, float);
    NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha", maxFlashAlpha, float);
    NETVAR("DT_CSPlayer", "m_iShotsFired", shotsFired, float);
    NETVAR("DT_CSPlayer", "m_bHasHelmet", helmet, bool);
    NETVAR("DT_CSPlayer", "m_ArmorValue", armor, int);
    NETVAR("DT_CSPlayer", "m_nSurvivalTeam", survivalTeam, int);
    NETVAR("DT_BaseAnimating", "m_bClientSideAnimation", clientAnimation, bool);

    AnimState *animState() {
      return *reinterpret_cast<AnimState **>((uintptr_t)this +
                                             Offsets::animState);
	}

	void SetAbsOrigin(const Vector *const angles)
	{
		asm volatile("mov %0, %%rdi;\n\t"
				   "mov %1, %%rsi;\n\t"
				   "call *%P2;"
				   :
				   : "r"(this), "r"(angles), "r"(Offsets::SetAbsOriginFnAddr)
				   : "%rdi", "%rsi");
	}

	QAngle* viewAngles() {
		return (QAngle*)((uintptr_t)deadflag_ptr() + 0x4);
	}

	Vector eyePos() {
		return origin() + ((viewOffset().z > 0) ? viewOffset() : Vector(0, 0, (flags() & (1 << 1)) ? 46 : 64)); // For some reason some ents' viewoffset is all 0s, this is a hacky fix for it
	}

	int crosshair() {
		return *reinterpret_cast<int*>((uintptr_t)defuser_ptr()+0x7c);
	}

	int moveType() {
		return *reinterpret_cast<int*>((uintptr_t)this + GETNETVAROFFSET("DT_BaseEntity", "m_nRenderMode") + 1);
	}

	void saveData(const char *context, int slot, int type) {
		Offsets::saveData(this, context, slot, type);
	}

	void restoreData (const char *context, int slot, int type) {
		Offsets::restoreData(this, context, slot, type);
	}

	void onPostRestoreData() {
		Offsets::onPostRestoreData(this);
	}

    float getMaxDesyncAngle() noexcept {
        auto state = animState();
        if (!state)
           return 0.0f;

        float yawModifier =
            (state->stopToFullRunningFraction * -0.3f - 0.2f) *
                std::clamp(state->footSpeed, 0.0f, 1.0f) +
            1.0f;

        if (state->duckAmount > 0.0f)
          yawModifier += (state->duckAmount *
                          std::clamp(state->footSpeed2, 0.0f, 1.0f) *
                          (0.5f - yawModifier));

        return state->velocitySubtractY * yawModifier;
    }

    bool isEnemy();
	bool getHitboxBones(matrix3x4_t* boneMatrix);
	bool getAnythingBones(matrix3x4_t* boneMatrix);
	Vector getBonePos(int bone);
	bool visible();
};

class Item : public Entity{
public:
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", itemIndex, ItemIndex);
};

enum class WeaponType {
  Knife = 0,
  Pistol,
  SubMachinegun,
  Rifle,
  Shotgun,
  SniperRifle,
  Machinegun,
  C4,
  Placeholder,
  Grenade,
  Unknown,
  StackableItem,
  Fists,
  BreachCharge,
  BumpMine,
  Tablet,
  Melee
};

class WeaponInfo {
public:
  char *GetConsoleName() { return *(char **)((uintptr_t)this + 0x8); }

  int GetClipSize() { return *(int *)((uintptr_t)this + 0x20); }

  WeaponType GetWeaponType() {
    return *(WeaponType *)((uintptr_t)this + 0x140);
  }

  void SetWeaponType(WeaponType type) {
    *(WeaponType *)((uintptr_t)this + 0x140) = type;
  }

  bool GetFullAuto() { return *(bool *)((uintptr_t)this + 0x168); }

  int GetDamage() { return *(int *)((uintptr_t)this + 0x16C); }

  float GetCycleTime() { return *(float *)((uintptr_t)this + 0x158); }

  float GetWeaponArmorRatio() { return *(float *)((uintptr_t)this + 0x174); }

  float GetPenetration() { return *(float *)((uintptr_t)this + 0x17C); }

  float GetRange() { return *(float *)((uintptr_t)this + 0x188); }

  float GetRangeModifier() { return *(float *)((uintptr_t)this + 0x18C); }

  float GetMaxPlayerSpeed() { return *(float *)((uintptr_t)this + 0x1B8); }

  float GetMaxPlayerSpeedScoped() { return *(float *)((uintptr_t)this + 0x1BC); }

  int GetZoomLevels() { // Doesn't work correctly on some weapons.
    return *(int *)((uintptr_t)this +
                    0x23C); // DT_WeaponCSBaseGun.m_zoomLevel ?
  }

  int *GetTracerFrequency() { return (int *)((uintptr_t)this + 0x298); }

  char *GetTracerEffect() { return *(char **)((uintptr_t)this + 0x290); }

  float GetRecoilMagnitude() { return *(float *)((uintptr_t)this + 0x224); }

  float GetRecoveryTimeStand() { return *(float *)((uintptr_t)this + 0x232); }
};

class Weapon : public Item {
public:
	NETVAR("DT_BaseCombatWeapon", "m_hOwner", owner, int);

	NETVAR("DT_BaseCombatWeapon", "m_iItemIDHigh", itemIDHigh, int);
	NETVAR("DT_BaseCombatWeapon", "m_iAccountID", accountID, int);
	NETVAR("DT_BaseCombatWeapon", "m_nFallbackPaintKit", paintKit, int);
	NETVAR("DT_BaseCombatWeapon", "m_flFallbackWear", wear, float);
	NETVAR("DT_BaseCombatWeapon", "m_iClip1", clip, int);
	NETVAR("DT_BaseCombatWeapon", "m_nFallbackStatTrak", statTrack, int);
	NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", nextPrimaryAttack, float);

	float GetSpread() {
		typedef float (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 521)(this);
	}

	float GetInaccuracy() {
		typedef float (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 551)(this);
	}

	WeaponType getWeaponType() {
		typedef WeaponType (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 523)(this);
	}

	WeaponInfo* GetWeaponInfo() {
		typedef WeaponInfo* (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 529)(this);
	}

	void UpdateAccuracyPenalty() {
		typedef void (*Fn)(void*);
		return getVirtualFunc<Fn>(this, 552)(this);
	}

    // auto requiresRecoilControl() noexcept
    // {
    //     const auto weaponData = GetWeaponInfo();
    //     if (weaponData)
    //         return weaponData->GetRecoveryTimeStand() > weaponData->GetCycleTime();
    //     return false;
    // }

    auto isSniperRifle() noexcept { return getWeaponType() == WeaponType::SniperRifle; }
};

class PlantedC4 : public Item {
public:
  NETVAR("DT_PlantedC4", "m_flC4Blow", time, float);
  NETVAR("DT_PlantedC4", "m_flTimerLength", timerLength, float);
  NETVAR("DT_PlantedC4", "m_nBombSite", site, int);
  NETVAR("DT_PlantedC4", "m_bBombTicking", ticking, bool);
  NETVAR("DT_PlantedC4", "m_flDefuseCountDown", defuseCountDown, float);
  NETVAR("DT_PlantedC4", "m_flDefuseLength", defuseLength, float);
  NETVAR("DT_PlantedC4", "m_hBombDefuser", defuser, int);
};

class TonemapController {
public:
	NETVAR("DT_EnvTonemapController", "m_bUseCustomAutoExposureMin", useExposureMin, bool);
	NETVAR("DT_EnvTonemapController", "m_bUseCustomAutoExposureMax", useExposureMax, bool);
	NETVAR("DT_EnvTonemapController", "m_flCustomAutoExposureMin", exposureMin, float);
	NETVAR("DT_EnvTonemapController", "m_flCustomAutoExposureMax", exposureMax, float);
};

class FogController {
public:
	NETVAR("DT_FogController", "m_fog.enable", enable, bool);
	NETVAR("DT_FogController", "m_fog.start", start, float);
	NETVAR("DT_FogController", "m_fog.end", end, float);
	NETVAR("DT_FogController", "m_fog.farz", farz, float);
	NETVAR("DT_FogController", "m_fog.maxdensity", maxDensity, float);
	NETVAR("DT_FogController", "m_fog.colorPrimary", colorPrimary, int);
};

struct BombData {
  void update() noexcept;

  float blowTime = 0.f;
  float timerLength = 1.f;
  int defuserHandle = -1;
  float defuseCountDown = 0.f;
  float defuseLength = 1.f;
  int bombsite = 0;
  Vector origin = Vector{0, 0, 0};
};