#include "raylib.h"
#include "rlgl.h"
#include <xstring>

using namespace std;

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
    Vector2 cellSize{ CELL_SIZE,CELL_SIZE };
  
    Color CurrentColor = BLACK;
    Color DefaultColor1 = WHITE;
    Color DefaultColor2 = LIGHTGRAY;
    int Action = 0;

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

 //RENDER TEXTURE FOR GRID TO BE EXPORTED
    RenderTexture ImageTexture = LoadRenderTexture(screenWidth, screenHeight - 200);

    Rectangle ScrRect = { 0,0,(float)ImageTexture.texture.width,(float)ImageTexture.texture.height};
    Vector2 Scrposition = { 0, 0 };

   

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

        //CONTROL KEYS
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_P)) {
            Action = 1;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_E)) {
            Action = 2;
        }
        

        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePosition = GetMousePosition();
            mousePosition.x -= Scrposition.x;
            mousePosition.y -= Scrposition.y;

            int gX = (int)(mousePosition.x / CELL_SIZE);
            int gY = (int)(mousePosition.y  / CELL_SIZE);

            //CHECKS FOR COLLSIONS WITH GRID
            if (gX >= 0 && gX < GRID_WIDTH && gY >= 0 && gY < GRID_HEIGHT) {
                if (Action == 1) {
                    grid[gX][gY].color = CurrentColor;
                }
                else if(Action == 2)
                {
                    grid[gX][gY].color = ((gX + gY) % 2 == 0) ? DefaultColor1 : DefaultColor2;
                }
            }
            
            //CHECKS FOR COLLISIONS WITH COLOUR BUTTONS
            for (int i = 0; i < paletteSize; i++) {
                Rectangle colorButton = { 10 + 40 * i,screenHeight - 100,30,30 };
                if (CheckCollisionPointRec(mousePosition, colorButton)) {
                    selectedColor = i;
                    CurrentColor = Palette[selectedColor];
                }
            }

            //BUTTONS 
            Rectangle drawButton = { 600,screenHeight - 150,100,40 };
            Rectangle eraseButton = { 725,screenHeight - 150,100,40 };
            //COLLISIONS FOR BUTTONS
            if (CheckCollisionPointRec(mousePosition, drawButton)) {
                Action = 1;
            }
            if (CheckCollisionPointRec(mousePosition, eraseButton)) {
                Action = 2;
            }
        }

        BeginTextureMode(ImageTexture);
        ClearBackground(RAYWHITE);

        for (int x = 0;x < GRID_WIDTH;x++) {
            for (int y = 0;y < GRID_HEIGHT;y++) {
                DrawRectangleV(grid[x][y].position, cellSize, grid[x][y].color);
                DrawRectangleLines(grid[x][y].position.x, grid[x][y].position.y, CELL_SIZE, CELL_SIZE, ColorAlpha(BLACK, 0));
            }
        }

        EndTextureMode();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
       
        DrawTextureRec(ImageTexture.texture,ScrRect,Scrposition, WHITE);

        DrawRectangle(0, screenHeight - 200, 1000, 200, DARKPURPLE);
        for (int i = 0;i < paletteSize;i++) {
            Rectangle colorButton = { 10 + 40 * i, screenHeight - 100, 30,30 };
            DrawRectangleRec(colorButton, Palette[i]);
            if (i == selectedColor) {
                DrawRectangleLinesEx(colorButton, 2, BLACK);
            }
        }

        Rectangle drawButton = { 600,screenHeight - 150,100,40 };
        Rectangle eraseButton = { 725,screenHeight - 150,100,40 };
        DrawRectangleRec(drawButton, (Action == 1) ? GREEN : DARKGRAY);
        DrawRectangleRec(eraseButton, (Action == 2) ? RED : DARKGRAY);
        DrawText("Draw", drawButton.x + 20, drawButton.y + 10, 20, BLACK);
        DrawText("Erase", eraseButton.x + 20, eraseButton.y + 10, 20, BLACK);


        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_S)) {
            Image gridImage = LoadImageFromTexture(ImageTexture.texture);
            ExportImage(gridImage, "pixel_grid_image.png");
            UnloadImage(gridImage);
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