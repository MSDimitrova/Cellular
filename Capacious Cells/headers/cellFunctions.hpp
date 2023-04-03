#pragma once
#include "libraries.hpp"

//movement
int CalculateSpeed()
{
    for (int i = 0; i < slots; i++)
        if (((AddIndex(player.movementIndex, -player.rotationIndex) == AddIndex(i * 2, 4)) || (AddIndex(player.movementIndex, -player.rotationIndex) == AddIndex(i * 2, 5)))
            && ((player.equipment[i].name == "bristles") || (player.equipment[i].name == "tail" && player.activeTail)))
            return player.speed + Pixels(player.equipment[i].boost);
    return player.speed;
}
void MoveInOneDirection(KeyboardKey targetKey, KeyboardKey avoidKey, int index)
{
    if (IsKeyDown(targetKey))
    {
        if (!IsKeyDown(avoidKey))
            player.movementIndex = index;

        player.pos.x += CalculateSpeed() * directionPos[index].x;
        player.pos.y += CalculateSpeed() * directionPos[index].y;
    }
}
void MoveInTwoDirections(Cell& cell, Vector2 direction, int index, bool& refCheck = dummyBool)
{
    refCheck = 1;
    cell.movementIndex = index;

    cell.pos.x += round(sqrt(pow(CalculateSpeed(), 2) / 2)) * direction.x;
    cell.pos.y += round(sqrt(pow(CalculateSpeed(), 2) / 2)) * direction.y;
}
void MoveEnemy(Cell& enemy)
{
    if (enemy.name == "enemy0")
    {
        enemy.rotation = atan2(player.pos.y - enemy.pos.y, player.pos.x - enemy.pos.x) * toDegrees;
        enemy.MapRotation();
        if (enemy.hp > 99)
            enemy.pos = HypotenuseCoordinates(enemy.pos, enemy.speed, enemy.rotation / toDegrees);
        else
            enemy.pos = HypotenuseCoordinates(enemy.pos, enemy.speed, AddRotation(enemy.rotation, 90) / toDegrees);
    }

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