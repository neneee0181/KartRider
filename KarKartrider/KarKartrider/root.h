#pragma once

#include "ModeManager.h"

#include "RoadModel.h"
#include "KartModel.h"
// Models
std::vector<Model*> karts;
std::vector<Model*> roads;

// mode
ModeManager MM;

int window_width = 800;
int window_height = 600;
int location_x = 0;
int location_y = 0;

GLvoid Reshape(int w, int h) {
    glViewport(location_x, location_y, w, h);
    width = w;
    height = h;
}
