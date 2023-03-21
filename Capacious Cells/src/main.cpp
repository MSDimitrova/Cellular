#include "../headers/libraries.hpp"

int main()
{
    //setup window
    InitWindow(1280, 720, "Capacious Cells");

        //size
    const Vector2 monitorSize = { GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()) };
    if ((monitorSize.x >= 1920 && monitorSize.y >= 1080))
        windowSize = { 1920, 1080 };
    SetWindowSize(windowSize.x, windowSize.y);

        //position
    CENTER = { { windowSize.x / 2 }, { windowSize.y / 2 } };
    SetWindowPosition(monitorSize.x / 2 - CENTER.x, monitorSize.y / 2 - CENTER.y);
    //std::cout << GetTime() << std::endl;
    if ((monitorSize.x == 1280 && monitorSize.y == 720) || (monitorSize.x == 1920 && monitorSize.y == 1080))
        SetWindowState(FLAG_FULLSCREEN_MODE);

        //other
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    //setup variables
    bool doubleMovementKeys = 0;
    int playerEP = 3, targetEquipment = -1, targetSlot = -1;
    Vector2 wasMousePos;
    int gameScreen = 2; //0 = game screen, 1 = evolve screen, 2 = main menu, 3 = pause screen
    if (windowSize.x == 1920 && windowSize.y == 1080)
        resolution = 1;
    cellMargin[0] = { Pixels(13), Pixels(-.5) };
    cellMargin[1] = { Pixels(-1.5), Pixels(9) };
    cellMargin[2] = { Pixels(-13), Pixels(-.5) };
    cellMargin[3] = { Pixels(-1.5), Pixels(-9) };

    //load textures
    missingTexture = LoadTexture("../../assets/MissingTexture.png");
    Texture2D playerSprite[3] = { LoadTexture(Path(assetsFolder, "cells/PlayerCell", pngExtention).string().c_str()),
        LoadTexture("../../assets/720p/cells/PlayerCell.png"), LoadTexture("../../assets/1080p/cells/PlayerCell.png") };
    //background = LoadTexture("../../assets/Background.png");
    
    //setup objects
    std::vector<const char*> prefabName{ "equipment/spike" , "equipment/canon", "equipment/bristles", "equipment/tail" };
    for (int i = 0; i < prefabName.size(); i++)
        prefabs[i]->Setup(prefabName[i]);

    player.UpdateSprite(&playerSprite[0]);
    player.pos = CENTER;
    camera = { CENTER, player.pos, 0, 1.0f };

    //debug
    //for (int i = 0; i < 4; i++)
        Equip(player, 0, 0);

    //start game runtime
    while (!WindowShouldClose())
    {
        pause = floor(gameScreen / 2);
        switch (gameScreen)
        {
            case 0: //game screen
            {
                UpdateScreen(BLACK);

                //movement
                if (IsKeyDown(KEY_W) && IsKeyDown(KEY_A))
                    MoveInTwoDirections(player.speed, -1, -1, player.pos, doubleMovementKeys);
                if (IsKeyDown(KEY_W) && IsKeyDown(KEY_D))
                    MoveInTwoDirections(player.speed, 1, -1, player.pos, doubleMovementKeys);
                if (IsKeyDown(KEY_S) && IsKeyDown(KEY_A))
                    MoveInTwoDirections(player.speed, -1, 1, player.pos, doubleMovementKeys);
                if (IsKeyDown(KEY_S) && IsKeyDown(KEY_D))
                    MoveInTwoDirections(player.speed, 1, 1, player.pos, doubleMovementKeys);
                if (!doubleMovementKeys)
                {
                    if (IsKeyDown(KEY_W))
                        player.pos.y -= player.speed;
                    if (IsKeyDown(KEY_S))
                        player.pos.y += player.speed;
                    if (IsKeyDown(KEY_A))
                        player.pos.x -= player.speed;
                    if (IsKeyDown(KEY_D))
                        player.pos.x += player.speed;
                }
                //map player's moving direction
                for (player.movementIndex = 0; player.movementIndex < 8; player.movementIndex++)
                    if (DirectionalSimilarity(player.pos, camera.target, directionPos[player.movementIndex]))
                        break;

                //map player's rotating direction
                player.rotationIndex = floor(player.rotation / 45);
                if (player.rotationIndex < 0)
                    player.rotationIndex += 8;

                //variable sets and resets
                doubleMovementKeys = 0;
                camera.target = player.pos;

                //game screen listens
                if (IsKeyPressed(KEY_E))
                    gameScreen = 1;
                else if (IsKeyPressed(KEY_ESCAPE))
                {
                    wasMousePos = GetMousePosition();
                    gameScreen = 3;
                }

                //debug
                if (IsKeyPressed(KEY_ONE))
                    player.UpdateSprite(&playerSprite[0]);
                else if (IsKeyPressed(KEY_TWO))
                    player.UpdateSprite(&playerSprite[2]);
                else if (IsKeyPressed(KEY_THREE))
                    std::cout << player.rotation << ", " << directionRotation[player.rotationIndex] << ", " << player.rotationIndex << std::endl;
            }
            break;

            case 1: //evolve screen
            {
                UpdateScreen(BLUE);

                if (targetEquipment == -1)
                {
                    if (IsKeyPressed(KEY_ONE))
                        targetEquipment = 0;
                    else if (IsKeyPressed(KEY_TWO))
                        targetEquipment = 1;
                    else if (IsKeyPressed(KEY_THREE))
                        targetEquipment = 2;
                    else if (IsKeyPressed(KEY_FOUR))
                        targetEquipment = 3;
                }
                else if (targetSlot == -1)
                {
                    if (IsKeyPressed(KEY_ONE))
                        targetSlot = 0;
                    else if (IsKeyPressed(KEY_TWO))
                        targetSlot = 1;
                    else if (IsKeyPressed(KEY_THREE))
                        targetSlot = 2;
                    else if (IsKeyPressed(KEY_FOUR))
                        targetSlot = 3;
                }
                else
                {
                    Equip(player, targetEquipment, targetSlot);
                    targetEquipment = -1;
                    targetSlot = -1;
                }

                //game screen listens
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    wasMousePos = GetMousePosition();
                    gameScreen = 3;
                }
                else if (IsKeyPressed(KEY_E))
                    gameScreen = 0;
            }
            break;

            case 2: //main menu
            {
                UpdateScreen(DARKGRAY);



                //game screen listens
                if (IsKeyPressed(KEY_ENTER))
                {
                    SetMousePosition(CENTER.x + Pixels(20), CENTER.y);
                    gameScreen = 0;
                }
            }
            break;

            case 3: //pause screen
            {
                UpdateScreen(DARKBLUE);



                //game screen listens
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    SetMousePosition(wasMousePos.x, wasMousePos.y);
                    gameScreen = 0;
                }
            }
            break;
        }
    }
    CloseWindow();
}