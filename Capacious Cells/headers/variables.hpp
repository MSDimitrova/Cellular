#pragma once
#include "libraries.hpp"

//dummies
bool dummyBool;
const char* dummyConstCharPtr;
Texture2D dummyTexture;

//path parts
const char* pngExtention = ".png";
const char* jsonExtention = ".json";
const char* resolutionPath[2] = { "720p/", "1080p/" };
const char* assetsFolder = "../../assets/";
const char* prefabsFolder = "../../prefabs/";

//constants
const float directionRotation[8] = { 0, 45, 90, 135, 180, 225, 270, 315 };
const Vector2 directionPos[8] = { {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1} };

//objects
Texture2D missingTexture;
Camera2D camera;

//misc.
bool pause,resolution = 0;
Vector2 CENTER, windowSize, cellMargin[4];