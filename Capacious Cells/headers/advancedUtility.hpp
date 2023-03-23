#pragma once
#include "libraries.hpp"

//cell related
void Equip(Cell& cell, int _part, int _slot)
{
    cell.equipment[_slot].Setup(*prefabPart[_part]);
    cell.equipment[_slot].slot = _slot;
}
void MoveInTwoDirections(Cell& cell, int directionX, int directionY, bool& refCheck = dummyBool)
{
    cell.pos.x += round(sqrt(pow(cell.speed, 2) / 2)) * directionX;
    cell.pos.y += round(sqrt(pow(cell.speed, 2) / 2)) * directionY;
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
bool CellLineCollision(GameObject& cell, Vector2 point1, Vector2 point2)
{
    Vector2 pos = GetWorldToScreen2D(cell.pos, camera);

    //setup cell's lines' points
    point[0] = HypotenuseCoordinates(pos, cell.size.x / 2, cell.rotation / 57.29578f);
    point[1] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation + 55) / 57.29578f);
    point[2] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation + 135) / 57.29578f);
    point[3] = HypotenuseCoordinates(pos, cell.size.x / 2, (cell.rotation + 180) / 57.29578f);
    point[4] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(9)), (cell.rotation - 135) / 57.29578f);
    point[5] = HypotenuseCoordinates(pos, HypotenuseLength((cell.size.y / 2) - Pixels(2), Pixels(5)), (cell.rotation - 55) / 57.29578f);

    //cycle collision checks between cell's lines and given line
    for (int i = 0; i < 5; i++)
    {
        DrawLine(point[i].x, point[i].y, point[i + 1].x, point[i + 1].y, BLUE);
        if (CheckCollisionLines(point1, point2, point[i], point[i + 1], &dummyVector2))
            return 1;
    }
    DrawLine(point[5].x, point[5].y, point[0].x, point[0].y, BLUE);

    if (CheckCollisionLines(point1, point2, point[5], point[0], &dummyVector2))
        return 1;
    return 0;
}
bool SpikeCollision(GameObject& cell, Equipment& spike)
{
    Vector2 pos = GetWorldToScreen2D(spike.pos, camera);

    //setup spike's lines' points
    Vector2 point1 = HypotenuseCoordinates(pos, spike.size.y / 2, (spike.rotation - 90) / 57.29578f);
    Vector2 point2 = HypotenuseCoordinates(pos, spike.size.x, spike.rotation / 57.29578f);
    Vector2 point3 = HypotenuseCoordinates(pos, spike.size.y / 2, (spike.rotation + 90) / 57.29578f);
    DrawLine(point1.x, point1.y, point2.x, point2.y, RED);
    DrawLine(point2.x, point2.y, point3.x, point3.y, RED);

    //check collision for both of the spike's lines
    if (CellLineCollision(cell, point1, point2))
        return 1;
    else if (CellLineCollision(cell, point2, point3))
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
            GetScreenToWorld2D(GetMousePosition(), camera).x - player.pos.x) * 57.29578f;
    }
}