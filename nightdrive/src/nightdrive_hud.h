#pragma once

// OpenGL 3.3 Core compatible HUD (initialize after GLEW is initialized).
bool initNightDriveHud();
void drawNightDriveHud(int width, int height, int fps, float speed,
                       bool paused, bool streetLightsOn, bool headlightsOn,
                       bool showHelp, const char* lightingMode);
