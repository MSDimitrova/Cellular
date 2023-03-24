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
    int screen = 2; //0 = game screen, 1 = evolve screen, 2 = main menu, 3 = pause screen
    if (windowSize.x == 1920 && windowSize.y == 1080)
        resolution = 1;
    cellMargin[0] = { Pixels(13), Pixels(-.5) };
    cellMargin[1] = { Pixels(-1.5), Pixels(9.1) };
    cellMargin[2] = { Pixels(-13), Pixels(-.5) };
    cellMargin[3] = { Pixels(-1.5), Pixels(-9.1) };

    //load textures
    missingTexture = LoadTexture("../../assets/MissingTexture.png");
    Texture2D playerSprite[3] = { LoadTexture(Path(assetsFolder, "cells/PlayerCell", pngExtention).string().c_str()),
        LoadTexture("../../assets/720p/cells/PlayerCell.png"), LoadTexture("../../assets/1080p/cells/PlayerCell.png") };
    //background = LoadTexture("../../assets/Background.png");
    
    //setup objects
    std::vector<const char*> prefabPartName{ "equipment/spike" , "equipment/canon", "equipment/bristles", "equipment/tail" };
    for (int i = 0; i < prefabPartName.size(); i++)
        prefabPart[i]->Setup(prefabPartName[i]);

    std::vector<const char*> prefabEnemyName{ "cells/enemy0"/* , "cells/enemy1", "cells/enemy2", "cells/enemy3"*/};
    for (int i = 0; i < prefabEnemies; i++)
        prefabEnemy[i].Setup(prefabEnemyName[i]);
    SetupEnemyPos();
    for (int i = 0; i < enemies; i++)
        enemy[i].Setup(prefabEnemy[0], i);

    player.UpdateSprite(&playerSprite[0]);
    player.pos = CENTER;
    player.speed = Pixels(1);

    camera = { CENTER, player.pos, 0, 1.0f };

    //debug
    for (int i = 0; i < 4; i++)
        Equip(enemy[0], 0, i);

    for (int i = 0; i < 4; i++)
        Equip(player, 0, i);

    //start game runtime
    while (!WindowShouldClose())
    {
        pause = screen;
        switch (screen)
        {
            case 0: //game screen
            {
                UpdateScreen(BLACK);

                //choosing what to render
                screenCheck[0] = GetScreenToWorld2D({ windowSize.x + Pixels(20), windowSize.y + Pixels(20) }, camera);
                screenCheck[1] = GetScreenToWorld2D({ Pixels(-20), windowSize.y + Pixels(20) }, camera);
                screenCheck[2] = GetScreenToWorld2D({ Pixels(-20), Pixels(-20) }, camera);
                screenCheck[3] = GetScreenToWorld2D({ windowSize.x + Pixels(20), Pixels(-20) }, camera);

                enemyOnScreen.clear();
                for (int i = 0; i < enemies; i++)
                    if (IsOnScreen(enemy[i].pos))
                        enemyOnScreen.push_back(&enemy[i]);

                for (int i = 0; i < enemyOnScreen.size(); i++)
                    for (int j = 0; j < 4; j++)
                    {
                        if (player.equipment[j].name == "spike")
                            if (SpikeCollision(*enemyOnScreen[i], player.equipment[j]))
                                enemyOnScreen[i]->UpdateSprite(&missingTexture);
                        if (enemyOnScreen[i]->equipment[j].name == "spike")
                            if (SpikeCollision(player, enemyOnScreen[i]->equipment[j]))
                                player.UpdateSprite(&missingTexture);
                    }

                //movement
                if (IsKeyDown(KEY_S) && IsKeyDown(KEY_D))
                    MoveInTwoDirections(player, { 1, 1 }, 1, doubleMovementKeys);
                if (IsKeyDown(KEY_S) && IsKeyDown(KEY_A))
                    MoveInTwoDirections(player, { -1, 1 }, 3, doubleMovementKeys);
                if (IsKeyDown(KEY_W) && IsKeyDown(KEY_A))
                    MoveInTwoDirections(player, { -1, -1 }, 5, doubleMovementKeys);
                if (IsKeyDown(KEY_W) && IsKeyDown(KEY_D))
                    MoveInTwoDirections(player, { 1, -1 }, 7, doubleMovementKeys);

                if (!doubleMovementKeys)
                {
                    MoveInOneDirection(KEY_D, KEY_A, 0);
                    MoveInOneDirection(KEY_S, KEY_W, 2);
                    MoveInOneDirection(KEY_A, KEY_D, 4);
                    MoveInOneDirection(KEY_W, KEY_S, 6);
                }

                //map player's rotating direction
                player.rotationIndex = floor(player.rotation / 45);
                if (player.rotationIndex < 0)
                    player.rotationIndex += 8;

                //variable sets and resets
                doubleMovementKeys = 0;
                camera.target = player.pos;

                //game screen listens
                if (IsKeyPressed(KEY_E))
                    screen = 1;
                else if (IsKeyPressed(KEY_ESCAPE))
                {
                    wasMousePos = GetMousePosition();
                    screen = 3;
                }

                //debug
                if (IsKeyPressed(KEY_ONE))
                    player.UpdateSprite(&playerSprite[0]);
                else if (IsKeyPressed(KEY_TWO))
                    player.UpdateSprite(&playerSprite[2]);
                else if (IsKeyPressed(KEY_THREE))
                    std::cout << player.movementIndex << std::endl;
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
                    screen = 3;
                }
                else if (IsKeyPressed(KEY_E))
                {
                    SetMousePosition(CENTER.x + Pixels(20), CENTER.y);
                    screen = 0;
                }
            }
            break;

            case 2: //main menu
            {
                UpdateScreen(DARKGRAY);



                //game screen listens
                if (IsKeyPressed(KEY_ENTER))
                {
                    SetMousePosition(CENTER.x + Pixels(20), CENTER.y);
                    screen = 0;
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
                    screen = 0;
                }
            }
            break;
        }
    }
    CloseWindow();
}