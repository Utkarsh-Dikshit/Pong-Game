#include <raylib.h>
using namespace std;

int screen_width = 1020, screen_height = 600;

class Ball{
    public:
        int position_x = screen_width/2;
        int position_y = screen_height/2;

        void draw(){
            DrawCircle(position_x, position_y, 20, WHITE);
        }

        void update(){
            position_x += 7;
            position_y += 7;
        }

};

class Paddles{
    public:
        void draw(){
            DrawRectangle(10, screen_height/2 - 40, 20, 80, WHITE);
            DrawRectangle(screen_width - 30, screen_height/2 - 40, 20, 80, WHITE);
        }
};

int main (){
    Ball ball = Ball();
    Paddles paddles = Paddles();

    InitWindow(1020, 600, "Pong Game");
    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(BLACK);

        ball.draw();
        ball.update();
        paddles.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}