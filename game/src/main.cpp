#include "raylib.h"
#include "raymath.h"

int main()
{
    InitWindow(800, 800, "Physics-1");

    Vector2 circle_position_1 = { 400.0f, 400.0f };
    Vector2 circle_position_2 = { 400.0f, 400.0f };
    float circle_radius = 25.0f;
    Color circle_color = PURPLE;

    while (!WindowShouldClose())
    {
        float t = GetTime();
        float dt = GetFrameTime();
        circle_position_1.x = 400.0f + 150.0f * sinf(t);    // moves left/right within a range of 150 units forever
        circle_position_2.y += 10.0f * dt;                  // moves down at 10 units per second

        BeginDrawing();
            ClearBackground(WHITE);
            DrawText("Hello world!", 10, 10, 20, LIGHTGRAY);
            DrawCircleV(circle_position_1, circle_radius, circle_color);
            DrawCircleV(circle_position_2, circle_radius, circle_color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
