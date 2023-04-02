#pragma once
#include "libraries.hpp"

void DrawGameObject(GameObject& gameObject, bool cell = 0)
{
    if (!pause && gameObject.sprite.size() > 1 && !cell)
        gameObject.Animate();

    DrawTexturePro(*gameObject.currentSprite, Rectangle{ 0, 0, gameObject.size.x, gameObject.size.y },
        { gameObject.pos.x, gameObject.pos.y, gameObject.size.x, gameObject.size.y },
        { gameObject.size.x / 2, gameObject.size.y / 2 }, gameObject.rotation, RAYWHITE);
}
void DrawCell(Cell& cell)
{
    //draw equipment
    for (int i = 0; i < slots; i++)
        if (cell.equipment[i].name != "none")
        {
            cell.equipment[i].UpdatePos(cell);
            DrawGameObject(cell.equipment[i]);
        }

    //draw cell
    DrawGameObject(cell, 1);

    //draw hp
    if(cell.id == -1 && screen == 4)
        cell.DrawHp(0);
    else
        cell.DrawHp();
}

void DrawFrame()
{
    //Update screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    BeginMode2D(camera);

    //DrawTexture(background, CENTER.x, CENTER.y, WHITE);
    DrawCircle(CENTER.x, CENTER.y, 5, RED);

    for (int i = 0; i < enemyOnScreen.size(); i++)
        DrawCell(*enemyOnScreen[i]);
    for (int i = 0; i < cannonBalls.size(); i++)
        DrawGameObject(cannonBalls[i]);
    for (int i = 0; i < foodOnScreen.size(); i++)
        DrawGameObject(*foodOnScreen[i]);
    DrawCell(player);
}