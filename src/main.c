#include "raylib.h"

#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define BUBBLE_COUNT 9
#define FISH_COUNT 3
#define OBSTACLE_COUNT 3
#define SUBMARINE_RADIUS 25
#define TREASURE_RADIUS 55

typedef enum {
    MENU,
    PLAYING,
    COMPLETE
} GameState;

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

typedef struct {
    Vector2 position;
    float radius;
} Obstacle;

typedef struct {
    Vector2 position;
    bool collected;
} Treasure;

static GameState gameState = MENU;
static Submarine submarine = {{120, 255}, 205};
static Bubble bubbles[BUBBLE_COUNT] = {
    {{65, 415}, 5, 34}, {{175, 330}, 8, 46}, {{285, 470}, 4, 30},
    {{405, 390}, 7, 40}, {{530, 495}, 6, 37}, {{640, 350}, 5, 32},
    {{750, 455}, 8, 44}, {{850, 300}, 4, 28}, {{920, 430}, 6, 35}
};
static Fish fish[FISH_COUNT] = {
    {{260, 150}, 58, ORANGE},
    {{610, 235}, -46, GOLD},
    {{850, 175}, 38, SKYBLUE}
};
static const Obstacle obstacles[OBSTACLE_COUNT] = {
    {{355, 205}, 43},
    {{545, 350}, 48},
    {{690, 190}, 39}
};
static Treasure treasure = {{835, 370}, false};

static float missionTime = 0;
static float hitTimer = 0;
static float messageTimer = 0;
static bool resultSaved = false;
static char saveMessage[96] = "Mission result has not been saved.";

static void UpdateEnvironment(float deltaTime);
static void DrawEnvironment(void);
static void DrawFish(Fish item);
static void DrawSubmarine(void);
static void DrawControls(void);
static void UpdateMenu(void);
static void UpdateGame(float deltaTime);
static void UpdateComplete(void);
static void DrawMenu(void);
static void DrawGame(void);
static void DrawComplete(void);
static void UpdateDrawFrame(void);
static bool ButtonPressed(Rectangle area);
static bool IsBlocked(Vector2 position);
static bool NearTreasure(void);
static void StartGame(void);
static void SaveMissionResult(void);
static void DrawButton(Rectangle area, const char *label, Color color);
static void DrawCenteredText(const char *text, int y, int size, Color color);

static Rectangle GetControlArea(int direction)
{
    const float centerX = 112;
    const float centerY = 430;
    const float size = 42;

    if (direction == 0) return (Rectangle){centerX, centerY - 48, size, size};
    if (direction == 1) return (Rectangle){centerX - 48, centerY, size, size};
    if (direction == 2) return (Rectangle){centerX + 48, centerY, size, size};
    return (Rectangle){centerX, centerY + 48, size, size};
}

static Rectangle GetActionArea(void)
{
    return (Rectangle){SCREEN_WIDTH - 160, SCREEN_HEIGHT - 88, 128, 54};
}

static bool ButtonPressed(Rectangle area)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), area)) return true;

    for (int i = 0; i < GetTouchPointCount(); i++) {
        if (CheckCollisionPointRec(GetTouchPosition(i), area)) return true;
    }

    return false;
}

static void DrawCenteredText(const char *text, int y, int size, Color color)
{
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, size)) / 2, y, size, color);
}

static void DrawButton(Rectangle area, const char *label, Color color)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, area);
    Color buttonColor = hovered ? ColorBrightness(color, 0.18f) : color;
    int textSize = 20;

    DrawRectangleRounded(area, 0.22f, 8, buttonColor);
    DrawRectangleRoundedLines(area, 0.22f, 8, Fade(RAYWHITE, 0.8f));
    DrawText(label,
             (int)(area.x + (area.width - MeasureText(label, textSize)) / 2),
             (int)(area.y + (area.height - textSize) / 2),
             textSize, RAYWHITE);
}

static bool ControlHeld(Rectangle area)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), area)) return true;

    for (int i = 0; i < GetTouchPointCount(); i++) {
        if (CheckCollisionPointRec(GetTouchPosition(i), area)) return true;
    }

    return false;
}

static void DrawFish(Fish item)
{
    float x = item.position.x;
    float y = item.position.y;

    DrawEllipse((int)x, (int)y, 24, 13, item.color);

    if (item.speed >= 0) {
        DrawTriangle((Vector2){x - 18, y},
                     (Vector2){x - 37, y - 12},
                     (Vector2){x - 37, y + 12}, item.color);
        DrawCircle((int)(x + 11), (int)(y - 2), 3, DARKBLUE);
    } else {
        DrawTriangle((Vector2){x + 18, y},
                     (Vector2){x + 37, y - 12},
                     (Vector2){x + 37, y + 12}, item.color);
        DrawCircle((int)(x - 11), (int)(y - 2), 3, DARKBLUE);
    }
}

static void DrawSubmarine(void)
{
    float x = submarine.position.x;
    float y = submarine.position.y;
    Color body = (Color){226, 151, 48, 255};
    Color darkBody = (Color){163, 91, 28, 255};

    DrawRectangle((int)(x - 62), (int)(y - 7), 20, 14, DARKGRAY);
    DrawEllipse((int)x, (int)y, 58, 25, body);
    DrawRectangle((int)(x - 20), (int)(y - 31), 40, 25, darkBody);
    DrawEllipse((int)x, (int)(y - 22), 12, 9, SKYBLUE);
    DrawCircle((int)(x + 29), (int)y, 4, RAYWHITE);
    DrawRectangle((int)(x - 14), (int)(y + 21), 28, 6, DARKGRAY);
    DrawLine((int)(x - 38), (int)(y + 4), (int)(x + 38), (int)(y + 4), Fade(BLACK, 0.25f));
}

static void DrawObstacle(Obstacle item)
{
    int x = (int)item.position.x;
    int y = (int)item.position.y;
    int r = (int)item.radius;

    DrawEllipse(x, y + 8, r + 8, r - 5, (Color){8, 37, 51, 255});
    DrawEllipse(x, y, r, r - 7, (Color){74, 100, 105, 255});
    DrawEllipse(x - r / 3, y - r / 4, r / 3, r / 5, (Color){112, 139, 137, 255});
    DrawLine(x - r / 3, y + r / 5, x + r / 3, y + r / 4, (Color){48, 72, 78, 255});
}

static void DrawCave(void)
{
    DrawEllipse(840, 396, 103, 76, (Color){10, 34, 48, 255});
    DrawEllipse(842, 397, 82, 62, (Color){2, 15, 29, 255});
    DrawEllipse(790, 365, 35, 24, (Color){76, 101, 99, 255});
    DrawEllipse(895, 368, 36, 28, (Color){73, 98, 97, 255});
    DrawEllipse(830, 334, 46, 22, (Color){82, 107, 104, 255});
    DrawText("HIDDEN CAVE", 790, 300, 14, Fade(RAYWHITE, 0.85f));
}

static void DrawTreasure(void)
{
    if (!treasure.collected) {
        DrawCircle((int)treasure.position.x, (int)treasure.position.y - 5,
                   48, Fade(GOLD, 0.12f));
        DrawRectangle(811, 360, 49, 34, BROWN);
        DrawRectangle(808, 353, 55, 13, (Color){151, 82, 36, 255});
        DrawRectangle(808, 369, 55, 7, GOLD);
        DrawRectangle(831, 360, 8, 18, GOLD);
        DrawCircle(835, 373, 3, DARKBROWN);
    } else {
        DrawRectangle(811, 360, 49, 34, BROWN);
        DrawRectangle(808, 348, 55, 12, (Color){163, 91, 39, 255});
        DrawRectangle(808, 368, 55, 7, GOLD);
        DrawLine(815, 360, 825, 345, GOLD);
        DrawLine(856, 360, 845, 345, GOLD);
    }
}

static void DrawSeaPlants(void)
{
    Color plant = (Color){30, 130, 103, 255};

    DrawLineEx((Vector2){45, 454}, (Vector2){67, 397}, 5, plant);
    DrawLineEx((Vector2){67, 454}, (Vector2){90, 390}, 5, plant);
    DrawLineEx((Vector2){90, 454}, (Vector2){112, 412}, 5, plant);
    DrawLineEx((Vector2){240, 454}, (Vector2){255, 405}, 4, plant);
    DrawLineEx((Vector2){255, 454}, (Vector2){282, 417}, 4, plant);
    DrawLineEx((Vector2){925, 454}, (Vector2){940, 395}, 5, plant);
    DrawLineEx((Vector2){900, 454}, (Vector2){913, 410}, 4, plant);
}

static void UpdateEnvironment(float deltaTime)
{
    for (int i = 0; i < BUBBLE_COUNT; i++) {
        bubbles[i].position.y -= bubbles[i].speed * deltaTime;

        if (bubbles[i].position.y < -12) {
            bubbles[i].position.y = SCREEN_HEIGHT + 12;
            bubbles[i].position.x = (float)GetRandomValue(15, SCREEN_WIDTH - 15);
        }
    }

    for (int i = 0; i < FISH_COUNT; i++) {
        fish[i].position.x += fish[i].speed * deltaTime;

        if (fish[i].speed > 0 && fish[i].position.x > SCREEN_WIDTH + 42)
            fish[i].position.x = -42;
        if (fish[i].speed < 0 && fish[i].position.x < -42)
            fish[i].position.x = SCREEN_WIDTH + 42;
    }
}

static void DrawEnvironment(void)
{
    ClearBackground((Color){4, 42, 73, 255});

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           (Color){18, 103, 139, 255},
                           (Color){3, 31, 58, 255});
    DrawRectangle(0, 446, SCREEN_WIDTH, 94, (Color){19, 74, 68, 255});
    DrawTriangle((Vector2){0, 446}, (Vector2){155, 354},
                 (Vector2){290, 446}, (Color){15, 81, 75, 255});
    DrawTriangle((Vector2){580, 446}, (Vector2){740, 345},
                 (Vector2){960, 446}, (Color){12, 73, 70, 255});

    for (int i = 0; i < BUBBLE_COUNT; i++) {
        DrawCircleLines((int)bubbles[i].position.x,
                        (int)bubbles[i].position.y,
                        bubbles[i].radius, Fade(SKYBLUE, 0.8f));
        DrawCircle((int)(bubbles[i].position.x - 2),
                   (int)(bubbles[i].position.y - 2), 2,
                   Fade(RAYWHITE, 0.65f));
    }

    for (int i = 0; i < FISH_COUNT; i++) DrawFish(fish[i]);

    DrawSeaPlants();
    DrawCave();
    DrawTreasure();
}

static bool IsBlocked(Vector2 position)
{
    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        if (CheckCollisionCircles(position, SUBMARINE_RADIUS,
                                  obstacles[i].position, obstacles[i].radius)) {
            return true;
        }
    }

    return false;
}

static bool NearTreasure(void)
{
    return CheckCollisionCircles(submarine.position, 35,
                                 treasure.position, TREASURE_RADIUS);
}

static void StartGame(void)
{
    submarine.position = (Vector2){120, 255};
    treasure.collected = false;
    missionTime = 0;
    hitTimer = 0;
    messageTimer = 0;
    resultSaved = false;
    saveMessage[0] = '\0';
    gameState = PLAYING;
}

static void SaveMissionResult(void)
{
    FILE *file = fopen("mission_result.txt", "w");

    if (file == NULL) {
        resultSaved = false;
        snprintf(saveMessage, sizeof(saveMessage),
                 "Could not save mission_result.txt.");
        return;
    }

    if (fprintf(file, "Underwater Treasure Hunt\nMission: Complete\nTime: %.1f seconds\n",
                missionTime) < 0) {
        resultSaved = false;
        snprintf(saveMessage, sizeof(saveMessage),
                 "The result file could not be written.");
    } else {
        resultSaved = true;
        snprintf(saveMessage, sizeof(saveMessage),
                 "Result saved to mission_result.txt for this session.");
    }

    if (fclose(file) != 0) {
        resultSaved = false;
        snprintf(saveMessage, sizeof(saveMessage),
                 "The result file could not be closed correctly.");
    }
}

static void UpdateMenu(void)
{
    Rectangle startButton = {375, 365, 210, 58};

    if (ButtonPressed(startButton) || IsKeyPressed(KEY_ENTER)) {
        StartGame();
    }
}

static void UpdateGame(float deltaTime)
{
    float moveX = 0;
    float moveY = 0;
    Vector2 nextPosition;

    missionTime += deltaTime;
    if (hitTimer > 0) hitTimer -= deltaTime;
    if (messageTimer > 0) messageTimer -= deltaTime;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveX -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX += 1;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) moveY -= 1;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) moveY += 1;

    if (ControlHeld(GetControlArea(0))) moveY -= 1;
    if (ControlHeld(GetControlArea(1))) moveX -= 1;
    if (ControlHeld(GetControlArea(2))) moveX += 1;
    if (ControlHeld(GetControlArea(3))) moveY += 1;

    if (moveX != 0 && moveY != 0) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    nextPosition = submarine.position;
    nextPosition.x += moveX * submarine.speed * deltaTime;

    if (nextPosition.x < 42) nextPosition.x = 42;
    if (nextPosition.x > SCREEN_WIDTH - 42) nextPosition.x = SCREEN_WIDTH - 42;

    if (!IsBlocked(nextPosition)) {
        submarine.position.x = nextPosition.x;
    } else {
        hitTimer = 0.35f;
    }

    nextPosition = submarine.position;
    nextPosition.y += moveY * submarine.speed * deltaTime;

    if (nextPosition.y < 72) nextPosition.y = 72;
    if (nextPosition.y > 410) nextPosition.y = 410;

    if (!IsBlocked(nextPosition)) {
        submarine.position.y = nextPosition.y;
    } else {
        hitTimer = 0.35f;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        gameState = MENU;
        return;
    }

    if (!treasure.collected &&
        (IsKeyPressed(KEY_SPACE) || ButtonPressed(GetActionArea()))) {
        if (NearTreasure()) {
            treasure.collected = true;
            SaveMissionResult();
            gameState = COMPLETE;
        } else {
            messageTimer = 2.0f;
        }
    }
}

static void UpdateComplete(void)
{
    Rectangle restartButton = {270, 380, 190, 58};
    Rectangle menuButton = {500, 380, 190, 58};

    if (ButtonPressed(restartButton) || IsKeyPressed(KEY_ENTER)) {
        StartGame();
    }

    if (ButtonPressed(menuButton) || IsKeyPressed(KEY_ESCAPE)) {
        gameState = MENU;
    }
}

static void DrawMenu(void)
{
    Rectangle startButton = {375, 365, 210, 58};

    DrawEnvironment();
    DrawSubmarine();

    DrawRectangle(120, 82, 720, 360, Fade((Color){2, 20, 38, 255}, 0.78f));
    DrawRectangleLines(120, 82, 720, 360, Fade(SKYBLUE, 0.7f));

    DrawCenteredText("UNDERWATER", 125, 34, RAYWHITE);
    DrawCenteredText("TREASURE HUNT", 165, 38, GOLD);
    DrawCenteredText("THE LOST TREASURE", 216, 19, SKYBLUE);
    DrawCenteredText("Explore the ocean, avoid the rocks, and find the hidden cave.", 265, 17, RAYWHITE);
    DrawCenteredText("W A S D / ARROWS to move  |  SPACE to collect", 298, 16, Fade(RAYWHITE, 0.9f));
    DrawCenteredText("On phones, use the on-screen controls.", 323, 16, Fade(RAYWHITE, 0.9f));

    DrawButton(startButton, "START EXPLORING", (Color){24, 119, 143, 255});
}

static void DrawControls(void)
{
    Rectangle up = GetControlArea(0);
    Rectangle left = GetControlArea(1);
    Rectangle right = GetControlArea(2);
    Rectangle down = GetControlArea(3);
    Rectangle action = GetActionArea();
    Color button = (Color){188, 221, 231, 205};

    DrawRectangleRounded(up, 0.2f, 6, button);
    DrawRectangleRounded(left, 0.2f, 6, button);
    DrawRectangleRounded(right, 0.2f, 6, button);
    DrawRectangleRounded(down, 0.2f, 6, button);

    DrawTriangle((Vector2){up.x + 21, up.y + 8},
                 (Vector2){up.x + 9, up.y + 31},
                 (Vector2){up.x + 33, up.y + 31}, DARKBLUE);
    DrawTriangle((Vector2){left.x + 8, left.y + 21},
                 (Vector2){left.x + 31, left.y + 9},
                 (Vector2){left.x + 31, left.y + 33}, DARKBLUE);
    DrawTriangle((Vector2){right.x + 34, right.y + 21},
                 (Vector2){right.x + 11, right.y + 9},
                 (Vector2){right.x + 11, right.y + 33}, DARKBLUE);
    DrawTriangle((Vector2){down.x + 21, down.y + 34},
                 (Vector2){down.x + 9, down.y + 11},
                 (Vector2){down.x + 33, down.y + 11}, DARKBLUE);

    DrawButton(action, "COLLECT", (Color){169, 112, 28, 245});
}

static void DrawGame(void)
{
    char timerText[48];

    DrawEnvironment();

    for (int i = 0; i < OBSTACLE_COUNT; i++) DrawObstacle(obstacles[i]);

    DrawSubmarine();

    DrawRectangle(0, 0, SCREEN_WIDTH, 66, Fade((Color){1, 20, 38, 255}, 0.72f));
    DrawText("MISSION: FIND THE LOST TREASURE", 20, 14, 20, RAYWHITE);
    snprintf(timerText, sizeof(timerText), "TIME: %.1f s", missionTime);
    DrawText(timerText, SCREEN_WIDTH - 150, 18, 18, GOLD);

    if (NearTreasure() && !treasure.collected) {
        DrawText("PRESS SPACE OR COLLECT", 655, 280, 17, GOLD);
    } else if (messageTimer > 0) {
        DrawText("GET CLOSER TO THE CHEST", 350, 92, 18, GOLD);
    }

    if (hitTimer > 0) {
        DrawRectangle(0, 66, SCREEN_WIDTH, SCREEN_HEIGHT - 66,
                      Fade(RED, 0.14f));
        DrawText("ROCK! FIND ANOTHER WAY.", 350, 92, 18, RAYWHITE);
    }

    DrawText("ESC: MENU", 20, SCREEN_HEIGHT - 28, 14, RAYWHITE);
    DrawControls();
}

static void DrawComplete(void)
{
    Rectangle restartButton = {270, 380, 190, 58};
    Rectangle menuButton = {500, 380, 190, 58};
    char result[64];

    DrawEnvironment();
    DrawSubmarine();

    DrawRectangle(150, 78, 660, 390, Fade((Color){2, 19, 35, 255}, 0.88f));
    DrawRectangleLines(150, 78, 660, 390, Fade(GOLD, 0.85f));

    DrawCenteredText("MISSION COMPLETE!", 126, 36, GOLD);
    DrawCenteredText("You found the lost treasure.", 188, 22, RAYWHITE);
    snprintf(result, sizeof(result), "Exploration time: %.1f seconds", missionTime);
    DrawCenteredText(result, 232, 19, SKYBLUE);

    if (resultSaved) {
        DrawCenteredText("Mission result saved for this session.", 274, 16, GREEN);
    } else {
        DrawCenteredText("Mission completed, but saving failed.", 274, 16, ORANGE);
    }
    DrawCenteredText(saveMessage, 304, 14, Fade(RAYWHITE, 0.85f));

    DrawButton(restartButton, "PLAY AGAIN", (Color){24, 119, 143, 255});
    DrawButton(menuButton, "MAIN MENU", (Color){85, 99, 111, 255});
}

static void UpdateDrawFrame(void)
{
    float deltaTime = GetFrameTime();

    if (deltaTime > 0.05f) deltaTime = 0.05f;

    UpdateEnvironment(deltaTime);

    if (gameState == MENU) UpdateMenu();
    else if (gameState == PLAYING) UpdateGame(deltaTime);
    else if (gameState == COMPLETE) UpdateComplete();

    BeginDrawing();

    if (gameState == MENU) DrawMenu();
    else if (gameState == PLAYING) DrawGame();
    else DrawComplete();

    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Underwater Treasure Hunt");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) UpdateDrawFrame();
    CloseWindow();
#endif

    return 0;
}
