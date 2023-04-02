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