/*
This project uses the Raylib framework to provide us functionality for math, graphics, GUI, input etc.
See documentation here: https://www.raylib.com/, and examples here: https://www.raylib.com/examples.html
*/

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <string>
#include <vector>

const unsigned int TARGET_FPS = 50;
float dt = 60;
float time = 0;
float x = 500;
float y = 500;
float frequency = 1;
float amplitude = 100;

class PhysicsObject
{
public:
    Vector2 position = {0 , 0 };
    Vector2 velocity = { 0, 0 };
    float mass = 1;

    float radius = 15;
    std::string name = "object";

    void draw()
    {
        DrawCircle(position.x, position.y, radius, RED);
        DrawText(name.c_str(), GetScreenHeight() - 140, 10, 30, LIGHTGRAY);
        DrawLineEx(position, (position + velocity), 1, RED);
    }
};

class PhysicsWorld
{
private:
    unsigned int objectCount = 0;
public:
    std::vector<PhysicsObject> objects;
    Vector2 accelerationGravity = { 0,9 };

   void add(PhysicsObject newObject)
   {
       newObject.name = std::to_string(objectCount);
       objects.push_back(newObject);
       objectCount++;
   }

   void update()
   {
       for (int i = 0; i < objects.size(); i++)
       {
           objects[i].position = objects[i].position + objects[i].velocity * dt;
           objects[i].velocity = objects[i].velocity + accelerationGravity * dt;
       }
   }
};

float speed = 100;
float angle = 0;
float launchPosition = 100;

PhysicsWorld world;

//Vector2 position = {500, 500};
//Vector2 velocity = { 0, 0 };


void update()
{
    dt = 1.0f / TARGET_FPS;
    time += dt;

    world.update();
    //position += velocity * dt;
    //velocity += accelerationGravity * dt;
    if (IsKeyPressed(KEY_SPACE))
    {
        PhysicsObject newBird;
        newBird.position = { 100, (float)GetScreenHeight() - launchPosition };
        newBird.velocity = { speed * (float)cos(angle * DEG2RAD), speed * (float)sin(angle * DEG2RAD) };
        newBird.radius = 15;
        world.add(newBird);
    }
   //y = y + (cos(time * frequency)) * frequency * amplitude * dt;
   // x = x + (-sin(time * frequency)) * frequency * amplitude * dt;
}

void draw()
{
        
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Johnny Zimmer: 101533005 - GAME2005", 10, 10, 20, DARKGRAY);
            GuiSliderBar(Rectangle{ 10, 40, 500, 30 }, "", TextFormat("Speed:%.0f", speed), &speed, -1000, 1000);
            GuiSliderBar(Rectangle{ 10, 80, 500, 30 }, "", TextFormat("Angle:%.0f Degrees", angle), &angle, -180, 180);
            GuiSliderBar(Rectangle{ 10, 120, 500, 30 }, "", TextFormat("Launch Position Height:%.0f ", launchPosition), &launchPosition, 100, 600);
            
            GuiSliderBar(Rectangle{ 10, 160, 500, 30 }, "", TextFormat("Gravity: %.0f Px/sec^2 ", world.accelerationGravity.y), &world.accelerationGravity.y, -1000, 1000);

            
            Vector2 startPos = { 100, GetScreenHeight() - launchPosition};
            Vector2 velocity = { speed * cos(angle* DEG2RAD), speed * sin(angle* DEG2RAD)};

            DrawLineEx(startPos, (startPos + velocity), 3, RED);

            for (int i = 0;i < world.objects.size();i++)
            {
                world.objects[i].draw();
            }
            //DrawCircle(position.x, position.y, 15, RED);
            
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
