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
                
                DeathCkeck();
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
                DrawTexture(mainMenu, GetScreenToWorld2D({ 0,0 }, camera).x, GetScreenToWorld2D({ 0,0 }, camera).y, WHITE);

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