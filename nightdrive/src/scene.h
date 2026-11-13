#pragma once
#include <vector>
#include "common.h"

struct Building { vec3 pos, size, color; };      


struct StreetLight { vec3 pos; float radius; };

struct Scene {
    std::vector<Building>    buildings;
    std::vector<StreetLight> lights;     
    int lightPreset = 16;                

    void build();                        
    void setLightPreset(int total);      
    void update(float carZ);             
    void draw(const DrawBoxFn& box, float carZ, bool streetOn) const;

private:
    std::vector<vec3> lightBase_;        
};
