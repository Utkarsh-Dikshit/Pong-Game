#include <raylib.h>
#include <iostream>
using namespace std;

Color green = {173, 204, 96, 255}; // {red, green, blue, transparency} // 255 is the maximum intensity and no transparency
Color darkgreen = {43, 51, 24, 255}; // {red, green, blue, transparency} // 255 is the maximum intensity and no transparency


int main (){

    cout << "Starting the game..." << endl;

    InitWindow(800,600, "GIVE A NAME");
    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        BeginDrawing();
        
        ClearBackground(WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}