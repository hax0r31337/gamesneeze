#include "features.hpp"

void Features::AsusWalls::frameStageNotify(FrameStage frame) {
  static float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

  std::unordered_map<MaterialHandle_t, ImColor> worldMaterials;
//   std::unordered_map<MaterialHandle_t, ImColor> worldMaterials2;

  if (!Interfaces::engine->IsInGame() && worldMaterials.size() > 0) {
    for (const auto &it : worldMaterials) {
      IMaterial *mat = Interfaces::materialSystem->GetMaterial(it.first);

      if (!mat) {
        continue;
      }

      mat->GetColorModulate(&r, &g, &b);
      a = mat->GetAlphaModulation();

      mat->ColorModulate(r, g, b);
      mat->AlphaModulate(a);
    }

    worldMaterials.clear();
    // worldMaterials2.clear();
  }

  if (frame != FRAME_NET_UPDATE_POSTDATAUPDATE_END || !CONFIGBOOL("Visuals>World>World>Asus Walls>Enabled")) {
    return;
  }

  for (MaterialHandle_t i = Interfaces::materialSystem->FirstMaterial();
       i != Interfaces::materialSystem->InvalidMaterial();
       i = Interfaces::materialSystem->NextMaterial(i)) {
    IMaterial *mat = Interfaces::materialSystem->GetMaterial(i);

    if (!mat || strcmp(mat->GetTextureGroupName(), "World textures") != 0) {
      continue;
    }

    if (worldMaterials.find(i) == worldMaterials.end()) {
      mat->GetColorModulate(&r, &g, &b);
      a = mat->GetAlphaModulation();
      worldMaterials.emplace(i, ImColor(r, g, b, a));
    //   worldMaterials2.emplace(i, ImColor(r, g, b, a));
    }

    ImColor color = CONFIGCOL("Visuals>World>World>Asus Walls>Color");

    if (worldMaterials.at(i) != color) {

      mat->ColorModulate(color.Value.x, color.Value.y, color.Value.z);
      mat->AlphaModulate(color.Value.w);

      worldMaterials.at(i) = color;
    }
  }
}