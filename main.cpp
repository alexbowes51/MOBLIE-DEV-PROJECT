#include "raylib.h"

#define GRID_WIDTH 50 
#define GRID_HEIGHT 40
#define CELL_SIZE 20 



typedef struct Cell {
    Vector2 position;
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
    Vector2 cellSize{ 20,20 };
  
    Color CurrentColor = BLACK;
    Color DefaultColor1 = WHITE;
    Color DefaultColor2 = LIGHTGRAY;
    bool Drawing = false;

    Color Palette[] = { BLACK,WHITE,RED,GREEN,BLUE,YELLOW,ORANGE,PURPLE,DARKBLUE,DARKGREEN };
    int paletteSize = sizeof(Palette) / sizeof(Palette[0]);
    int selectedColor = 0;

    Cell grid[GRID_WIDTH][GRID_HEIGHT];

    for (int x = 0;x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT;y++) {
            grid[x][y].position.x = x * cellSize.x;
            grid[x][y].position.y = y * cellSize.y;
            grid[x][y].color = ((x + y) % 2 == 0) ? DefaultColor1 : DefaultColor2;
        }
    }

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
            Vector2 mousePosition = GetMousePosition();
            int gX = mousePosition.x / CELL_SIZE;
            int gY = mousePosition.y / CELL_SIZE;

            if (gX >= 0 && gX < GRID_WIDTH && gY >= 0 && gY < GRID_HEIGHT) {
                grid[gX][gY].color = CurrentColor;
            }
              
            
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int x = 0; x < GRID_WIDTH;x++) {
            for (int y = 0;y < GRID_HEIGHT;y++) {
                DrawRectangleV(grid[x][y].position, cellSize, grid[x][y].color);

                DrawRectangleLines(grid[x][y].position.x, grid[x][y].position.y, CELL_SIZE, CELL_SIZE,ColorAlpha(BLACK,0));
            }
        }

        DrawRectangle(0, screenHeight - 200, 1000, 200, DARKPURPLE);
        for (int i = 0;i < paletteSize;i++) {
            Rectangle colorButton = { 10 + 40 * i, screenHeight - 100, 30,30 };
            DrawRectangleRec(colorButton, Palette[i]);
            if (i == selectedColor) {
                DrawRectangleLinesEx(colorButton, 2, BLACK);
            }
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