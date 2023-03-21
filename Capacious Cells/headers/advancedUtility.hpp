#pragma once
#include "libraries.hpp"

//cell related
void Equip(Cell& cell, int _prefab, int _slot)
{
    cell.equipment[_slot].Setup(*prefabs[_prefab]);
    cell.equipment[_slot].slot = _slot;
}

//screen related
void DrawGameObject(GameObject& gameObject)
{
    if (!pause && gameObject.sprite.size() > 1)
        gameObject.Animate();

    DrawTexturePro(*gameObject.currentSprite, Rectangle{ 0, 0, gameObject.size.x, gameObject.size.y },
        { gameObject.pos.x, gameObject.pos.y, gameObject.size.x, gameObject.size.y },
        { gameObject.size.x / 2, gameObject.size.y / 2 }, gameObject.rotation, RAYWHITE);
}
void UpdateScreen(Color bg)
{
    BeginDrawing();
    ClearBackground(bg);
    DrawFPS(10, 10);

    BeginMode2D(camera);
    //DrawTexture(background, CENTER.x - background.width / 2, CENTER.y - background.height / 2, RAYWHITE);
    DrawCircle(CENTER.x, CENTER.y, 5, RED);

    for (int i = 0; i < 4; i++)
        if (player.equipment[i].name != "none")
        {
            player.equipment[i].UpdatePos(player, player.rotationIndex);
            DrawGameObject(player.equipment[i]);
        }
    DrawGameObject(player);

    EndMode2D();
    EndDrawing();

    if (!pause)
    {
        //player rotation
        player.rotation = atan2(GetScreenToWorld2D(GetMousePosition(), camera).y - player.pos.y,
            GetScreenToWorld2D(GetMousePosition(), camera).x - player.pos.x) * 57.29578f;
    }
}