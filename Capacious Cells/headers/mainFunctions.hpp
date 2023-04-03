#pragma once
#include "libraries.hpp"

//outside game loop
void SetupWindow()
{
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
    std::cout << monitorSize.x << " " << monitorSize.y << ", " << windowSize.x << " " << windowSize.y << std::endl;

    //misc.
    SetTargetFPS(60);
    SetExitKey(KEY_END);
}

void LoadTextures()
{
    //cells
    whiteCellSprite = LoadTexture(Path(assetsFolder, "cells/WhiteCell", pngExtention).string().c_str());
    enemyCellSprite = LoadTexture(Path(assetsFolder, "cells/EnemyCell", pngExtention).string().c_str());
    playerSprite[0] = LoadTexture(Path(assetsFolder, "cells/PlayerCell", pngExtention).string().c_str());
    playerSprite[1] = LoadTexture(Path(assetsFolder, "cells/HungryPlayerCell", pngExtention).string().c_str());

    //UI
    evolutionCardSprite = LoadTexture(Path(assetsFolder, "UI/EvolutionCard", pngExtention).string().c_str());
    cardButtonSprite = LoadTexture(Path(assetsFolder, "UI/CardButton", pngExtention).string().c_str());

    //backgrounds
    //background = LoadTexture(Path(assetsFolder, "screens/Background", pngExtention).string().c_str());
    deathScreen = LoadTexture(Path(assetsFolder, "screens/DeathScreen", pngExtention).string().c_str());
    darkenedScreen = LoadTexture(Path(assetsFolder, "screens/DarkenedScreen", pngExtention).string().c_str());

    //balls
    cannonBallSprite = LoadTexture(Path(assetsFolder, "balls/CannonBall", pngExtention).string().c_str());
    foodSprite = LoadTexture(Path(assetsFolder, "balls/Food", pngExtention).string().c_str());

    //debug
    missingTexture = LoadTexture("../../assets/MissingTexture.png");
    playerSprite[2] = LoadTexture("../../assets/720p/cells/PlayerCell.png");
}
void SetupObjects()
{
    //setup prefabs
        //parts
    toxin.sprite.push_back(LoadTexture(Path(assetsFolder, "equipment/toxin(card)", pngExtention).string().c_str()));
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
}

//game screen
void Sorting()
{
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
}
void KillObjects()
{
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
}

void CannonBallActions()
{
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
}
void EnemyActions()
{
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

        MoveEnemy(*enemyOnScreen[i]);
    }
}
void PlayerActions()
{
    //rotate
    player.rotation = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - player.pos.y,
        GetScreenToWorld2D(GetMousePosition(), camera).x - player.pos.x) * toDegrees;
    player.MapRotation();

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
    if (heal > 0)
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

    //tail managment
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
}

void VariableResets()
{
    doubleMovementKeys = 0;
    movementControls = 1;
    camera.target = player.pos;
    inGameFrames++;
}

//evolve screen
void DrawUI()
{
    Vector2 evolutionCardPosition{ GetScreenWidth() / 2 - evolutionCardSprite.width / 2, 800 };
    Vector2 cardButtonPosition{ GetScreenWidth() / 2 - cardButtonSprite.width / 2, 1000 };

    DrawTexture(evolutionCardSprite, evolutionCardPosition.x - Pixels(50), evolutionCardPosition.y, WHITE);
    DrawTexture(evolutionCardSprite, evolutionCardPosition.x - Pixels(25), evolutionCardPosition.y, WHITE);
    DrawTexture(evolutionCardSprite, evolutionCardPosition.x, evolutionCardPosition.y, WHITE);
    DrawTexture(evolutionCardSprite, evolutionCardPosition.x + Pixels(25), evolutionCardPosition.y, WHITE);
    DrawTexture(evolutionCardSprite, evolutionCardPosition.x + Pixels(50), evolutionCardPosition.y, WHITE);

    DrawTexture(cardButtonSprite, cardButtonPosition.x - Pixels(50), cardButtonPosition.y, WHITE);
    DrawTexture(cardButtonSprite, cardButtonPosition.x - Pixels(25), cardButtonPosition.y, WHITE);
    DrawTexture(cardButtonSprite, cardButtonPosition.x, cardButtonPosition.y, WHITE);
    DrawTexture(cardButtonSprite, cardButtonPosition.x + Pixels(25), cardButtonPosition.y, WHITE);
    DrawTexture(cardButtonSprite, cardButtonPosition.x + Pixels(50), cardButtonPosition.y, WHITE);

    DrawTexturePro(spike.sprite[0], spikeSource, spikeDest, spikeOrigin, -90, WHITE);
    DrawTexturePro(cannon.sprite[0], cannonSource, cannonDest, cannonOrigin, -90, WHITE);
    DrawTexturePro(tail.sprite[0], tailSource, tailDest, tailOrigin, -90, WHITE);
    DrawTexturePro(bristles.sprite[0], bristlesSource, bristlesDest, bristlesOrigin, -90, WHITE);
    DrawTexturePro(toxin.sprite[0], toxinSource, toxinDest, toxinOrigin, -90, WHITE);
}
void Evolve()
{
    Vector2 evolutionCardPosition{ GetScreenWidth() / 2 - evolutionCardSprite.width / 2, 800 };
    Vector2 cardButtonPosition{ GetScreenWidth() / 2 - cardButtonSprite.width / 2, 1000 };

    spikePosition = { evolutionCardPosition.x - Pixels(50) + evolutionCardSprite.width / 2, evolutionCardPosition.y + evolutionCardSprite.height / 2 };
    spikeDest = { spikePosition.x, spikePosition.y, (float)spike.sprite[0].width, (float)spike.sprite[0].height };
    spikeSource = { 0, 0, (float)spike.sprite[0].width, (float)spike.sprite[0].height };
    spikeOrigin = { (float)spike.sprite[0].width / 2, (float)spike.sprite[0].height / 2 };

    cannonPosition = { evolutionCardPosition.x - Pixels(25) + evolutionCardSprite.width / 2, evolutionCardPosition.y + evolutionCardSprite.height / 2 };
    cannonDest = { cannonPosition.x, cannonPosition.y, (float)cannon.sprite[0].width, (float)cannon.sprite[0].height };
    cannonSource = { 0, 0, (float)cannon.sprite[0].width, (float)cannon.sprite[0].height };
    cannonOrigin = { (float)cannon.sprite[0].width / 2, (float)cannon.sprite[0].height / 2 };

    bristlesPosition = { evolutionCardPosition.x + evolutionCardSprite.width / 2, evolutionCardPosition.y + evolutionCardSprite.height / 2 };
    bristlesDest = { bristlesPosition.x, bristlesPosition.y, (float)bristles.sprite[0].width, (float)bristles.sprite[0].height };
    bristlesSource = { 0, 0, (float)bristles.sprite[0].width, (float)bristles.sprite[0].height };
    bristlesOrigin = { (float)bristles.sprite[0].width / 2, (float)bristles.sprite[0].height / 2 };

    tailPosition = { evolutionCardPosition.x + Pixels(25) + evolutionCardSprite.width / 2, evolutionCardPosition.y + evolutionCardSprite.height / 2 };
    tailDest = { tailPosition.x, tailPosition.y, (float)tail.sprite[0].width, (float)tail.sprite[0].height };
    tailSource = { 0, 0, (float)tail.sprite[0].width, (float)tail.sprite[0].height };
    tailOrigin = { (float)tail.sprite[0].width / 2, (float)tail.sprite[0].height / 2 };

    toxinPosition = { evolutionCardPosition.x + Pixels(50) + evolutionCardSprite.width / 2, evolutionCardPosition.y + evolutionCardSprite.height / 2 };
    toxinDest = { toxinPosition.x, toxinPosition.y, (float)toxin.sprite[0].width, (float)toxin.sprite[0].height };
    toxinSource = { 0, 0, (float)toxin.sprite[0].width, (float)toxin.sprite[0].height };
    toxinOrigin = { (float)toxin.sprite[0].width / 2, (float)toxin.sprite[0].height / 2 };

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
        player.Equip(targetEquipment, targetSlot);
        targetEquipment = -1;
        targetSlot = -1;
    }
}

//other
void SetupVariables()
{
    //setup variables
    doubleMovementKeys = 0, playerEP = 3, targetEquipment = -1, targetSlot = -1;

    //setup margins
    cellMargin[0] = { Pixels(13), Pixels(-.5) };
    cellMargin[1] = { Pixels(-1.5), Pixels(9) };
    cellMargin[2] = { Pixels(-13), Pixels(-.5) };
    cellMargin[3] = { Pixels(-1.5), Pixels(-9) };

    //setup enemies
    for (int i = 0; i < enemies; i++)
    {
        enemy[i].Setup(prefabEnemy[int(floor(i/10))], i);
        enemy[i].currentSprite = enemy[i].sprite[0];
    }

    for (int i = 0; i < enemies; i++)
        enemy[i].pos = { CENTER.x + Pixels(randomCoordinate()), CENTER.y + Pixels(randomCoordinate()) };

    //setup food
    for (int i = 0; i < foods; i++)
        food[i].eaten = 0;
    food[0].pos = { CENTER.x + Pixels(30), CENTER.y + Pixels(30) };
    food[1].pos = { CENTER.x + Pixels(30), CENTER.y - Pixels(30) };
    food[2].pos = { CENTER.x - Pixels(30), CENTER.y + Pixels(30) };
    food[3].pos = { CENTER.x - Pixels(30), CENTER.y - Pixels(30) };

    //setup player
    player.pos = CENTER;
    player.rotation = 0;
    player.speed = Pixels(1);

    player.knockbackFrames = 0;
    player.invincibilityFrames = 0;

    player.UpdateSprite(&playerSprite[0]);
    player.UpdateSprite(&playerSprite[1], 1);
    player.UpdateSprite(&whiteCellSprite, 2);
    player.currentSprite = player.sprite[0];

    //setup misc.
    camera = { CENTER, player.pos, 0, 1.0f };

    //clear vectors
    cannonBalls.clear();
    enemyOnScreen.clear();

    //debug
    for (int i = 0; i < slots; i++)
        player.Equip(4, i);
    //player.Equip(1, 0);
}
void ScreenListens()
{
    switch (screen)
    {
    case 0:
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
        break;

    case 1:
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
        break;

    case 2:
        if (IsKeyPressed(KEY_ENTER))
        {
            SetMousePosition(CENTER.x + Pixels(20), CENTER.y);
            screen = 0;
        }
        break;

    case 3:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            SetMousePosition(wasMousePos.x, wasMousePos.y);
            screen = 0;
        }
        break;

    case 4:
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
        break;
    }
}