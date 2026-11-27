#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define BUBBLE_COUNT 8
#define FISH_COUNT 2

typedef struct {
    Vector2 position;
    float speed;
} Submarine;

typedef struct {
    Vector2 position;
    float radius;
    float speed;
} Bubble;

typedef struct {
    Vector2 position;
    float speed;
    Color color;
} Fish;

static Submarine submarine = {{150, 270}, 230};
static Bubble bubbles[BUBBLE_COUNT] = {
    {{80, 420}, 5, 35}, {{210, 350}, 8, 48},
    {{330, 470}, 4, 30}, {{470, 390}, 7, 42},
    {{590, 500}, 6, 38}, {{710, 360}, 5, 32},
    {{820, 460}, 8, 45}, {{900, 410}, 4, 28}
};
static Fish fish[FISH_COUNT] = {
    {{300, 170}, 65, ORANGE},
    {{670, 245}, -45, GOLD}
};

static void DrawFish(Fish item);
static void DrawSubmarine(void);
static void DrawControls(void);
static void UpdateGame(void);
static void DrawGame(void);
static void UpdateDrawFrame(void);
static bool ControlPressed(Rectangle area);

static void DrawFish(Fish item)
{
    bool facingRight = item.speed > 0;
    float x = item.position.x;
    float y = item.position.y;

    DrawEllipse((int)x, (int)y, 25, 13, item.color);

    if (facingRight) {
        DrawTriangle((Vector2){x - 20, y},
                     (Vector2){x - 39, y - 12},
                     (Vector2){x - 39, y + 12}, item.color);
        DrawCircle((int)(x + 12), (int)(y - 2), 3, BLACK);
    } else {
        DrawTriangle((Vector2){x + 20, y},
                     (Vector2){x + 39, y - 12},
                     (Vector2){x + 39, y + 12}, item.color);
        DrawCircle((int)(x - 12), (int)(y - 2), 3, BLACK);
    }
}

static void DrawSubmarine(void)
{
    float x = submarine.position.x;
    float y = submarine.position.y;

    DrawRectangle((int)(x - 58), (int)(y - 7), 18, 14, DARKGRAY);
    DrawEllipse((int)x, (int)y, 58, 24, (Color){224, 154, 56, 255});
    DrawRectangle((int)(x - 19), (int)(y - 29), 38, 24, (Color){198, 128, 38, 255});
    DrawEllipse((int)x, (int)(y - 19), 12, 9, SKYBLUE);
    DrawCircle((int)(x + 30), (int)y, 4, LIGHTGRAY);
    DrawRectangle((int)(x - 13), (int)(y + 20), 26, 6, DARKGRAY);
}

static Rectangle GetControlArea(int direction)
{
    float centerX = SCREEN_WIDTH - 112;
    float centerY = SCREEN_HEIGHT - 105;
    float size = 38;

    if (direction == 0) return (Rectangle){centerX, centerY - 43, size, size};
    if (direction == 1) return (Rectangle){centerX - 43, centerY, size, size};
    if (direction == 2) return (Rectangle){centerX + 43, centerY, size, size};
    return (Rectangle){centerX, centerY + 43, size, size};
}

static bool ControlPressed(Rectangle area)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), area)) return true;

    for (int i = 0; i < GetTouchPointCount(); i++) {
        if (CheckCollisionPointRec(GetTouchPosition(i), area)) return true;
    }

    return false;
}

static void DrawControls(void)
{
    Color buttonColor = (Color){180, 220, 235, 180};
    Rectangle up = GetControlArea(0);
    Rectangle left = GetControlArea(1);
    Rectangle right = GetControlArea(2);
    Rectangle down = GetControlArea(3);

    DrawRectangleRec(up, buttonColor);
    DrawRectangleRec(left, buttonColor);
    DrawRectangleRec(right, buttonColor);
    DrawRectangleRec(down, buttonColor);

    DrawTriangle((Vector2){up.x + 19, up.y + 8},
                 (Vector2){up.x + 8, up.y + 29},
                 (Vector2){up.x + 30, up.y + 29}, DARKBLUE);
    DrawTriangle((Vector2){left.x + 8, left.y + 19},
                 (Vector2){left.x + 29, left.y + 8},
                 (Vector2){left.x + 29, left.y + 30}, DARKBLUE);
    DrawTriangle((Vector2){right.x + 30, right.y + 19},
                 (Vector2){right.x + 9, right.y + 8},
                 (Vector2){right.x + 9, right.y + 30}, DARKBLUE);
    DrawTriangle((Vector2){down.x + 19, down.y + 30},
                 (Vector2){down.x + 8, down.y + 9},
                 (Vector2){down.x + 30, down.y + 9}, DARKBLUE);
}

static void UpdateGame(void)
{
    float deltaTime = GetFrameTime();
    float moveX = 0;
    float moveY = 0;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveX -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX += 1;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) moveY -= 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) moveY += 1;

    if (ControlPressed(GetControlArea(0))) moveY -= 1;
    if (ControlPressed(GetControlArea(1))) moveX -= 1;
    if (ControlPressed(GetControlArea(2))) moveX += 1;
    if (ControlPressed(GetControlArea(3))) moveY += 1;

    submarine.position.x += moveX * submarine.speed * deltaTime;
    submarine.position.y += moveY * submarine.speed * deltaTime;

    if (submarine.position.x < 65) submarine.position.x = 65;
    if (submarine.position.x > SCREEN_WIDTH - 65) submarine.position.x = SCREEN_WIDTH - 65;
    if (submarine.position.y < 65) submarine.position.y = 65;
    if (submarine.position.y > SCREEN_HEIGHT - 65) submarine.position.y = SCREEN_HEIGHT - 65;

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        bubbles[i].position.y -= bubbles[i].speed * deltaTime;

        if (bubbles[i].position.y < -10) {
            bubbles[i].position.y = SCREEN_HEIGHT + 10;
            bubbles[i].position.x = (float)GetRandomValue(15, SCREEN_WIDTH - 15);
        }
    }

    for (int i = 0; i < FISH_COUNT; i++) {
        fish[i].position.x += fish[i].speed * deltaTime;

        if (fish[i].speed > 0 && fish[i].position.x > SCREEN_WIDTH + 45)
            fish[i].position.x = -45;
        if (fish[i].speed < 0 && fish[i].position.x < -45)
            fish[i].position.x = SCREEN_WIDTH + 45;
    }
}

static void DrawGame(void)
{
    ClearBackground((Color){5, 47, 82, 255});

    DrawRectangle(0, 0, SCREEN_WIDTH, 120, Fade((Color){26, 116, 150, 255}, 0.25f));
    DrawRectangle(0, 445, SCREEN_WIDTH, 95, (Color){13, 74, 70, 255});

    DrawTriangle((Vector2){0, 445}, (Vector2){170, 340},
                 (Vector2){280, 445}, (Color){10, 87, 78, 255});
    DrawTriangle((Vector2){650, 445}, (Vector2){790, 325},
                 (Vector2){960, 445}, (Color){9, 77, 72, 255});

    DrawEllipse(815, 410, 74, 43, BLACK);
    DrawRectangle(773, 410, 84, 45, (Color){9, 53, 59, 255});
    DrawRectangle(795, 390, 42, 25, BROWN);
    DrawRectangle(795, 400, 42, 7, GOLD);
    DrawCircle(815, 386, 8, GOLD);

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        DrawCircleLines((int)bubbles[i].position.x,
                        (int)bubbles[i].position.y,
                        bubbles[i].radius, Fade(SKYBLUE, 0.8f));
    }

    for (int i = 0; i < FISH_COUNT; i++) DrawFish(fish[i]);

    DrawLine(60, 445, 90, 390, DARKGREEN);
    DrawLine(90, 445, 112, 380, DARKGREEN);
    DrawLine(112, 445, 140, 400, DARKGREEN);
    DrawLine(895, 445, 915, 390, DARKGREEN);
    DrawLine(915, 445, 940, 380, DARKGREEN);

    DrawSubmarine();

    DrawText("UNDERWATER TREASURE HUNT", 24, 20, 24, RAYWHITE);
    DrawText("Explore the ocean and find the lost treasure.", 26, 52, 16, SKYBLUE);
    DrawText("MOVE: W A S D / ARROW KEYS", 24, SCREEN_HEIGHT - 28, 14, RAYWHITE);

    DrawControls();
}

static void UpdateDrawFrame(void)
{
    UpdateGame();
    BeginDrawing();
    DrawGame();
    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Treasure Hunt");
    SetTargetFPS(60);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) UpdateDrawFrame();
    CloseWindow();
#endif

    return 0;
}
