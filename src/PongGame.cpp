#include <raylib.h>
using namespace std;

Color TransRed = {230, 41, 55, 175};
Color TransWhite = {255, 255, 255, 50};
Color TransGreen = {0, 228, 48, 175};
Color TransBlue = {0, 121, 241, 100};

int screen_width = 1020, screen_height = 600;
int playerScore = 0, cpuScore = 0;
Texture2D textureStart, texturePlay;
bool sound_wall = false, musicStop = false;
bool button_clicked = false, exitPressed = false;
bool gamePause = false;
struct Button{
    Rectangle rec;
    Color color;
    Color text_color;
};

Button button_play, button_exit, button_music;

bool Is_Mouse_Over_Button (Button button){
    return CheckCollisionPointRec(GetMousePosition(), button.rec);
}

void Init_Button(Button *button, Rectangle rec, Color color, Color text_color){
    button -> rec = rec;
    button -> color = color;
    button -> text_color = text_color;
}

class Ball{
    public:
        float position_x = screen_width/2;
        float position_y = screen_height/2;
        float radius = 18;
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
                sound_wall = true;
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
        float speed = 7.2;
        
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
        
        Sound collision_paddle, gameOver, background, collision_walls;

        Game(){
            InitAudioDevice();
            collision_paddle = LoadSound("src/sounds/ball collision sound.wav");
            gameOver = LoadSound("src/sounds/Gameover.wav");
            collision_walls = LoadSound("src/sounds/ball with wall.wav");
            background = LoadSound("src/sounds/Background music.mp3");
        }
        ~Game(){
            UnloadSound(collision_paddle);
            UnloadSound(gameOver);
            UnloadSound(collision_walls);
            UnloadSound(background);
        }

        void draw(){
            ball.draw();
            player.draw();
            cpu.draw();
        }
        
        void update(){
            ball.update();
            if (sound_wall == true){
                if (!musicStop){
                    PlaySound(collision_walls);
                }
                sound_wall = false;
            }
            
            player.update();
            
            if (((ball.position_x < screen_width/3 + 50 || ball.position_x > screen_width/2) && ball.position_x < 2*screen_width/3) && ball.speed_x < 0){
                cpu.update(ball.position_y);
            }
        }
        
        void checkClick(){
            if (Is_Mouse_Over_Button(button_play) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                button_clicked = true;
                gamePause = false;
            }
            if (Is_Mouse_Over_Button(button_music) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                musicStop = !musicStop;
                ResumeSound(background);
            }
            if (Is_Mouse_Over_Button(button_exit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                exitPressed = true;
            }
        }

        void CheckCollision(){
            // Checking for collision of ball with the paddle_player
            if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{player.position_x, player.position_y, player.width, player.height})){
                ball.speed_x *= -1;
                if(!musicStop){
                    PlaySound(collision_paddle);
                }
            }
            // Checking for collision of ball with the paddle_cpu
            if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{cpu.position_x, cpu.position_y, cpu.width, cpu.height})){
               ball.speed_x *= -1;
                if(!musicStop){
                    PlaySound(collision_paddle);
                }            
            }
        }
        
        void GameOver(){
            if (cpuScore == 5){
                DrawText(TextFormat("G A M E O V E R"), screen_width/2 - 200, screen_height/2 - 25, 50, WHITE);
                PlaySound(gameOver);
                gamePause = true;
            }
        }
};

class Menu{
    public:
        void draw(){
            DrawRectangleRounded(button_play.rec, 0.8, 10, button_play.color);
            DrawText("PLAY", button_play.rec.x + button_play.rec.width / 2 - MeasureText("PLAY", 30)/2, button_play.rec.y+button_play.rec.height/2-30/2, 30, button_play.text_color);

            DrawRectangleRounded(button_music.rec, 0.8, 10, button_music.color);
            DrawText("MUSIC", button_music.rec.x + button_music.rec.width / 2 - MeasureText("MUSIC", 30)/2, button_music.rec.y+button_music.rec.height/2-30/2, 30, button_music.text_color);
        
            DrawRectangleRounded(button_exit.rec, 0.8, 10, button_exit.color);
            DrawText("EXIT", button_exit.rec.x + button_exit.rec.width / 2 - MeasureText("EXIT", 30)/2, button_exit.rec.y+button_exit.rec.height/2-30/2, 30, button_exit.text_color);
        }
};


void Create_Menu(){
    Init_Button(&button_play, Rectangle{419, 63, 185.7, 63.4}, BLUE, BLACK);
    Init_Button(&button_music, Rectangle{417, 186.4, 185.7, 63.4}, BLUE, BLACK);
    Init_Button(&button_exit, Rectangle{419, 309.9, 185.7, 63.4}, BLUE, BLACK);
    
    // Changing the color of the button when hovering
    if (Is_Mouse_Over_Button(button_play)){
        button_play.color = TransBlue;
        button_play.text_color = BLACK;
    }
    else{
        button_play.color = BLUE;
        button_play.text_color = WHITE;
    }

    if (musicStop){
        if (Is_Mouse_Over_Button(button_music)){
            button_music.color = TransRed;
            button_music.text_color = BLACK;
        }
        else{
            button_music.color = RED;
            button_music.text_color = WHITE;
        }
    }
    else if (Is_Mouse_Over_Button(button_music)){
        button_music.color = TransBlue;
        button_music.text_color = BLACK;
    }
    else{
        button_music.color = BLUE;
        button_music.text_color = WHITE;
    }

    if (Is_Mouse_Over_Button(button_exit)){
        button_exit.color = TransRed;
        button_exit.text_color = BLACK;
    }
    else{
        button_exit.color = BLUE;
        button_exit.text_color = WHITE;
    }    
    DrawRectangleRounded(Rectangle{364.6, 28.3, 287.6, 385.7}, 0.5, 6, TransWhite);
}


int main(){
    Game game = Game();
    bool start = true;
    Menu menu = Menu();

    InitWindow(screen_width, screen_height, "Pong Game");
    SetTargetFPS(60);

    textureStart = LoadTexture("src/image/GameScreenBG.png");
    texturePlay = LoadTexture("src/image/GamePlayingScreenBG.png");

    while (WindowShouldClose() == false && exitPressed == false){
        BeginDrawing();
        
        if (IsKeyPressed(KEY_SPACE) && !start){
            gamePause = !gamePause;
        }

        if (!IsSoundPlaying(game.background) && !musicStop){
            PlaySound(game.background);
        }
        
        if (start){
            DrawTexturePro(textureStart, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0,0}, 0, WHITE);
            Create_Menu();
            game.checkClick();
            menu.draw();
        }
        
        if (musicStop){
            DrawLine(417, 218.1, 600, 218.1, button_music.text_color);
            PauseSound(game.background);
        }

        if (button_clicked){
            start = false;

            ClearBackground(BLACK);
            DrawTexturePro(texturePlay, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0,0}, 0, WHITE);
            
            game.draw();

            if (gamePause){                
                Create_Menu();
                game.checkClick();
                menu.draw();
            }
                        
            if (!gamePause){
                DrawLine(screen_width/2, 0, screen_width/2, screen_height, WHITE);
                DrawCircle(screen_width/2, screen_height/2, 120, TransWhite);
                DrawText(TextFormat("%i", cpuScore), screen_width/4-20, 20, 60, WHITE);
                DrawText(TextFormat("%i", playerScore), 3*screen_width/4-20, 20, 60, WHITE);
                game.update();
                game.CheckCollision();
                game.GameOver();
            }
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

//2 Pause karke menu wapas laana without changing the bg [Use SPACE button]
//3 GameOVer and Winning 5 points only
//4 Ball check karna 