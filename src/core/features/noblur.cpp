#include "features.hpp"

void Features::NoBlur::frameStageNotify(FrameStage stage) {
  static auto blur = Interfaces::convar->FindVar("@panorama_disable_blur");
  blur->SetValue(CONFIGBOOL("Visuals>World>World>No Blur"));
}