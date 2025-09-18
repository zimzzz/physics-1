/*
This project uses the Raylib framework to provide us functionality for math, graphics, GUI, input etc.
See documentation here: https://www.raylib.com/, and examples here: https://www.raylib.com/examples.html
*/

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"

const unsigned int TARGET_FPS = 50;
float dt = 60;
float time = 0;
float x = 500;
float y = 500;
float frequency = 1;
float amplitude = 100;

float speed = 100;
float angle = 0;

void update()
{
    dt = 1.0f / TARGET_FPS;
    time += dt;

   //y = y + (cos(time * frequency)) * frequency * amplitude * dt;
   // x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
}

void draw()
{
        
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Johnny Zimmer: 101533005 - GAME2005", 10, 10, 20, DARKGRAY);
            GuiSliderBar(Rectangle{ 10, 40, 500, 30 }, "Time", TextFormat("Speed:%.0f", speed), &speed, -1000, 1000);
            GuiSliderBar(Rectangle{ 10, 80, 500, 30 }, "Time", TextFormat("Angle:%.0f Degrees", angle), &angle, -180, 180);
            Vector2 startPos = { 100, GetScreenHeight() - 100};
            Vector2 velocity = { speed * cos(angle* DEG2RAD), speed * sin(angle* DEG2RAD)};

            DrawLineEx(startPos, (startPos + velocity), 3, RED);
            
            //GuiSliderBar(Rectangle{ 60, 50, 1000, 10 }, "Time", TextFormat("%.2f", time), &time, 0, 240);
            //DrawText(TextFormat("T: %.2f", time), GetScreenWidth() - 180, 10, 30, LIGHTGRAY);
            //DrawCircle(x, y, 70, RED);

        EndDrawing();

}


int main()
{
    InitWindow(InitialWidth, InitialHeight, "Johnny Zimmer: 101533005 - GAME2005");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        update();
        draw();
    }

    CloseWindow();
    return 0;
}
