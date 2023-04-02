#pragma once
#include "libraries.hpp"

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
            cell.equipment[i].UpdatePos(cell);
            DrawGameObject(cell.equipment[i]);
        }
    DrawGameObject(cell);
}