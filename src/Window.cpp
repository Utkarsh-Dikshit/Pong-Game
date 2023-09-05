#include <raylib.h>
using namespace std;

int screen_width = 1020, screen_height = 600;
int playerScore = 0, cpuScore = 0;

class Ball{
    public:
        int position_x = screen_width/2;
        int position_y = screen_height/2;
        int radius = 15;
        int speed_x = 7;
        int speed_y = 7;

        void draw(){
            DrawCircle(position_x, position_y, radius, WHITE);
        }

        void update(){
            position_x += speed_x;
            position_y += speed_y;
            
            if (position_x + radius >= screen_width){
                cpuScore ++;
                ballReset();
            }
            if (position_x - radius <= 0){
                playerScore ++;
                ballReset();
            }
            if (position_y + radius >= screen_height || position_y - radius <= 0){
                speed_y *= -1;
            }

        }
        
        void ballReset(){
            position_x = screen_width/2;
            position_y = screen_height/2; 
            int speed_choice [2] ={-1, 1};
            speed_x *= speed_choice[GetRandomValue(0,1)];
            speed_y *= speed_choice[GetRandomValue(0,1)];
        }

};

class Paddle{
    protected:
        void Movement_Limiter(){
            if (position_y <= 0){
                position_y = 0;
            }
            if (position_y >= screen_height - height){
                position_y = screen_height - height;
            }
        }
    public:
        int width = 20, height = 100;
        int position_x = screen_width - width - 10;
        int position_y = screen_height/2 - height/2;
        int speed = 5;

        void draw(){
            DrawRectangle(position_x, position_y, width, height, WHITE);
        }

        void update(){
            if (IsKeyDown(KEY_UP)){
                position_y -= speed;
            }
            if (IsKeyDown(KEY_DOWN)){
                position_y += speed;
            }
            Movement_Limiter();
        }

};

// Using inheritance for paddle moved by CPU, you can also create new class separately without using the player paddle
class cpu_Paddle{
    protected:
        void Movement_Limiter(){
            if (position_y <= 0){
                position_y = 0;
            }
            if (position_y >= screen_height - height){
                position_y = screen_height - height;
            }
        }
    public:
        int width = 20, height = 100;
        int position_x = 10;
        int position_y = screen_height/2 - height/2;
        int speed = 6;
        
        void draw(){
            DrawRectangle(position_x, position_y, width, height, WHITE);
        }

        void update(int ballPosition_y){
            if (position_y + height/2 > ballPosition_y){
                position_y -= speed;
            }
            if (position_y + height/2 < ballPosition_y){
                position_y += speed;
            }
            Movement_Limiter();
        }

};

int main (){
    Ball ball = Ball();
    Paddle player = Paddle();
    cpu_Paddle cpu = cpu_Paddle();

    InitWindow(screen_width, screen_height, "Pong Game");
    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawLine(screen_width/2, 0, screen_width/2, screen_height, WHITE);
        
        ball.draw();
        ball.update();

        player.draw();
        player.update();

        cpu.draw();
        cpu.update(ball.position_y);

        // Checking for collision of ball with the paddle_player
        if (CheckCollisionCircleRec(Vector2{float (ball.position_x), float (ball.position_y)}, ball.radius, Rectangle{float(player.position_x), float(player.position_y), float(player.width), float(player.height)})){
            ball.speed_x *= -1;
        }
        // Checking for collision of ball with the paddle_cpu
        if (CheckCollisionCircleRec(Vector2{float (ball.position_x), float (ball.position_y)}, ball.radius, Rectangle{float(cpu.position_x), float(cpu.position_y), float(cpu.width), float(cpu.height)})){
            ball.speed_x *= -1;
        }
        
        DrawText(TextFormat("%i", cpuScore), screen_width/4-20, 20, 80, WHITE);
        DrawText(TextFormat("%i", playerScore), 3*screen_width/4-20, 20, 80, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}