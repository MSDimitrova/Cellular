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