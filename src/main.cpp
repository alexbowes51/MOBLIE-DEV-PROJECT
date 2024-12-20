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
    ImageFlipVertical(&gridImage);
    ExportImage(gridImage, "pixel_grid_image.png");
    UnloadImage(gridImage);
}

// Load Function
void LoadGridImage(const char* filename, Cell grid[GRID_WIDTH][GRID_HEIGHT]) {
    Image gridImage = LoadImage(filename);

    // Resize image to match grid dimensions if necessary
    ImageResize(&gridImage, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);

    // Iterate through the resized image and update grid colors
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            Color pixelColor = GetImageColor(gridImage, x * CELL_SIZE, y * CELL_SIZE);
            grid[x][y].color = pixelColor;
        }
    }

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

    //Controll Menu
    bool showMessageBox = false;

    //Rectangle Drawing 
    Vector2 RectStart = { -10,-10 };
    Vector2 RectEnd = { -10,-10 };
    bool DrawingRect = false;

        
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
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyReleased(KEY_L)) {
            LoadGridImage("pixel_grid_image.png",grid);
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
            Rectangle RectangleButton = { 600,screenHeight - 100,100,40 };
            Rectangle LoadButton = { 700,screenHeight - 50,100,40 };

            

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
            if (CheckCollisionPointRec(mousePosition, RectangleButton)) {
                Action = 4;
            }
            if (CheckCollisionPointRec(mousePosition, LoadButton)) {
                LoadGridImage("pixel_grid_image.png", grid);
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



        //RECTANGLE 
        if (Action == 4) {
            Vector2 mousePosition = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                RectStart.x = (int)(mousePosition.x / CELL_SIZE);
                RectStart.y = (int)(mousePosition.y / CELL_SIZE);
                DrawingRect = true;
            }

            if (DrawingRect && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                RectEnd.x = (int)(mousePosition.x / CELL_SIZE);
                RectEnd.y = (int)(mousePosition.y / CELL_SIZE);
            }

            if (DrawingRect && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                DrawingRect = false;
                int StartX = min((int)RectStart.x, (int)RectEnd.x);
                int StartY = min((int)RectStart.y, (int)RectEnd.y);
                int EndX = max((int)RectStart.x, (int)RectEnd.x);
                int EndY = max((int)RectStart.y, (int)RectEnd.y);

                for (int x = StartX; x <= EndX;x++) {
                    for (int y = StartY; y <= EndY;y++) {
                        grid[x][y].color = CurrentColor;
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

        //Tool Bar 
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
        Rectangle RectangleButton = { 600,screenHeight - 100,100,40 };
        Rectangle LoadButton = { 700,screenHeight - 50,100,40 };

        //buttons Colors
        DrawRectangleRec(drawButton, (Action == 1) ? DARKGRAY : GREEN);
        DrawRectangleRec(eraseButton, (Action == 2) ? DARKGRAY : GREEN); 
        DrawRectangleRec(SelectionButton, (Action == 3) ? DARKGRAY : GREEN);
        DrawRectangleRec(RectangleButton, (Action == 4) ? DARKGRAY : GREEN);
        DrawRectangleRec(SaveButton, BLUE);
        DrawRectangleRec(LoadButton, BLUE);

       
        //drawing the text 
        DrawText("Draw", drawButton.x + 20, drawButton.y + 10, 20, BLACK);
        DrawText("Erase", eraseButton.x + 20, eraseButton.y + 10, 20, BLACK);
        DrawText("Save", SaveButton.x + 20, SaveButton.y + 10, 20, BLACK);
        DrawText("Selection", SelectionButton.x + 5, SelectionButton.y + 10, 20, BLACK);
        DrawText("Rectangle", RectangleButton.x + 10, RectangleButton.y + 10, 15, BLACK);
        DrawText("Load", LoadButton.x + 20, LoadButton.y + 10, 20, BLACK);

        //Drawing Selecting
        if (Action == 3 && Selecting) {
            Vector2 MousePos = GetMousePosition();
            int selWidth = (int)(MousePos.x - SelectStart.x * CELL_SIZE);
            int selHeight = (int)(MousePos.y - SelectStart.y * CELL_SIZE);
            DrawRectangleLines(SelectStart.x * CELL_SIZE, SelectStart.y * CELL_SIZE, selWidth, selHeight, GREEN);
        }

        //Drawing Preview of Rectangle
        if (Action == 4 && DrawingRect) {
            Vector2 mousePosition = GetMousePosition();
            int StartX = min((int)RectStart.x, (int)RectEnd.x);
            int StartY = min((int)RectStart.y, (int)RectEnd.y);
            int EndX = max((int)RectStart.x, (int)RectEnd.x);
            int EndY = max((int)RectStart.y, (int)RectEnd.y);

            //Converting the preiw cord to grid ??
            int RectX = StartX * CELL_SIZE;
            int RectY = StartY * CELL_SIZE;
            int RectW = (EndX - StartX + 1) * CELL_SIZE;
            int RectH = (EndY - StartY + 1) * CELL_SIZE;

            DrawRectangle(RectX,RectY,RectW,RectH,ColorAlpha(CurrentColor,0.3));
        }

        //Controlls Message 
        if (GuiButton({ 24, 700, 120, 50 }, "#191#CONTROLLS")) showMessageBox = true;
        if (showMessageBox)
        {
            int result = GuiMessageBox({ 24, 550, 250, 150 },
                "#191#CONTROLLS","\n" "CTRL + S = SAVE\n" "CTRL + E = ERASE\n" "CTRL + P = DRAW\n", 
                "Nice;Cool");

            if (result >= 0) showMessageBox = false;
        }

        //UI 
        if (Action == 1) {
            DrawText("Action: DRAW", 400, screenHeight - 180, 20, WHITE);
        }
        if (Action == 2) {
            DrawText("Action: ERASE", 400, screenHeight - 180, 20, WHITE);
        }
        if (Action == 3) {
            DrawText("Action: SELECTION", 400, screenHeight - 180, 20, WHITE);
        }
        if (Action == 4) {
            DrawText("Action: RECTANGLE", 400, screenHeight - 180, 20, WHITE);
        }

        DrawRectangle(450, screenHeight - 50, 40, 40, CurrentColor);
        DrawText("Active Color", 425, screenHeight - 100, 20, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}