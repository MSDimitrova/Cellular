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
        //cells
    whiteCellSprite = LoadTexture(Path(assetsFolder, "cells/WhiteCell", pngExtention).string().c_str());
    enemyCellSprite = LoadTexture(Path(assetsFolder, "cells/EnemyCell", pngExtention).string().c_str());
    playerSprite[0] = LoadTexture(Path(assetsFolder, "cells/PlayerCell", pngExtention).string().c_str());
    playerSprite[1] = LoadTexture(Path(assetsFolder, "cells/HungryPlayerCell", pngExtention).string().c_str());

        //backgrounds
    //Texture2D background = LoadTexture(Path(assetsFolder, "screens/Background", pngExtention).string().c_str());
    Texture2D deathScreen = LoadTexture(Path(assetsFolder, "screens/DeathScreen", pngExtention).string().c_str());
    Texture2D darkenedScreen = LoadTexture(Path(assetsFolder, "screens/DarkenedScreen", pngExtention).string().c_str());

        //balls
    cannonBallSprite = LoadTexture(Path(assetsFolder, "balls/CannonBall", pngExtention).string().c_str());
    foodSprite = LoadTexture(Path(assetsFolder, "balls/Food", pngExtention).string().c_str());

        //debug
    missingTexture = LoadTexture("../../assets/MissingTexture.png");
    playerSprite[2] = LoadTexture("../../assets/720p/cells/PlayerCell.png");

    //setup prefabs
        //parts
    for (int i = 0; i < prefabPartName.size(); i++)
        prefabPart[i]->Setup(prefabPartName[i]);

        //enemies
    for (int i = 0; i < prefabEnemies; i++)
        prefabEnemy[i].Setup(prefabEnemyName[i]);

    //setup sprites
        //food
    for (int i = 0; i < foods; i++)
        food[i].UpdateSprite(&foodSprite);

        //enemies
    for (int i = 0; i < enemies; i++)
    {
        enemy[i].UpdateSprite(&enemyCellSprite);
        enemy[i].UpdateSprite(&whiteCellSprite, 1);
    }

    //prepare game
    screen = 2;
    SetupVariables();

    //start game runtime
    while (!WindowShouldClose())
    {
        pause = screen;

        //Update screen
        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(10, 10);
        BeginMode2D(camera);

        //DrawTexture(background, CENTER.x, CENTER.y, WHITE);
        DrawCircle(CENTER.x, CENTER.y, 5, RED);

        for (int i = 0; i < enemyOnScreen.size(); i++)
            DrawCell(*enemyOnScreen[i]);
        for (int i = 0; i < cannonBalls.size(); i++)
            DrawGameObject(cannonBalls[i]);
        for (int i = 0; i < foodOnScreen.size(); i++)
            DrawGameObject(*foodOnScreen[i]);
        DrawCell(player);

        switch (screen)
        {
            case 0: //game screen
            {
                EndMode2D();
                EndDrawing();

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

                    //sort food
                foodOnScreen.clear();
                for (int i = 0; i < foods; i++)
                    if (IsOnScreen(food[i].pos))
                        foodOnScreen.push_back(&food[i]);

                //kill enemies
                toErase.clear();
                for (int i = 0; i < enemyOnScreen.size(); i++)
                    if (enemyOnScreen[i]->hp < 1)
                        toErase.push_back(i);

                for (int i = 0; i < toErase.size(); i++)
                {
                    tempInt = toErase[i] - i; //DON'T TOUCH!!!
                    enemyOnScreen.erase(enemyOnScreen.begin() + tempInt);
                }

                //kill food
                toErase.clear();
                for (int i = 0; i < foodOnScreen.size(); i++)
                    if (foodOnScreen[i]->eaten == 1)
                        toErase.push_back(i);

                for (int i = 0; i < toErase.size(); i++)
                {
                    tempInt = toErase[i] - i; //DON'T TOUCH!!!
                    foodOnScreen.erase(foodOnScreen.begin() + tempInt);
                }

                //cannon ball actions
                toErase.clear();
                for (int i = 0; i < cannonBalls.size(); i++)
                    if (IsOnScreen(cannonBalls[i].pos))
                    {

                        cannonBalls[i].MoveCannonBall();
                        if (cannonBalls[i].parent)
                        {
                            if (CannonBallCollision(player, cannonBalls[i]))
                                toErase.push_back(i);
                        }
                        else
                            for (int j = 0; j < enemyOnScreen.size(); j++)
                                if (CannonBallCollision(*enemyOnScreen[j], cannonBalls[i]))
                                    toErase.push_back(i);
                    }
                    else
                        toErase.push_back(i);

                    //destroy cannon balls
                for (int i = 0; i < toErase.size(); i++)
                {
                    tempInt = toErase[i] - i; //DON'T TOUCH!!!
                    cannonBalls.erase(cannonBalls.begin() + tempInt);
                }

                //enemy actions
                for (int i = 0; i < enemyOnScreen.size(); i++)
                {
                    //equipment
                    for (int j = 0; j < slots; j++)
                    {
                        if (player.equipment[j].name == "spike")
                            SpikeCollision(*enemyOnScreen[i], player, j);
                        if (enemyOnScreen[i]->equipment[j].name == "spike")
                            SpikeCollision(player, *enemyOnScreen[i], j);

                        if (enemyOnScreen[i]->equipment[j].name == "cannon")
                            TryShootingCannonball(*enemyOnScreen[i], j);

                        if (player.equipment[j].name == "toxin")
                            ToxinCollision(*enemyOnScreen[i], player, j);
                        if (enemyOnScreen[i]->equipment[j].name == "toxin")
                            ToxinCollision(player, *enemyOnScreen[i], j);
                    }

                    //effects and damage
                    enemyOnScreen[i]->ApplyInvincibility();
                    enemyOnScreen[i]->ApplyKnockback();
                    enemyOnScreen[i]->ApplyDamage();
                }

                //player actions
                    //rotate
                player.rotation = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - player.pos.y,
                    GetScreenToWorld2D(GetMousePosition(), camera).x - player.pos.x) * toDegrees;
                if (player.rotation < 0)
                    player.rotation += 360;

                    //map rotation
                player.rotationIndex = floor(player.rotation / 45);
                if (player.rotationIndex < 0)
                    player.rotationIndex += 8;

                    //equipment
                for (int i = 0; i < 4; i++)
                {
                    if (player.equipment[i].name == "cannon")
                        TryShootingCannonball(player, i);
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
                if (hunger && foodOnScreen.size() > 0)
                {
                    tempPos = { player.pos.x - player.size.x / 2, player.pos.y - player.size.y / 2 };
                    tempV2 = { player.pos.x + player.size.x / 2, player.pos.y + player.size.y / 2 };
                    for (int i = 0; i < foodOnScreen.size(); i++)
                        foodOnScreen[i]->TryToGetEaten();
                }
                player.ApplyDamage();
                if(heal > 0)
                    ApplyHeal();

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

                    //hunger
                if (IsKeyPressed(KEY_Q))
                {
                    hunger = !hunger;
                    player.currentSprite = player.sprite[hunger];
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
                {
                    wasMousePos = GetMousePosition();
                    screen = 1;
                }
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
                    std::cout << player.hpText << std::endl;

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
                EndMode2D();
                EndDrawing();

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
                    else if (IsKeyPressed(KEY_FIVE))
                        targetEquipment = 4;
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
                    SetMousePosition(wasMousePos.x, wasMousePos.y);
                    screen = 0;
                }
            }
            break;

            case 2: //main menu
            {
                EndMode2D();
                EndDrawing();



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
                DrawTexture(darkenedScreen, GetScreenToWorld2D({ 0,0 }, camera).x, GetScreenToWorld2D({ 0,0 }, camera).y, WHITE);

                EndMode2D();
                EndDrawing();



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
                DrawTexture(deathScreen, GetScreenToWorld2D({ 0,0 }, camera).x, GetScreenToWorld2D({ 0,0 }, camera).y, WHITE);

                EndMode2D();
                EndDrawing();



                //game screen listens
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    SetupVariables();
                    screen = 2;
                }
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