#include <raylib.h>
using namespace std;

Color lightRed = {240, 41, 74, 255};
Color lightWhite = {255, 255, 255, 150};
Color darkRed = {250, 41, 74, 255};

int screen_width = 1020, screen_height = 600;
int playerScore = 0, cpuScore = 0;
bool sound = false;

class Ball{
    public:
        float position_x = screen_width/2;
        float position_y = screen_height/2;
        float radius = 15;
        int speed_x = 7;
        int speed_y = 7;
        
        void draw(){
            DrawCircle(position_x, position_y, radius, YELLOW);
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
                sound = true;
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
        float width = 20, height = 100;
        float position_x = screen_width - width - 7;
        float position_y = screen_height/2 - height/2;
        int speed = 5;

        void draw(){
            Rectangle rec = Rectangle {position_x, position_y, width, height};
            DrawRectangleRounded(rec, 0.8, 7, WHITE);
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
        float width = 20, height = 100;
        float position_x = 7;
        float position_y = screen_height/2 - height/2;
        int speed = 7;
        
        void draw(){
            Rectangle rec = Rectangle {position_x, position_y, width, height};
            DrawRectangleRounded(rec, 0.8, 7, WHITE);
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

class Game{
    public:
        Ball ball = Ball();
        Paddle player = Paddle();
        cpu_Paddle cpu = cpu_Paddle();
        bool gamerun = true;
        
        Sound collision_paddle, gameOver, background, collision_walls;

        Game(){
            InitAudioDevice();
            collision_paddle = LoadSound("src/sounds/ball collision sound.wav");
            gameOver = LoadSound("src/sounds/Gameover.wav");
            collision_walls = LoadSound("src/sounds/ball with wall.wav");
        }
        ~Game(){
            UnloadSound(collision_paddle);
            UnloadSound(gameOver);
            UnloadSound(collision_walls);
        }

        void draw(){
            ball.draw();
            player.draw();
            cpu.draw();
        }
        
        void update(){
            ball.update();
            if (sound == true){
                PlaySound(collision_walls);
                sound = false;
            }

            player.update();

            if (((ball.position_x < screen_width/3 + 50 || ball.position_x > screen_width/2) && ball.position_x < 2*screen_width/3) && ball.speed_x < 0){
                cpu.update(ball.position_y);
            }
        }

        void CheckCollision(){
            // Checking for collision of ball with the paddle_player
            if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{player.position_x, player.position_y, player.width, player.height})){
                ball.speed_x *= -1;
                PlaySound(collision_paddle);
            }
            // Checking for collision of ball with the paddle_cpu
            if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{cpu.position_x, cpu.position_y, cpu.width, cpu.height})){
               ball.speed_x *= -1;
               PlaySound(collision_paddle);
            }
        }
        
        void GameOver(){
            if (cpuScore == 5){
                DrawText(TextFormat("G A M E O V E R"), screen_width/2 - 300, screen_height/2 - 25, 50, WHITE);
                PlaySound(gameOver);
            }
        }
};

int main (){
    Game game = Game();

    InitWindow(screen_width, screen_height, "Pong Game");
    SetTargetFPS(60);

    while (WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(lightRed);

        DrawRectangle(0, 0, screen_width/2, screen_height, RED);  
        DrawLine(screen_width/2, 0, screen_width/2, screen_height, WHITE);
        DrawCircle(screen_width/2, screen_height/2, 120, lightWhite);

        game.draw();
        game.update();
        game.CheckCollision();
        game.GameOver();

        DrawText(TextFormat("%i", cpuScore), screen_width/4-20, 20, 60, WHITE);
        DrawText(TextFormat("%i", playerScore), 3*screen_width/4-20, 20, 60, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}