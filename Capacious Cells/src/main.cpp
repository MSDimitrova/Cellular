#include "../headers/libraries.hpp"

int main()
{
    InitWindow(1280, 720, "Capacious Cells");
    SetupWindow();

    LoadTextures();
    SetupObjects();

    //prepare game
    screen = 2;
    SetupVariables();

    //start game runtime
    while (!WindowShouldClose())
    {
        pause = screen;
        DrawFrame();

        switch (screen)
        {
            case 0: //game screen
            {
                EndMode2D();
                EndDrawing();

                Sorting();
                KillObjects();

                CannonBallActions();
                EnemyActions();
                PlayerActions();

                VariableResets();
                ScreenListens();
                

                //debug
                if (IsKeyPressed(KEY_ONE))
                    player.UpdateSprite(&playerSprite[0]);
                else if (IsKeyPressed(KEY_TWO))
                    player.UpdateSprite(&playerSprite[2]);
                else if (IsKeyPressed(KEY_THREE))
                    std::cout << player.hpText << std::endl;

                tempPos = GetWorldToScreen2D(player.pos, camera);
                tempV2 = HypotenuseCoordinates(tempPos, Pixels(25), player.rotation / toDegrees);
                DrawLine(tempPos.x, tempPos.y, tempV2.x, tempV2.y, YELLOW);

                if (player.hp < 1)
                {
                    player.hp = initialPlayerHp;
                    screen = 4;
                }
            }
            break;

            case 1: //evolve screen
            {
                EndMode2D();

                DrawUI();

                EndDrawing();

                Evolve();

                ScreenListens();
            }
            break;

            case 2: //main menu
            {
                EndMode2D();
                EndDrawing();



                ScreenListens();
            }
            break;

            case 3: //pause screen
            {
                DrawTexture(darkenedScreen, GetScreenToWorld2D({ 0,0 }, camera).x, GetScreenToWorld2D({ 0,0 }, camera).y, WHITE);

                EndMode2D();
                EndDrawing();



                ScreenListens();
            }
            break;

            case 4: //death screen
            {
                DrawTexture(deathScreen, GetScreenToWorld2D({ 0,0 }, camera).x, GetScreenToWorld2D({ 0,0 }, camera).y, WHITE);

                EndMode2D();
                EndDrawing();



                ScreenListens();
            }
            break;
        }
    }
    CloseWindow();
}