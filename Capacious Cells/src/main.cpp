#include "../headers/libraries.hpp"

int main()
{
    //setup window
    InitWindow(1280, 720, "Capacious Cells");

    //size
    const Vector2 monitorSize = { GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()) };
    if (monitorSize.x >= 1920 && monitorSize.y >= 1080)
    {
        windowSize = { 1920, 1080 };
        resolution = 1;
    }
    SetWindowSize(windowSize.x, windowSize.y);

    //position
    CENTER = { { windowSize.x / 2 }, { windowSize.y / 2 } };
    SetWindowPosition(monitorSize.x / 2 - CENTER.x, monitorSize.y / 2 - CENTER.y);
    if ((monitorSize.x == 1280 && monitorSize.y == 720) || (monitorSize.x == 1920 && monitorSize.y == 1080))
        SetWindowState(FLAG_FULLSCREEN_MODE);

    //other
    SetTargetFPS(60);
    SetExitKey(KEY_END);

    //load textures
    missingTexture = LoadTexture("../../assets/MissingTexture.png");
    whiteCellSprite = LoadTexture(Path(assetsFolder, "cells/WhiteCell", pngExtention).string().c_str());
    enemyCellSprite = LoadTexture(Path(assetsFolder, "cells/EnemyCell", pngExtention).string().c_str());
    playerSprite[0] = LoadTexture(Path(assetsFolder, "cells/PlayerCell", pngExtention).string().c_str());

    playerSprite[1] = LoadTexture("../../assets/720p/cells/PlayerCell.png");
    playerSprite[2] = LoadTexture("../../assets/1080p/cells/PlayerCell.png");
    //background = LoadTexture("../../assets/Background.png");

    //setup prefabs
        //parts
    for (int i = 0; i < prefabPartName.size(); i++)
        prefabPart[i]->Setup(prefabPartName[i]);

        //enemies
    for (int i = 0; i < prefabEnemies; i++)
        prefabEnemy[i].Setup(prefabEnemyName[i]);

    screen = 2;
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
                    //setup screen checks
                screenCheck[0] = GetScreenToWorld2D({ windowSize.x + Pixels(20), windowSize.y + Pixels(20) }, camera);
                screenCheck[1] = GetScreenToWorld2D({ Pixels(-20), windowSize.y + Pixels(20) }, camera);
                screenCheck[2] = GetScreenToWorld2D({ Pixels(-20), Pixels(-20) }, camera);
                screenCheck[3] = GetScreenToWorld2D({ windowSize.x + Pixels(20), Pixels(-20) }, camera);

                //sort enemies
                enemyOnScreen.clear();
                for (int i = 0; i < enemies; i++)
                    if (IsOnScreen(enemy[i].pos))
                        enemyOnScreen.push_back(&enemy[i]);

                //kill cells
                    //kill enemies
                toErase.clear();
                for (int i = 0; i < enemyOnScreen.size(); i++)
                    if (enemyOnScreen[i]->hp < 1)
                        toErase.push_back(i);

                for (int i = 0; i < toErase.size(); i++)
                {
                    tempInt = toErase[i] - i; //DON'T TOUCH!!!
                    enemyOnScreen.erase(enemyOnScreen.begin() + tempInt);
                    //enemy.erase(enemy.begin() + tempInt);
                }

                //canon ball actions
                toErase.clear();
                for (int i = 0; i < canonBalls.size(); i++)
                    if (IsOnScreen(canonBalls[i].pos))
                    {

                        canonBalls[i].MoveCanonBall();
                        if (canonBalls[i].parent)
                        {
                            if (CanonBallCollision(player, canonBalls[i]))
                                toErase.push_back(i);
                        }
                        else
                            for (int j = 0; j < enemyOnScreen.size(); j++)
                                if (CanonBallCollision(*enemyOnScreen[j], canonBalls[i]))
                                    toErase.push_back(i);
                    }
                    else
                        toErase.push_back(i);

                    //destroy canon balls
                for (int i = 0; i < toErase.size(); i++)
                {
                    tempInt = toErase[i] - i; //DON'T TOUCH!!!
                    canonBalls.erase(canonBalls.begin() + tempInt);
                }

                //enemy actions
                for (int i = 0; i < enemyOnScreen.size(); i++)
                {
                    //equipment
                    for (int j = 0; j < 4; j++)
                    {
                        if (player.equipment[j].name == "spike")
                            SpikeCollision(*enemyOnScreen[i], player, j);
                        if (enemyOnScreen[i]->equipment[j].name == "spike")
                            SpikeCollision(player, *enemyOnScreen[i], j);

                        if (enemyOnScreen[i]->equipment[j].name == "canon")
                            TryShootingCanonball(*enemyOnScreen[i], j);
                    }

                    //effects and damage
                    enemyOnScreen[i]->ApplyInvincibility();
                    enemyOnScreen[i]->ApplyKnockback();
                    enemyOnScreen[i]->ApplyDamage();
                }

                //player actions
                    //map player's rotating direction
                player.rotationIndex = floor(player.rotation / 45);
                if (player.rotationIndex < 0)
                    player.rotationIndex += 8;

                    //equipment
                for (int i = 0; i < 4; i++)
                {
                    if (player.equipment[i].name == "canon")
                        TryShootingCanonball(player, i);
                    if (player.equipment[i].name == "tail")
                        if (IsKeyPressed(KEY_LEFT_SHIFT) && player.cooldownTailFrames == 0 && player.activeTailFrames == 0)
                        {
                            player.activeTail = 1;
                            player.activeTailFrames = 120;
                        }
                }

                    //effects and damage
                player.ApplyInvincibility();
                if (player.ApplyKnockback())
                    movementControls = 0;
                player.ApplyDamage();

                    //movement
                if (movementControls)
                {
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
                }

                //variable sets and resets
                if (playerHadTail)
                {
                    player.activeTailFrames = 0;
                    player.activeTail = 0;
                    playerHadTail = 0;
                }

                if (player.activeTailFrames > 0)
                {
                    player.activeTailFrames--;
                    if (player.activeTailFrames == 0)
                    {
                        player.activeTail = 0;
                        player.cooldownTailFrames = 240;
                    }
                }
                else if (player.cooldownTailFrames > 0)
                    player.cooldownTailFrames--;

                doubleMovementKeys = 0;
                movementControls = 1;
                camera.target = player.pos;
                inGameFrames++;

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
                    std::cout << player.id << std::endl;

                tempPos = GetWorldToScreen2D(player.pos, camera);
                tempV2 = HypotenuseCoordinates(tempPos, Pixels(25), player.rotation / toDegrees);
                DrawLine(tempPos.x, tempPos.y, tempV2.x, tempV2.y, YELLOW);

                if (player.hp < 1)
                {
                    player.hp = initialPlayerHp;
                    screen = 4;
                }
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
                SetupVariables();
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

            case 4: //death screen
            {
                UpdateScreen(GRAY);



                //game screen listens
                if (IsKeyPressed(KEY_ESCAPE))
                    screen = 2;
                else if (IsKeyPressed(KEY_ENTER))
                {
                    SetupVariables();
                    SetMousePosition(CENTER.x + Pixels(20), CENTER.y);
                    screen = 0;
                }
            }
            break;
        }
    }
    CloseWindow();
}