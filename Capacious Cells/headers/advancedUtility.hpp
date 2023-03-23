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