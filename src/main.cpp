#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"

#include <xstring>

using namespace std;

#define GRID_WIDTH 50 
#define GRID_HEIGHT 40
#define CELL_SIZE 20 

typedef struct Cell {
    Vector2 position;
    Color color;
}Cell;


//Save Function
void SaveGridImage(RenderTexture2D ImageTexture) {
    Image gridImage = LoadImageFromTexture(ImageTexture.texture);
    ExportImage(gridImage, "pixel_grid_image.png");
    UnloadImage(gridImage);
}


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1000;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "PIXIL DRAW");

    //Variables-----------------------------------------------------------------------------
    Vector2 cellSize{ CELL_SIZE,CELL_SIZE };
  
    Color CurrentColor = BLACK;
    Color DefaultColor1 = WHITE;
    Color DefaultColor2 = LIGHTGRAY;
    int Action = 3;
    
    //Color palette varibales
    Color Palette[] = { BLACK,WHITE,RED,GREEN,BLUE,YELLOW,ORANGE,PURPLE,DARKBLUE,DARKGREEN};
    int paletteSize = sizeof(Palette) / sizeof(Palette[0]);
    int selectedColor = 0;

    //Grid initialization
    Cell grid[GRID_WIDTH][GRID_HEIGHT];
    for (int x = 0;x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT;y++) {
            grid[x][y].position.x = x * cellSize.x;
            grid[x][y].position.y = y * cellSize.y;
            grid[x][y].color = ((x + y) % 2 == 0) ? DefaultColor1 : DefaultColor2;
        }
    }

    //Selection Tool 
    bool Selecting = false;
    Vector2 SelectStart{}, SelectEnd{};
    Color SelectionBuffer[GRID_WIDTH][GRID_HEIGHT];
    bool BeenSelected = false;

    int startX = (int)SelectStart.x;
    int startY = (int)SelectStart.y;
    int endX = (int)SelectEnd.x;
    int endY = (int)SelectEnd.y;

    bool showMessageBox = false;
        
   //----------------------------------------------------------------------------------------

 //RENDER TEXTURE FOR GRID TO BE EXPORTED
    RenderTexture ImageTexture = LoadRenderTexture(screenWidth, screenHeight - 200);

 //Textures size + position                                  //if not minus its inverted 
    Rectangle ScrRect = { 0,0,(float)ImageTexture.texture.width,-(float)ImageTexture.texture.height};
    Vector2 Scrposition = { 0,0 };

   

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
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_S)) {
            SaveGridImage(ImageTexture);
        }
        
        //Drawing Mechanic
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
            Rectangle SaveButton = { 825,screenHeight - 50,100,40 };
            Rectangle SelectionButton = { 850,screenHeight - 150,100,40 };

            //COLLISIONS FOR BUTTONS
            if (CheckCollisionPointRec(mousePosition, drawButton)) {
                Action = 1;
            }
            if (CheckCollisionPointRec(mousePosition, eraseButton)) {
                Action = 2;
            }
            if (CheckCollisionPointRec(mousePosition, SaveButton)) {
                SaveGridImage(ImageTexture);
            }
            if (CheckCollisionPointRec(mousePosition, SelectionButton)) {
                Action = 3;
            }
            

            //SELECTION
            if (Action == 3) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (!Selecting) {
                        Selecting = true;
                        BeenSelected = false;
                        SelectStart.x = (int)(mousePosition.x / CELL_SIZE);
                        SelectStart.y = (int)(mousePosition.y / CELL_SIZE);
                    }
                    else {
                        Selecting = false;
                        SelectEnd = mousePosition;

                        SelectEnd.x = (int)(mousePosition.x / CELL_SIZE);
                        SelectEnd.y = (int)(mousePosition.y / CELL_SIZE);

                        BeenSelected = true;
                        startX = min((int)SelectStart.x, (int)SelectEnd.x);
                        startY = min((int)SelectStart.y, (int)SelectEnd.y);
                        endX = max((int)SelectStart.x, (int)SelectEnd.x);
                        endY = max((int)SelectStart.y, (int)SelectEnd.y);

                        //COPY THE SELECTED AREA
                        for (int x = startX; x <= endX; x++) {
                            for (int y = startY; y <= endY; y++) {
                                SelectionBuffer[x - startX][y - startY] = grid[x][y].color;
                            }
                        }
                    }
                }  
            }  
        } 

        //Paste the copied area 
        if (BeenSelected && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    Vector2 PastePos = GetMousePosition();
                    int pasteX = (int)(PastePos.x / CELL_SIZE);
                    int pasteY = (int)(PastePos.y / CELL_SIZE);

                    for (int x = 0; x <= endX - startX; x++) {
                        for (int y = 0; y <= endY - startY; y++) {
                            int gX = pasteX + x;
                            int gY = pasteY + y;
                            if (gX >= 0 && gX < GRID_WIDTH && gY >= 0 && gY < GRID_HEIGHT) {
                                grid[gX][gY].color = SelectionBuffer[x][y];
                            }
                        }
                    }
        }

        //TEXTURE
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

        //drawing the buttons 
        Rectangle drawButton = { 600,screenHeight - 150,100,40 };
        Rectangle eraseButton = { 725,screenHeight - 150,100,40 };
        Rectangle SaveButton = { 825,screenHeight - 50,100,40 };
        Rectangle SelectionButton = { 850,screenHeight - 150,100,40 };

        //buttons Colors
        DrawRectangleRec(drawButton, (Action == 1) ? DARKGRAY : GREEN);
        DrawRectangleRec(eraseButton, (Action == 2) ? DARKGRAY : GREEN); 
        DrawRectangleRec(SelectionButton, (Action == 3) ? DARKGRAY : GREEN);
        DrawRectangleRec(SaveButton, BLUE);
       
        //drawing the text 
        DrawText("Draw", drawButton.x + 20, drawButton.y + 10, 20, BLACK);
        DrawText("Erase", eraseButton.x + 20, eraseButton.y + 10, 20, BLACK);
        DrawText("Save", SaveButton.x + 20, SaveButton.y + 10, 20, BLACK);
        DrawText("Selection", SelectionButton.x + 20, SelectionButton.y + 10, 20, BLACK);

        //Drawing Selecting
        if (Action == 3 && Selecting) {
            Vector2 MousePos = GetMousePosition();
            int selWidth = (int)(MousePos.x - SelectStart.x * CELL_SIZE);
            int selHeight = (int)(MousePos.y - SelectStart.y * CELL_SIZE);
            DrawRectangleLines(SelectStart.x* CELL_SIZE, SelectStart.y* CELL_SIZE, selWidth, selHeight, GREEN);
        }


        if (GuiButton({ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

        if (showMessageBox)
        {
            int result = GuiMessageBox({ 85, 70, 250, 100 },
                "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

            if (result >= 0) showMessageBox = false;
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