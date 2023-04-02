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
        ball.UpdateSprite(&cannonBallSprite);
        if (cell.id != -1) //check if the player is the parent (only cell which's id doesn't change (the default is -1))
            ball.parent = 1;
        ball.attack = cell.equipment[cannonSlot].boost;
        cannonBalls.push_back(ball);
    }
}

void ApplyHeal()
{
    player.hp += heal;
    if (player.hp > player.maxHp)
        player.hp = player.maxHp;
    heal = 0;
}