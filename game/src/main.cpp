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

float coefficientOfFriction = 0.5f;
float worldmass = 1.0f;
float restitution = 0.9f;

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
    Vector2 netForce = { 0,0 };
    float bounciness = 0.8f;

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



bool CircleCircleCollision(PhysicsCircle* circleA, PhysicsCircle* circleB);
bool CircleHalfspaceCollision(PhysicsCircle* circle, PhysicsHalfspace* halfspace);

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

    void ResetNetForce()
    {
        for (int i = 0; i < objects.size(); i++)
        {
            objects[i]->netForce = { 0,0 };
        }
    }

    void AddGravityForce()
    {
        for (int i = 0; i < objects.size(); i++)
        {
            PhysicsObject* object = objects[i];

            if (object->isStatic) continue;

            Vector2 FGravity = accelerationGravity * object->mass;
            object->netForce += FGravity;

            DrawLineEx(object->position, object->position + FGravity, 1, PURPLE);
        }
    }

    void ApplyKinematics()
    {
        for (int i = 0; i < objects.size(); i++)
        {
            PhysicsObject* object = objects[i];

            if (object->isStatic) continue;

            object->position = object->position + object->velocity * dt;
            Vector2 acceleration = object->netForce / object->mass;
            object->velocity = object->velocity + acceleration * dt;

            DrawLineEx(object->position, object->position + object->netForce, 4, GRAY);
        }
    }

    void update()
    {
        ResetNetForce();
        AddGravityForce();
        checkCollisions();
        ApplyKinematics();
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
                    didOverlap = CircleCircleCollision((PhysicsCircle*)objectPointerA, (PhysicsCircle*)objectPointerB);
                    
                }
                else if (shapeOfA == CIRCLE && shapeOfB == HALFSPACE)
                {
                    didOverlap = CircleHalfspaceCollision((PhysicsCircle*)objectPointerA, (PhysicsHalfspace*)objectPointerB);
                }
                else if (shapeOfA == HALFSPACE && shapeOfB == CIRCLE)
                {
                    didOverlap = CircleHalfspaceCollision((PhysicsCircle*)objectPointerB, (PhysicsHalfspace*)objectPointerA);

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
PhysicsHalfspace halfspace2;
PhysicsHalfspace halfspace3;

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

bool CircleCircleCollision(PhysicsCircle* circleA, PhysicsCircle* circleB)
{
    Vector2 displacementFromAToB = circleB->position - circleA->position;
    float distance = Vector2Length(displacementFromAToB);
    float sumOfRadius = circleA->radius + circleB->radius;
    float overlap = sumOfRadius - distance;
    if (overlap >= 0)
    {
        Vector2 normal = displacementFromAToB / distance;
        Vector2 mtv = normal * overlap;
        circleA->position -= mtv * 0.5f;
        circleB->position += mtv * 0.5f;

        Vector2 velocityBRelativeToA = circleB->velocity - circleA->velocity;
        float closingVelocity = Vector2DotProduct(velocityBRelativeToA, normal);
        if (closingVelocity >= 0)  return true;
        
        float restitution = circleA->bounciness * circleB->bounciness;
        float totalMass = circleA->mass + circleB->mass;
        float impulseMagnitude = ((1.0f + restitution)*closingVelocity*circleA->mass*circleB->mass) / totalMass;
        Vector2 impulseB = normal * -impulseMagnitude;
        Vector2 impulseA = normal * impulseMagnitude;

        circleA->velocity += impulseA / circleA->mass;
        circleB->velocity += impulseB / circleB->mass;
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

bool CircleHalfspaceCollision(PhysicsCircle* circle, PhysicsHalfspace* halfspace)
{
    Vector2 displacementToCircle = circle->position - halfspace->position;
    float dot = Vector2DotProduct(displacementToCircle, halfspace->getNormal());
    Vector2 vectorProjection = halfspace->getNormal() * dot;
    float overlap = circle->radius - dot;
    if (overlap > 0)
    {
        Vector2 mtv = halfspace->getNormal() * overlap;
        circle->position += mtv;

        Vector2 Fgravity = world.accelerationGravity * circle->mass;
        Vector2 FgPerp = halfspace->getNormal() * Vector2DotProduct(Fgravity, halfspace->getNormal());
        Vector2 Fnormal = FgPerp * -1;
        circle->netForce += Fnormal;
        DrawLineEx(circle->position, circle->position + Fnormal, 2, GREEN);
        //Friction
        Vector2 normalVelocity = halfspace->getNormal() * Vector2DotProduct(circle->velocity, halfspace->getNormal());
        Vector2 frictionVelocity = circle->velocity - normalVelocity;
        float frictionSpeed = Vector2Length(frictionVelocity);
        if (frictionSpeed > 0.0001f)
        {
            Vector2 frictionDirection = Vector2Normalize(frictionVelocity) * -1;
            float frictionMagnitude = coefficientOfFriction * Vector2Length(Fnormal);
            Vector2 Ffriction = frictionDirection * frictionMagnitude;
            circle->netForce += Ffriction;
            DrawLineEx(circle->position, circle->position + Ffriction, 2, ORANGE);
        }
        //Bouncing
        //Vector2 velocityBRelativeToA = circleB->velocity - circleA->velocity;
        float closingVelocity = Vector2DotProduct(circle->velocity, halfspace->getNormal());
        if (closingVelocity >= 0)  return true;

        float restitution = circle->bounciness * halfspace->bounciness;
        circle->velocity += halfspace->getNormal() * closingVelocity * -(1.0f + restitution);


        return true;
    }
    else
    {
        return false;
    }
    /*bool isOverlapping = dot <= circle->radius && dot >= -circle->radius;*/
    /*DrawLineEx(circle->position, circle->position - vectorProjection, 1, GRAY);
    Vector2 midpoint = circle->position - vectorProjection * 0.5f;
    DrawText(TextFormat("D: %6.0f", dot), midpoint.x, midpoint.y, 30, GRAY);*/
}

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
        newBird->mass = worldmass;
        newBird->bounciness = restitution;
        world.add(newBird);
    }
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
            GuiSliderBar(Rectangle{ 10, 240, 300, 30 }, "", TextFormat("Friction: %0.2f", coefficientOfFriction), &coefficientOfFriction, 0, 1);
            GuiSliderBar(Rectangle{ 410, 240, 300, 30 }, "", TextFormat("World Mass: %0.2f", worldmass), &worldmass, 0, 10);
            GuiSliderBar(Rectangle{ 810, 240, 300, 30 }, "", TextFormat("Resitution: %0.2f", restitution), &restitution, 0, 1);



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

           /* Vector2 location = { 500,500 };
            DrawCircleLines(location.x, location.y, 100, WHITE);
            float mass = 8;
            Vector2 Fgravity = world.accelerationGravity * mass;
            DrawLine(location.x, location.y, location.x + Fgravity.x, location.y + Fgravity.y, PURPLE);
            Vector2 FgPerp = halfspace.getNormal() * Vector2DotProduct(Fgravity, halfspace.getNormal());
            Vector2 Fnormal = FgPerp * -1;
            DrawLine(location.x, location.y, location.x + Fnormal.x, location.y + Fnormal.y, GREEN);
            Vector2 FgPara = Fgravity - FgPerp;
            Vector2 FgFriction = FgPara * -1;
            DrawLine(location.x, location.y, location.x + FgFriction.x, location.y + FgFriction.y, ORANGE);*/


        EndDrawing();

}

int main()
{
    InitWindow(InitialWidth, InitialHeight, "Johnny Zimmer: 101533005 - GAME2005");
    SetTargetFPS(TARGET_FPS);
    halfspace.isStatic = true;
    halfspace.position = { 500 ,600 };
    world.add(&halfspace);
    halfspace2.setRotationDegrees(-45);

    /*halfspace2.isStatic = true;
    halfspace2.position = { 500,800 };
    halfspace2.setRotationDegrees(45);

    halfspace3.isStatic = true;
    halfspace3.position = { 800,700 };

    
    world.add(&halfspace2);
    world.add(&halfspace3);*/
    while (!WindowShouldClose())
    {
        update();
        draw();
    }

    CloseWindow();
    return 0;
}
