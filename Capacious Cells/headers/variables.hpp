#pragma once
#include "libraries.hpp"

//used only in main
bool doubleMovementKeys, playerHadTail = 0;
int playerEP, targetEquipment, targetSlot;
int screen; //0 = game screen, 1 = evolve screen, 2 = main menu, 3 = pause screen, 4 - death screen
Vector2 wasMousePos;

//dummies
bool dummyBool;
const char* dummyConstCharPtr;
Vector2 dummyVector2;
Texture2D dummyTexture;

//temporaries
int tempInt;
float tempRotation;
Vector2 tempPos, tempV2;
Color tempColor;

//path parts
const char* pngExtention = ".png";
const char* jsonExtention = ".json";
const char* resolutionPath[2] = { "720p/", "1080p/" };
const char* assetsFolder = "../../assets/";
const char* prefabsFolder = "../../prefabs/";

//constants
const int foods = 4, enemies = 4, prefabEnemies = 1, initialPlayerHp = 150, parts = 5, slots = 4;
const float directionRotation[8] = { 0, 45, 90, 135, 180, 225, 270, 315 };
const float toDegrees = 57.29578f;
const Vector2 directionPos[8] = { {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1} };

//constant after first assignment
bool resolution = 0;
Vector2 CENTER, windowSize, cellMargin[slots];

//sprites
Texture2D whiteCellSprite, enemyCellSprite, playerSprite[3];
Texture2D background, deathScreen, darkenedScreen;
Texture2D cannonBallSprite, foodSprite;
Texture2D missingTexture;

//objects
Camera2D camera;

//names
std::vector<const char*> prefabPartName{ "equipment/spike" , "equipment/cannon", "equipment/bristles", "equipment/tail" , "equipment/toxin" },
	prefabEnemyName{ "cells/enemy0"/* , "cells/enemy1", "cells/enemy2", "cells/enemy3"*/ };

//player related
bool pause, movementControls = 1, hunger = 0;
int heal = 0;

//misc.
int inGameFrames = 0;
std::vector<int> toErase;
Vector2 screenCheck[4];
Vector2 cellPoint[6], spikePoint[3], ballPoint[4], toxinPoint[4]; //collider points