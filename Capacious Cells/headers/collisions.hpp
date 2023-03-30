#pragma once
#include "libraries.hpp"

void CellLines(GameObject& cell)
{
    Vector2 pos = GetWorldToScreen2D(cell.pos, camera);
    cellPoint[0] = HypotenuseCoordinates(pos, cell.size.x / 2, cell.rotation / toDegrees);
    cellPoint[1] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation + 55) / toDegrees);
    cellPoint[2] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation + 135) / toDegrees);
    cellPoint[3] = HypotenuseCoordinates(pos, cell.size.x / 2, (cell.rotation + 180) / toDegrees);
    cellPoint[4] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation - 135) / toDegrees);
    cellPoint[5] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation - 55) / toDegrees);
}
void SpikeLines(Equipment& spike)
{
    Vector2 pos = GetWorldToScreen2D(spike.pos, camera);
    spikePoint[0] = HypotenuseCoordinates(pos, spike.size.y / 2, (spike.rotation - 90) / toDegrees);
    spikePoint[1] = HypotenuseCoordinates(pos, spike.size.x, spike.rotation / toDegrees);
    spikePoint[2] = HypotenuseCoordinates(pos, spike.size.y / 2, (spike.rotation + 90) / toDegrees);

    DrawLine(spikePoint[0].x, spikePoint[0].y, spikePoint[1].x, spikePoint[1].y, RED);
    DrawLine(spikePoint[1].x, spikePoint[1].y, spikePoint[2].x, spikePoint[2].y, RED);
}
void CanonBallLines(GameObject& ball)
{
    Vector2 pos = GetWorldToScreen2D(ball.pos, camera);
    ballPoint[0] = { pos.x + ball.size.x, pos.y };
    ballPoint[1] = { pos.x, pos.y + ball.size.y };
    ballPoint[2] = { pos.x - ball.size.x, pos.y };
    ballPoint[3] = { pos.x, pos.y - ball.size.y };

    DrawLine(ballPoint[0].x, ballPoint[0].y, ballPoint[1].x, ballPoint[1].y, RED);
    DrawLine(ballPoint[1].x, ballPoint[1].y, ballPoint[2].x, ballPoint[2].y, RED);
    DrawLine(ballPoint[2].x, ballPoint[2].y, ballPoint[3].x, ballPoint[3].y, RED);
    DrawLine(ballPoint[3].x, ballPoint[3].y, ballPoint[0].x, ballPoint[0].y, RED);
}

bool LineCellCollision(GameObject& cell, Vector2 point1, Vector2 point2)
{
    CellLines(cell);

    //cycle collision checks between cell's lines and given line
    for (int i = 0; i < 5; i++)
    {
        DrawLine(cellPoint[i].x, cellPoint[i].y, cellPoint[i + 1].x, cellPoint[i + 1].y, BLUE);
        if (CheckCollisionLines(point1, point2, cellPoint[i], cellPoint[i + 1], &dummyVector2))
            return 1;
    }
    DrawLine(cellPoint[5].x, cellPoint[5].y, cellPoint[0].x, cellPoint[0].y, BLUE);

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
        }
}
bool CanonBallCollision(Cell& attacked, CanonBall& ball)
{
    CanonBallLines(ball);

    //check collision for all of the ball's lines
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