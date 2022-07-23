#pragma once
#include "../../includes.hpp"
#include <vector>

namespace Features {
    namespace Prediction {
        inline bool inPrediction;
        void start(CUserCmd *cmd);
        void end();
        void restoreEntityToPredictedFrame(int predicted_frame);
    }
    namespace ESP {
        void draw();
    }
    namespace Chams {
        void drawModelExecute(void* thisptr, void* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
    }
    namespace AutoDefuse {
        void createMove(CUserCmd* cmd);
    }
    namespace Nightmode {
        void onTonemapController(TonemapController* tonemapController);
    }
    namespace RankReveal {
        void createMove(CUserCmd* cmd);
    }
    namespace RecoilCrosshair {
        void frameStageNotify(FrameStage frame);
        void draw();
    }
    namespace SkinChanger {
        void frameStageNotify(FrameStage frame);
    }
    namespace LegitBot {
        void createMove(CUserCmd* cmd);
    }
    namespace Backtrack {
        struct BacktrackPlayer {
            matrix3x4_t boneMatrix[128];
            int playerIndex;
            int playerFlags;
            float playerVelocity;
            Vector playerHeadPos;
        };

        struct BackTrackTick {
            std::map<int, BacktrackPlayer> players;
            int tickCount;
        };

        inline int lastBacktrack;
        inline std::vector<BackTrackTick> backtrackTicks;

        void store(CUserCmd* cmd);
        void createMove(CUserCmd* cmd);
    }
    namespace Forwardtrack {
        void createMove(CUserCmd* cmd);
    }
    namespace ColorModulation {
        void updateColorModulation();
    }
    namespace ClantagChanger {
        void frameStageNotify(FrameStage frame);
    }
    namespace NameStealer {
        bool changeName(bool reconnect, const char* newName);
        void setName(const char* name);
        void setBannedName(const char *name);
        void frameStageNotify(FrameStage frame);
        enum class Colors {
          WHITE = 1,
          DARK_RED,
          LIGHT_PURPLE,
          DARK_GREEN,
          LIGHT_GREEN,
          GREEN,
          LIGHT_RED,
          GRAY,
          YELLOW,
          LIGHT_BLUE,
          BLUE,
          DARK_BLUE,
          DARK_GRAY,
          PURPLE,
          RED,
          ORANGE,
        };
    }

    namespace QuickPeek {
        void gotoStart(CUserCmd* cmd);
        void createMove(CUserCmd *cmd);
    }
    namespace SkyboxChanger {
        void frameStageNotify(FrameStage frame);
    }
    namespace Spectators {
        void draw();
    }
    namespace PlayerList {
        void draw();
    }
    namespace Notifications {
        void draw();
        void addNotification(ImColor color, const char* fmt, ...);
    }
    namespace FlappyBird {
        void draw();
    }
    namespace UseSpam {
        void createMove(CUserCmd* cmd);
    }
    namespace AutoAccept {
        void emitSound(const char* pSoundEntry);
    }
    namespace Radar {
        void espPlayerLoop(Player* p);
    }
    namespace FastDuck {
        void createMove(CUserCmd *cmd);
    }
    namespace Triggerbot {
        void createMove(CUserCmd* cmd);
        Entity *findPlayerThatRayHits(Vector start, Vector end,
                                      Trace *traceToPlayer);
    }
    namespace ReportBot {
        inline std::vector<std::uint64_t> reportedPlayers;
        inline int reportbotRound;
        void createMove(CUserCmd* cmd);
        std::vector<std::uint64_t> getXuidsOfCandidatesToBeReported();
        std::string generateReportString();
        bool isPlayerReported(std::uint64_t xuid);
    }
    namespace RageBot {
        struct FireBulletData {
            Vector src;
            Trace enter_trace;
            Vector direction;
            TraceFilter filter;
            float trace_length;
            float trace_length_remaining;
            float current_damage;
            int penetrate_count;
        };
        void createMove(CUserCmd* cmd);
        void applyAutoSlow(CUserCmd *cmd, Weapon *weapon) ;
        
        void scaleDamage(HitGroups hitgroup, Player *enemy,
                 float weapon_armor_ratio, float &current_damage);
        bool traceToExit(Vector &end, Trace *enter_trace, Vector start, Vector dir,
                 Trace *exit_trace);
        bool handleBulletPenetration(WeaponInfo *weaponInfo,
                                    Features::RageBot::FireBulletData &data);
        void traceLine(Vector vecAbsStart, Vector vecAbsEnd, unsigned int mask,
               Player *ignore, Trace *ptr);
        bool simulateFireBullet(Weapon *pWeapon, bool allowFriendlyFire,
                               FireBulletData &data);
        int getDamageDeal(Player *entity, const Vector &destination,
                Weapon *weapon, bool allowFriendlyFire);
        void bestHeadPoint(Player *player, int &Damage,
                           Vector &Spot, float headScale,
                           Weapon *weapon, bool friendlyFire);
        void bestMultiPoint(Player *player, int &BoneIndex, int &Damage,
                            Vector &Spot, float bodyScale,
                            Weapon *weapon, bool friendlyFire);
        bool canShoot(Weapon *activeWeapon, QAngle *angle, Player *enemy,
                      int hitChance);
    }
    namespace AntiAim {
        inline matrix3x4_t fakeBones[128];
        inline float fakeYaw;
        void createMove(CUserCmd* cmd);
    }
    namespace Hitmarkers {
        struct DamageMarker {
            float drawHitmarkerTill;
            Vector position;
            bool headshot;
            int damage;
        };

        inline std::vector<DamageMarker> damageMarkers;
        inline float drawHitmarkerTill;
        void draw();
    }
    namespace SlowWalk {
        void createMove(CUserCmd* cmd);
    }
    namespace NoFlash {
        void frameStageNotify(FrameStage frame);
    }
    namespace RagdollGravity {
        void frameStageNotify(FrameStage frame);
    }
    namespace Resolver {
        void frameStageNotify(FrameStage frame);
    }

    namespace AutoL {
        void killEvent(player_info_t info);
    }
    namespace NoVisualRecoil {
        void frameStageNotify(FrameStage frame);
    }
    namespace BulletTracers {
        void event(IGameEvent* event);
    }
    namespace Movement {
        inline int flagsBackup;
        inline Vector velBackup;
        inline bool shouldEdgebug;
        inline bool shouldDuckNext;
        inline Vector edgebugPos;
        void prePredCreateMove(CUserCmd* cmd);
        void postPredCreateMove(CUserCmd* cmd);
        void edgeBugPredictor(CUserCmd* cmd);
        void draw();
    }
}
