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

enum PhysicsShape
{
    CIRCLE,
    HALFSPACE
};

class PhysicsObject
{
public:
    bool isStatic = false;
    Vector2 position = {0 , 0 };
    Vector2 velocity = { 0, 0 };
    float mass = 1;
    
    std::string name = "object";
    Color color = GREEN;
    virtual void draw()
    {
        DrawCircle(position.x, position.y, 2, color);
        //DrawText(name.c_str(), position.x, position.y, 12, LIGHTGRAY);
        // DrawLineEx(position, (position + velocity), 1, RED);
    }
    virtual PhysicsShape Shape() = 0;
};

class PhysicsHalfspace : public PhysicsObject
{
private:
    float rotation = 0;
    Vector2 normal = { 0,-1 };
public:
    void setRotationDegrees(float rotationInDegrees)
    {
        rotation = rotationInDegrees;
        normal = Vector2Rotate({ 0,-1 },rotation * DEG2RAD);
    }
    float getRotation()
    {
        return rotation;
    }
    Vector2 getNormal()
    {
        return normal;
    }
    void draw() override
    {
        DrawCircle(position.x, position.y, 8, color);
        DrawLineEx(position, position + normal * 30, 1, color);
        Vector2 parallelToSurface = Vector2Rotate(normal, PI * 0.5f);
        DrawLineEx(position - parallelToSurface * 4000,position + parallelToSurface * 4000, 1, color);

    }

    PhysicsShape Shape() override
    {
        return HALFSPACE;
    }
};

class PhysicsCircle : public PhysicsObject
{
public:
    float radius;

    void draw() override
    {
        DrawCircle(position.x, position.y, radius, color);
        DrawText(name.c_str(), position.x, position.y, radius * 2, LIGHTGRAY);
        DrawLineEx(position, (position + velocity), 1, color);
    }

    PhysicsShape Shape() override
    {
        return CIRCLE;
    }
};

bool CircleCircleOverlap(PhysicsCircle* circleA, PhysicsCircle* circleB)
{
    Vector2 displacementFromAToB = circleB->position - circleA->position;
    float distance = Vector2Length(displacementFromAToB);
    float sumOfRadius = circleA->radius + circleB->radius;
    if (sumOfRadius > distance)
    {
        return true;
    }
    else
        return false;
}

bool CircleHalfspaceOverlap(PhysicsCircle* circle, PhysicsHalfspace* halfspace)
{
    Vector2 displacementToCircle = circle->position - halfspace->position;
    float dot = Vector2DotProduct(displacementToCircle, halfspace->getNormal());
    Vector2 vectorProjection = halfspace->getNormal() * dot;

    bool isOverlapping = dot <= circle->radius && dot >= -circle->radius;
    DrawLineEx(circle->position, circle->position - vectorProjection, 1, GRAY);
    Vector2 midpoint = circle->position - vectorProjection * 0.5f;
    DrawText(TextFormat("D: %6.0f", dot), midpoint.x, midpoint.y, 30, GRAY);

    return isOverlapping;
}

class PhysicsWorld
{
private:
    unsigned int objectCount = 0;
public:
    std::vector<PhysicsObject*> objects;
    Vector2 accelerationGravity = { 0,9 };

    void add(PhysicsObject* newObject)
    {
        newObject->name = std::to_string(objectCount);
        objects.push_back(newObject);
        objectCount++;
    }

    void update()
    {
        for (int i = 0; i < objects.size(); i++)
        {
            PhysicsObject* object = objects[i];

            if (object->isStatic) continue;

            object->position = object->position + object->velocity * dt;
            object->velocity = object->velocity + accelerationGravity * dt;
        }

        checkCollisions();
    }

    void checkCollisions()
    {
        for (int i = 0; i < objects.size(); i++)
        {
            objects[i]->color = GREEN;
        }

        for (int i = 0; i < objects.size();i++)
        {
            for (int j = i + 1 ; j < objects.size();j++)
            {
                if (i == j) continue;
                PhysicsObject* objectPointerA = objects[i];
                PhysicsObject* objectPointerB = objects[j];
                //PhysicsCircle* circlePointerA = (PhysicsCircle*)objectPointerA;
                //PhysicsCircle* circlePointerB = (PhysicsCircle*)objectPointerB;
                PhysicsShape shapeOfA = objectPointerA->Shape();
                PhysicsShape shapeOfB = objectPointerB->Shape();

                bool didOverlap = false;

                if (shapeOfA == CIRCLE && shapeOfB == CIRCLE)
                {
                    didOverlap = (CircleCircleOverlap((PhysicsCircle*)objectPointerA, (PhysicsCircle*)objectPointerB));
                    
                }
                else if (shapeOfA == CIRCLE && shapeOfB == HALFSPACE)
                {
                    didOverlap = CircleHalfspaceOverlap((PhysicsCircle*)objectPointerA, (PhysicsHalfspace*)objectPointerB);
                }
                else if (shapeOfA == HALFSPACE && shapeOfB == CIRCLE)
                {
                    didOverlap = CircleHalfspaceOverlap((PhysicsCircle*)objectPointerB, (PhysicsHalfspace*)objectPointerA);

                }

                if(didOverlap)
                {
                    objectPointerA->color = RED;
                    objectPointerB->color = RED;
                }
                
  
            }

        }
    }
};



float speed = 100;
float angle = 0;
float launchPosition = 100;

PhysicsWorld world;
PhysicsHalfspace halfspace;
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
        PhysicsCircle* newBird = new PhysicsCircle();
        newBird->position = { 100, (float)GetScreenHeight() - launchPosition };
        newBird->velocity = { speed * (float)cos(angle * DEG2RAD), speed * (float)sin(angle * DEG2RAD) };
        newBird->radius = 15;
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

            GuiSliderBar(Rectangle{ 10, 200, 300, 30 }, "", TextFormat("Halfspace X: %0.f",halfspace.position.x), &halfspace.position.x, 0, GetScreenHeight());
            GuiSliderBar(Rectangle{ 410, 200, 300, 30 }, "", TextFormat("HalfSpace Y: %0.f",halfspace.position.y), &halfspace.position.y, 0, GetScreenHeight());
            float halfspaceRotation = halfspace.getRotation();
            GuiSliderBar(Rectangle{ 810, 200, 300, 30 }, "", TextFormat("Rotation: %0.f", halfspace.getRotation()), &halfspaceRotation, -360, 360);

            halfspace.setRotationDegrees(halfspaceRotation);
            
            Vector2 startPos = { 100, GetScreenHeight() - launchPosition};
            Vector2 velocity = { speed * cos(angle* DEG2RAD), speed * sin(angle* DEG2RAD)};

            DrawLineEx(startPos, (startPos + velocity), 3, RED);

            for (int i = 0;i < world.objects.size();i++)
            {
                world.objects[i]->draw();
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
    halfspace.isStatic = true;
    halfspace.position = { 500 ,700 };
    world.add(&halfspace);
    while (!WindowShouldClose())
    {
        update();
        draw();
    }

    CloseWindow();
    return 0;
}
