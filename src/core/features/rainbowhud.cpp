#include "features.hpp"

void Features::RainbowHud::frameStageNotify(FrameStage frame) {
  if (frame != FRAME_RENDER_START)
    return;
    
  const auto cvar = Interfaces::convar->FindVar("cl_hud_color");

  static bool enabled = false;
  static int backup = 0;

  if (CONFIGBOOL("Visuals>World>World>Rainbow HUD")) {
    static float currentColor = 0;

    cvar->SetValue(currentColor);

    currentColor += 0.05;

    if (currentColor > 10)
      currentColor = 0;

    enabled = true;
  } else {
    if (enabled)
      cvar->SetValue(backup);

    enabled = false;
  }
}