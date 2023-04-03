#pragma once
#include "libraries.hpp"

void DrawControls()
{
    tempV2 = GetScreenToWorld2D({Pixels(2), Pixels(2)}, camera);
    switch (screen)
    {
    case 0:
        DrawText("Controls:\nWASD: Move\nSHIFT: Activate tail\nQ: Open membrane\nESC: Pause\nE: Evolve\nEND: Return to desktop", tempV2.x, tempV2.y, Pixels(5), WHITE);
        break;

    case 1:
        DrawText("Controls:\nESC: Pause\nE: Exit evolution\nEND: Return to desktop", tempV2.x, tempV2.y, Pixels(5), WHITE);
        break;

    case 2:
        DrawText("Controls:\nENTER: Start new game\nEND: Return to desktop", tempV2.x, tempV2.y, Pixels(5), WHITE);
        break;
        
    case 3:
        DrawText("Controls:\nESC: Continue\nEND: Return to desktop", tempV2.x, tempV2.y, Pixels(5), WHITE);
        break;

    case 4:
        DrawText("Controls:\nENTER: Start new game\nESC: Return to main menu\nEND: Return to desktop", tempV2.x, tempV2.y, Pixels(5), WHITE);
        break;
    }
}

void DrawGameObject(GameObject& gameObject, bool exeption = 0)
{
    if (!pause && gameObject.sprite.size() > 1 && !exeption)
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
            if (cell.equipment[i].name == "toxin")
                DrawGameObject(cell.equipment[i], 1);
            else
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
    
    DrawTexture(background, CENTER.x - background.width / 2, CENTER.y - background.height / 2, WHITE);
    DrawCircle(CENTER.x, CENTER.y, 5, RED);

    for (int i = 0; i < enemyOnScreen.size(); i++)
        DrawCell(*enemyOnScreen[i]);
    for (int i = 0; i < cannonBalls.size(); i++)
        DrawGameObject(cannonBalls[i]);
    for (int i = 0; i < foodOnScreen.size(); i++)
        DrawGameObject(*foodOnScreen[i]);
    DrawCell(player);
    DrawControls();
}