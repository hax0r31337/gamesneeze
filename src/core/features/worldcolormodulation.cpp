#include "features.hpp"

void Features::ColorModulation::updateColorModulation() {
    for(auto i = Interfaces::materialSystem->FirstMaterial(); i != Interfaces::materialSystem->InvalidMaterial(); i = Interfaces::materialSystem->NextMaterial(i)) {
        IMaterial* material = Interfaces::materialSystem->GetMaterial(i);
        if (material && (strstr(material->GetTextureGroupName(), "World"))) {
            auto col = CONFIGCOL("Visuals>World>World>World Color Modulation");
            material->AlphaModulate(col.Value.w);
            material->ColorModulate(col.Value.x, col.Value.y, col.Value.z);
        }
        if (material && strstr(material->GetTextureGroupName(), "SkyBox")) {
            auto col = CONFIGCOL("Visuals>World>World>SkyBox Color Modulation");
            material->AlphaModulate(col.Value.w);
            material->ColorModulate(col.Value.x,col.Value.y,col.Value.z);
        }
    }
}
