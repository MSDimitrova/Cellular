#pragma once
#include "libraries.hpp"

void Equip(Cell& cell, int _part, int _slot)
{
    cell.equipment[_slot].Setup(*prefabPart[_part]);
    cell.equipment[_slot].slot = _slot;
}
void MoveInOneDirection(KeyboardKey targetKey, KeyboardKey avoidKey, int index)
{
    if (IsKeyDown(targetKey))
    {
        player.pos.x += player.speed * directionPos[index].x;
        player.pos.y += player.speed * directionPos[index].y;
        if (!IsKeyDown(avoidKey))
            player.movementIndex = index;
    }
}
void MoveInTwoDirections(Cell& cell, Vector2 direction, int index = -1, bool& refCheck = dummyBool)
{
    cell.pos.x += round(sqrt(pow(cell.speed, 2) / 2)) * direction.x;
    cell.pos.y += round(sqrt(pow(cell.speed, 2) / 2)) * direction.y;
    if(index != -1)
        cell.movementIndex = index;
    refCheck = 1;
}
void SetupEnemyPos()
{
    enemyPos[0] = { Pixels(60), 0 };
    enemyPos[1] = { 0, Pixels(60) };
    enemyPos[2] = { Pixels(-60), 0 };
    enemyPos[3] = { 0, Pixels(-60) };
}

void TryShootingCanonball(Cell& cell, int canonSlot)
{
    if (inGameFrames % 60 == 0 && inGameFrames > 0)
    {
        CanonBall ball;
        ball.pos = cell.equipment[canonSlot].pos;
        ball.rotation = AddRotation(cell.rotation, directionRotation[canonSlot * 2]) / toDegrees;
        ball.speed = Pixels(3);
        ball.UpdateSprite(&canonBallPrefab.sprite[0]);
        if (cell.id != -1) //check if the player is the parent (only cell which's id doesn't change (the default is -1))
            ball.parent = 1;
        canonBalls.push_back(ball);
    }
}