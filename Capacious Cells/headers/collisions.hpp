#pragma once
#include "libraries.hpp"

void CellLines(GameObject& cell)
{
    tempPos = GetWorldToScreen2D(cell.pos, camera);
    cellPoint[0] = HypotenuseCoordinates(tempPos, cell.size.x / 2, cell.rotation / toDegrees);
    cellPoint[1] = HypotenuseCoordinates(tempPos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation + 55) / toDegrees);
    cellPoint[2] = HypotenuseCoordinates(tempPos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation + 135) / toDegrees);
    cellPoint[3] = HypotenuseCoordinates(tempPos, cell.size.x / 2, (cell.rotation + 180) / toDegrees);
    cellPoint[4] = HypotenuseCoordinates(tempPos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation - 135) / toDegrees);
    cellPoint[5] = HypotenuseCoordinates(tempPos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation - 55) / toDegrees);
}
void SpikeLines(Equipment& spike)
{
    tempPos = HypotenuseCoordinates(spike.pos, Pixels(2), (spike.rotation - 180) / toDegrees);
    tempPos = GetWorldToScreen2D(tempPos, camera);

    spikePoint[0] = HypotenuseCoordinates(tempPos, spike.size.y / 2, (spike.rotation - 90) / toDegrees);
    spikePoint[1] = HypotenuseCoordinates(tempPos, spike.size.x + Pixels(2), spike.rotation / toDegrees);
    spikePoint[2] = HypotenuseCoordinates(tempPos, spike.size.y / 2, (spike.rotation + 90) / toDegrees);

    /*DrawLine(spikePoint[0].x, spikePoint[0].y, spikePoint[1].x, spikePoint[1].y, RED);
    DrawLine(spikePoint[1].x, spikePoint[1].y, spikePoint[2].x, spikePoint[2].y, RED);*/
}
void CannonBallLines(GameObject& ball)
{
    tempPos = GetWorldToScreen2D(ball.pos, camera);
    ballPoint[0] = { tempPos.x + ball.size.x, tempPos.y };
    ballPoint[1] = { tempPos.x, tempPos.y + ball.size.y };
    ballPoint[2] = { tempPos.x - ball.size.x, tempPos.y };
    ballPoint[3] = { tempPos.x, tempPos.y - ball.size.y };

    /*DrawLine(ballPoint[0].x, ballPoint[0].y, ballPoint[1].x, ballPoint[1].y, RED);
    DrawLine(ballPoint[1].x, ballPoint[1].y, ballPoint[2].x, ballPoint[2].y, RED);
    DrawLine(ballPoint[2].x, ballPoint[2].y, ballPoint[3].x, ballPoint[3].y, RED);
    DrawLine(ballPoint[3].x, ballPoint[3].y, ballPoint[0].x, ballPoint[0].y, RED);*/
}

bool LineCellCollision(GameObject& cell, Vector2 point1, Vector2 point2)
{
    CellLines(cell);

    //cycle collision checks between cell's lines and given line
    for (int i = 0; i < 5; i++)
    {
        //DrawLine(cellPoint[i].x, cellPoint[i].y, cellPoint[i + 1].x, cellPoint[i + 1].y, BLUE);
        if (CheckCollisionLines(point1, point2, cellPoint[i], cellPoint[i + 1], &dummyVector2))
            return 1;
    }
    //DrawLine(cellPoint[5].x, cellPoint[5].y, cellPoint[0].x, cellPoint[0].y, BLUE);

    if (CheckCollisionLines(point1, point2, cellPoint[5], cellPoint[0], &dummyVector2))
        return 1;
    return 0;
}
void SpikeCollision(Cell& attacked, Cell& attacker, int spikeSlot)
{
    SpikeLines(attacker.equipment[spikeSlot]);

    //check collision for both of the spike's lines
    if (LineCellCollision(attacked, spikePoint[0], spikePoint[1]) || LineCellCollision(attacked, spikePoint[1], spikePoint[2]))
        if (attacked.knockbackFrames < 1)
        {
            attacked.damage += attacker.equipment[spikeSlot].boost;
            attacked.SetKnockback(16, AddRotation(attacker.rotation, directionRotation[spikeSlot * 2]) / toDegrees);
            attacker.SetKnockback(16, AddRotation(attacker.rotation, 180, directionRotation[spikeSlot * 2]) / toDegrees);
        }
}
bool CannonBallCollision(Cell& attacked, CannonBall& ball)
{
    CannonBallLines(ball);

    //check collision for all of the ball's lines and load damage
    for (int i = 0; i < 3; i++)
        if (LineCellCollision(attacked, ballPoint[i], ballPoint[i + 1]))
        {
            attacked.damage += ball.attack;
            return 1;
        }

    if (LineCellCollision(attacked, ballPoint[3], ballPoint[0]))
    {
        attacked.damage += ball.attack;
        return 1;
    }
    return 0;
}