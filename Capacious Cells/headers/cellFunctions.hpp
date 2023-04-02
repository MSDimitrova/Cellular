#pragma once
#include "libraries.hpp"

void Equip(Cell& cell, int _part, int _slot)
{
    if (cell.equipment[_slot].name == "tail" && cell.id == -1)
        playerHadTail = 1;
    cell.equipment[_slot] = defaultEquipment; //clean equipment
    cell.equipment[_slot].Setup(*prefabPart[_part]);
    cell.equipment[_slot].slot = _slot;
}

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
    enemyPos[0] = { Pixels(60), 0 };
    enemyPos[1] = { 0, Pixels(60) };
    enemyPos[2] = { Pixels(-60), 0 };
    enemyPos[3] = { 0, Pixels(-60) };

    for (int i = 0; i < enemies; i++)
    {
        enemy[i].Setup(prefabEnemy[0], i);
        enemy[i].UpdateSprite(&enemyCellSprite);
    }

    //setup player
    player.pos = CENTER;
    player.rotation = 0;
    player.speed = Pixels(1);

    player.knockbackFrames = 0;
    player.invincibilityFrames = 0;

    player.UpdateSprite(&playerSprite[0]);
    player.UpdateSprite(&playerSprite[1], 1);
    player.currentSprite = player.sprite[0];

    //setup misc.
    camera = { CENTER, player.pos, 0, 1.0f };
    cannonBallPrefab.Setup("cannonBall");

    //clear vectors
    cannonBalls.clear();
    enemyOnScreen.clear();

    //debug
    for (int i = 0; i < slots; i++)
        Equip(player, 4, i);

    for (int i = 0; i < enemies; i++)
        for (int j = 0; j < slots; j++)
            Equip(enemy[i], 0, j);
}

void MoveInOneDirection(KeyboardKey targetKey, KeyboardKey avoidKey, int index)
{
    if (IsKeyDown(targetKey))
    {
        if (!IsKeyDown(avoidKey))
            player.movementIndex = index;

        tempInt = player.CalculateSpeed();
        player.pos.x += tempInt * directionPos[index].x;
        player.pos.y += tempInt * directionPos[index].y;
    }
}
void MoveInTwoDirections(Cell& cell, Vector2 direction, int index, bool& refCheck = dummyBool)
{
    refCheck = 1;
    cell.movementIndex = index;

    tempInt = player.CalculateSpeed();
    cell.pos.x += round(sqrt(pow(tempInt, 2) / 2)) * direction.x;
    cell.pos.y += round(sqrt(pow(tempInt, 2) / 2)) * direction.y;
}

void TryShootingCannonball(Cell& cell, int cannonSlot)
{
    if (inGameFrames % 60 == 0 && inGameFrames > 0)
    {
        CannonBall ball;
        ball.rotation = AddRotation(cell.rotation, directionRotation[cannonSlot * 2]) / toDegrees;
        ball.pos = HypotenuseCoordinates(cell.equipment[cannonSlot].pos, Pixels(5), ball.rotation);
        ball.speed = Pixels(3);
        ball.UpdateSprite(&cannonBallPrefab.sprite[0]);
        if (cell.id != -1) //check if the player is the parent (only cell which's id doesn't change (the default is -1))
            ball.parent = 1;
        ball.attack = cell.equipment[cannonSlot].boost;
        cannonBalls.push_back(ball);
    }
}