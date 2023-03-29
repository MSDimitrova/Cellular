#pragma once
#include "libraries.hpp"

//cell related
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

//collisions
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
bool SpikeCollision(Cell& attacked, Cell& attacker, int spikeSlot)
{
    SpikeLines(attacker.equipment[spikeSlot]);

    //check collision for both of the spike's lines
    if (LineCellCollision(attacked, spikePoint[0], spikePoint[1]) || LineCellCollision(attacked, spikePoint[1], spikePoint[2]))
        if (attacked.knockbackFrames < 1)
            attacked.SetKnockback(24, AddRotation(attacker.rotation, directionRotation[spikeSlot * 2]) / toDegrees);
        return 1;
    return 0;
}

//screen related
bool IsOnScreen(Vector2 pos)
{
    if (pos.x > screenCheck[2].x && pos.y > screenCheck[2].y && pos.x < screenCheck[0].x && pos.y < screenCheck[0].y)
        return 1;
    return 0;
}
void DrawGameObject(GameObject& gameObject)
{
    if (!pause && gameObject.sprite.size() > 1)
        gameObject.Animate();

    DrawTexturePro(*gameObject.currentSprite, Rectangle{ 0, 0, gameObject.size.x, gameObject.size.y },
        { gameObject.pos.x, gameObject.pos.y, gameObject.size.x, gameObject.size.y },
        { gameObject.size.x / 2, gameObject.size.y / 2 }, gameObject.rotation, RAYWHITE);
}
void DrawCell(Cell& cell)
{
    for (int i = 0; i < 4; i++)
        if (cell.equipment[i].name != "none")
        {
            cell.equipment[i].UpdatePos(cell, cell.rotationIndex);
            DrawGameObject(cell.equipment[i]);
        }
    DrawGameObject(cell);
}
void UpdateScreen(Color bg)
{
    BeginDrawing();
    ClearBackground(bg);
    DrawFPS(10, 10);

    BeginMode2D(camera);
    //DrawTexture(background, CENTER.x - background.width / 2, CENTER.y - background.height / 2, RAYWHITE);
    DrawCircle(CENTER.x, CENTER.y, 5, RED);

    for (int i = 0; i < enemyOnScreen.size(); i++)
        DrawCell(*enemyOnScreen[i]);
    DrawCell(player);
    
    EndMode2D();
    EndDrawing();

    if (!pause)
    {
        //player rotation
        player.rotation = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - player.pos.y,
            GetScreenToWorld2D(GetMousePosition(), camera).x - player.pos.x) * toDegrees;
        if (player.rotation < 0)
            player.rotation += 360;
    }
}