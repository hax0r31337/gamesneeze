#include "includes.hpp"
#include "sdk/interfaces/interfaces.hpp"

float Globals::serverTime(CUserCmd *cmd) {
  static int tick;
  static CUserCmd *lastCmd;

  if (cmd) {
    if (localPlayer && (!lastCmd || lastCmd->hasbeenpredicted))
      tick = localPlayer->tickbase();
    else
      tick++;
    lastCmd = cmd;
  }
  return tick * Interfaces::globals->interval_per_tick;
}