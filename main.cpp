#include "raylib.h"

#define MAX_CELLS 1000



typedef struct Cell {
    Vector2 position;
    float width;
    float height;
    Color color;
}Cell;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "PIXIL DRAW");

    //Variables
    Cell cells[MAX_CELLS];
    int CellCount = 0;
    Color CurrentColor = BLACK;
    Color DefaultColor1 = WHITE;
    Color DefaultColor2 = GRAY;
    Vector2 CellStart = { 0 };
    bool Drawing = false;

    Color Palette[] = { BLACK,WHITE,RED,GREEN,BLUE,YELLOW,ORANGE,PURPLE,DARKBLUE,DARKGREEN };
    int paletteSize = sizeof(Palette) / sizeof(Palette[0]);
    int selectedColor = 0;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        if (IsKeyReleased(KEY_EQUAL)) {  // Press '=' to go forward in the palette
            selectedColor = (selectedColor + 1) % paletteSize;
            CurrentColor = Palette[selectedColor];
        }
        else if (IsKeyReleased(KEY_MINUS)) {  // Press '-' to go backward in the palette
            selectedColor = (selectedColor - 1 + paletteSize) % paletteSize;
            CurrentColor = Palette[selectedColor];
        }
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            CellStart = GetMousePosition();
            if (CellCount < MAX_CELLS) {
                Vector2 CellEnd = GetMousePosition();

                cells[CellCount].position.x = CellStart.x;
                cells[CellCount].position.y = CellStart.y;
                cells[CellCount].width = 10;
                cells[CellCount].height = 10;
                CellCount++;

                cells[CellCount].color = CurrentColor;
              
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, screenHeight - 200, 1000, 200, DARKPURPLE);
        for (int i = 0;i < paletteSize;i++) {
            Rectangle colorButton = { 10 + 40 * i, screenHeight - 100, 30,30 };
            DrawRectangleRec(colorButton, Palette[i]);
            if (i == selectedColor) {
                DrawRectangleLinesEx(colorButton, 2, BLACK);
            }
        }

        for (int i = 0; i < CellCount;i++) {
            DrawRectangleRec({ cells[i].position.x,cells[i].position.y,
                              cells[i].width,cells[i].height }, cells[i].color);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}