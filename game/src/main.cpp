//This project uses the Raylib framework to handle graphics, input, GUI, etc.
//Documentation here! https://www.raylib.com/index.html, examples at https://www.raylib.com/examples.html

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"

const unsigned int TARGET_FPS = 50; //1/50 = 0.02
float time = 0;
float dt;
float x = 500;
float y = 500;
float frequency = 1.0f;
float amplitude = 70;

//Change world state
void update()
{
    dt = 1.0 / TARGET_FPS;
    time += dt;


    x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
    y = y + (cos(time * frequency)) * frequency * amplitude * dt;
}

//Display world state
void draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Joss Moo-Young 123456789", 10, 20, 20, LIGHTGRAY);

    GuiSliderBar(Rectangle{ 60, (float)GetScreenHeight() - 10, 1000, 10}, "Time", TextFormat("%.2f", time), &time, 0, 240);
    DrawText(TextFormat("FPS: %i, TIME: %.2f", TARGET_FPS, time), GetScreenWidth() - 250, 20, 20, LIGHTGRAY);

    DrawCircle(x, y, 60, RED);
    DrawCircle(GetScreenWidth()/2 + cos(time * frequency) * amplitude, GetScreenHeight()/2 + sin(time * frequency) * amplitude, 60, GREEN);

    EndDrawing();
}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Physics Labs: Joss Moo-Young 123456789");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        update();
        draw();
    }

    CloseWindow();
    return 0;
}
