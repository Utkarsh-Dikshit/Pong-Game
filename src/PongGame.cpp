#include <raylib.h>
using namespace std;

// Trans --> Transparent/Translucent
Color TransRed = {230, 41, 55, 175};
Color TransWhite = {255, 255, 255, 50};
Color TransWhite2 = {255, 255, 255, 150};
Color TransBlack = {0, 0, 0, 150};
Color TransGreen = {0, 228, 48, 175};
Color TransBlue = {0, 121, 241, 100};

int screen_width = 1020, screen_height = 600;
int playerScore = 0, cpuScore = 0;

Texture2D textureStart, texturePlay;

bool start = false;
bool sound_wall = false, musicStop = false;
bool button_clicked_play = false, exitPressed = false;
bool gamePause = false;

const char *SubMenu_text;
const char *first_button_text;

struct Button
{
    Rectangle rec;
    Color color;
    Color text_color;
};

Button button_play, button_exit, button_music_main, button_music_sub, button_MainMenu, button_play_sub;

void Init_Button(Button *button, Rectangle rec, Color color, Color text_color)
{
    button->rec = rec;
    button->color = color;
    button->text_color = text_color;
}

bool Is_Mouse_Over_Button(Button button)
{
    return CheckCollisionPointRec(GetMousePosition(), button.rec);
}

class Player_Paddle
{
protected:
    void Movement_Limiter()
    {
        if (position_y <= 0)
        {
            position_y = 0;
        }
        if (position_y >= screen_height - height)
        {
            position_y = screen_height - height;
        }
    }

public:
    float width = 20, height = 100;
    float position_x = screen_width - width - 7;
    float position_y = screen_height / 2 - height / 2;
    int speed = 5.3;

    void draw()
    {
        Rectangle rec = Rectangle{position_x, position_y, width, height};
        DrawRectangleRounded(rec, 0.8, 7, WHITE);
    }

    void update()
    {
        if (IsKeyDown(KEY_UP))
        {
            position_y -= speed;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            position_y += speed;
        }
        Movement_Limiter();
    }
};

class cpu_Paddle
{
protected:
    void Movement_Limiter()
    {
        if (position_y <= 0)
        {
            position_y = 0;
        }
        if (position_y >= screen_height - height)
        {
            position_y = screen_height - height;
        }
    }

public:
    float width = 20, height = 100;
    float position_x = 7;
    float position_y = screen_height / 2 - height / 2;
    float speed = 7.2;

    void draw()
    {
        Rectangle rec = Rectangle{position_x, position_y, width, height};
        DrawRectangleRounded(rec, 0.8, 7, WHITE);
    }

    void update(int ballPosition_y)
    {
        if (position_y + height / 2 > ballPosition_y)
        {
            position_y -= speed;
        }
        if (position_y + height / 2 < ballPosition_y)
        {
            position_y += speed;
        }
        Movement_Limiter();
    }
};

class Ball
{
public:
    float position_x = screen_width / 2;
    float position_y = screen_height / 2;
    float radius = 18;
    float speed_x = 7;
    float speed_y = 7;

    void draw()
    {
        DrawCircle(position_x, position_y, radius, YELLOW);
    }

    void update()
    {
        position_x += speed_x;
        position_y += speed_y;

        if (position_y + radius >= screen_height || position_y - radius <= 0)
        {
            speed_y *= -1;
            sound_wall = true;
        }
    }

    void ballReset()
    {
        position_x = screen_width / 2;
        position_y = screen_height / 2;
        speed_x = 7;
        speed_y = 7;
        int speed_choice[2] = {-1, 1};
        speed_x *= speed_choice[GetRandomValue(0, 1)];
        speed_y *= speed_choice[GetRandomValue(0, 1)];
    }
};

class Game
{
public:
    Ball ball = Ball();
    Player_Paddle player = Player_Paddle();
    cpu_Paddle cpu = cpu_Paddle();
    float width = 20, height = 100;

    Sound collision_paddle, gameOver, WinSound, background_outer, background_inner, collision_walls, PointEarned_Player, PointEarned_cpu;

    Game()
    {
        InitAudioDevice();
        collision_paddle = LoadSound("src/sounds/ball collision sound.wav");
        PointEarned_Player = LoadSound("src/sounds/Achievement.wav");
        PointEarned_cpu = LoadSound("src/sounds/loose point sound.mp3");
        gameOver = LoadSound("src/sounds/Gameover.wav");
        WinSound = LoadSound("src/sounds/WinningSound.wav");
        collision_walls = LoadSound("src/sounds/ball with wall.wav");
        background_outer = LoadSound("src/sounds/Background music1.mp3");
        background_inner = LoadSound("src/sounds/Background music2.mp3");
    }
    ~Game()
    {
        UnloadSound(collision_paddle);
        UnloadSound(PointEarned_Player);
        UnloadSound(PointEarned_cpu);
        UnloadSound(gameOver);
        UnloadSound(WinSound);
        UnloadSound(collision_walls);
        UnloadSound(background_outer);
        UnloadSound(background_inner);
    }


    void draw()
    {
        ball.draw();
        player.draw();
        cpu.draw();
    }

    void update()
    {
        ball.update();
        
        if (ball.position_x - ball.radius > screen_width)
        {
            if (!musicStop)
            {
                PlaySound(PointEarned_cpu);
            }
            cpuScore++;
            ball.ballReset();
            cpu.speed = 7.2;
        }
        
        if (ball.position_x + ball.radius < 0)
        {
            if (!musicStop)
            {
                PlaySound(PointEarned_Player);
            }
            playerScore++;
            ball.ballReset();
            cpu.speed = 7.2;
        }
        
        if (sound_wall == true)
        {
            if (!musicStop)
            {
                PlaySound(collision_walls);
            }
            sound_wall = false;
        }

        player.update();

        if (((ball.position_x < screen_width / 3 + 50 || ball.position_x > screen_width / 2) && ball.position_x < 2 * screen_width / 3) && ball.speed_x < 0)
        {
            cpu.update(ball.position_y);
        }
    }

    void checkClick()
    {
        if (Is_Mouse_Over_Button(button_play) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !start)
        {
            button_clicked_play = true;
            gamePause = false;
            reset();
            if (!musicStop)
            {
                PlaySound(background_inner);
            }
        }
        if (Is_Mouse_Over_Button(button_music_main) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !start)
        {
            musicStop = !musicStop;
            ResumeSound(background_outer);
        }
        if (Is_Mouse_Over_Button(button_exit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !start)
        {
            exitPressed = true;
        }
        if (Is_Mouse_Over_Button(button_play_sub) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && start)
        {
            gamePause = false;
        }
        if (Is_Mouse_Over_Button(button_music_sub) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && start)
        {
            musicStop = !musicStop;
            ResumeSound(background_outer);
        }
        if (Is_Mouse_Over_Button(button_MainMenu) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && start)
        {
            start = false;
            button_clicked_play = false;           
        }
    }

    void CheckCollision()
    {
        // Checking for collision of ball with the paddle_player (also making some changes to remove multiple collision within same rectangle)
        if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{player.position_x, player.position_y, 0, player.height}))
        {
            ball.speed_x *= -1;
            if (!musicStop)
            {
                PlaySound(collision_paddle);
            }
        }
        // Checking for collision of ball with the paddle_cpu (also making some changes to remove multiple collision within same rectangle)
        if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{cpu.position_x + cpu.width, cpu.position_y, 0, cpu.height}))
        {
            ball.speed_x *= -1;
            if (ball.speed_x > 0)
            {
                ball.speed_x += 0.8;
                if (ball.speed_y > 0)
                {
                    ball.speed_y += 0.8;
                }
                else
                {
                    ball.speed_y -= 0.8;
                }
            }
            cpu.speed += 0.3;
            if (!musicStop)
            {
                PlaySound(collision_paddle);
            }
        }
    }

    void reset()
    {
        ball.ballReset();
        cpu.speed = 7.2;
        cpuScore = 0;
        playerScore = 0;
        player.position_x = screen_width - width - 7;
        player.position_y = screen_height / 2 - height / 2;
        cpu.position_x = 7;
        cpu.position_y = screen_height / 2 - height / 2;
    }

    void GameEnd()
    {
        if (cpuScore == 5)
        {
            PlaySound(gameOver);
            reset();
            SubMenu_text = "Y O U  L O S E :(";
            gamePause = true;   
            first_button_text = "RETRY";
            PlaySound(background_outer);
        }
        if (playerScore == 5)
        {
            PlaySound(WinSound);
            reset();
            SubMenu_text = "Y O U  W I N :)";
            gamePause = true;
            first_button_text = "RESTART";
            PlaySound(background_outer);
        }
    }
};

class Menu
{
public:
    void Create_MAIN()
    {
        Init_Button(&button_play, Rectangle{419, 64, 184.5, 63.4}, BLUE, BLACK);
        Init_Button(&button_music_main, Rectangle{419, 187.1, 184.5, 63.4}, BLUE, BLACK);
        Init_Button(&button_exit, Rectangle{419, 310.9, 184.5, 63.4}, BLUE, BLACK);

        // Changing the color of the button when hovering
        if (Is_Mouse_Over_Button(button_play))
        {
            button_play.color = TransBlue;
            button_play.text_color = BLACK;
        }
        else
        {
            button_play.color = BLUE;
            button_play.text_color = WHITE;
        }

        if (musicStop)
        {
            if (Is_Mouse_Over_Button(button_music_main))
            {
                button_music_main.color = TransRed;
                button_music_main.text_color = BLACK;
            }
            else
            {
                button_music_main.color = RED;
                button_music_main.text_color = WHITE;
            }
        }
        else if (Is_Mouse_Over_Button(button_music_main))
        {
            button_music_main.color = TransBlue;
            button_music_main.text_color = BLACK;
        }
        else
        {
            button_music_main.color = BLUE;
            button_music_main.text_color = WHITE;
        }

        if (Is_Mouse_Over_Button(button_exit))
        {
            button_exit.color = TransRed;
            button_exit.text_color = BLACK;
        }
        else
        {
            button_exit.color = BLUE;
            button_exit.text_color = WHITE;
        }
    }

    void Create_SUBMAIN()
    {
        Init_Button(&button_play_sub, Rectangle{419, 64 + 78.85, 184.5, 63.4}, BLUE, BLACK);
        Init_Button(&button_music_sub, Rectangle{419, 187.1 + 78.85, 184.5, 63.4}, BLUE, BLACK);
        Init_Button(&button_MainMenu, Rectangle{419, 310.9 + 78.85, 184.5, 63.4}, BLUE, BLACK);

        // Changing the color of the button when hovering
        if (Is_Mouse_Over_Button(button_play_sub))
        {
            button_play_sub.color = TransGreen;
            button_play_sub.text_color = BLACK;
        }
        else
        {
            button_play_sub.color = BLUE;
            button_play_sub.text_color = WHITE;
        }

        if (musicStop)
        {
            if (Is_Mouse_Over_Button(button_music_sub))
            {
                button_music_sub.color = TransRed;
                button_music_sub.text_color = BLACK;
            }
            else
            {
                button_music_sub.color = RED;
                button_music_sub.text_color = WHITE;
            }
        }
        else if (Is_Mouse_Over_Button(button_music_sub))
        {
            button_music_sub.color = TransBlue;
            button_music_sub.text_color = BLACK;
        }
        else
        {
            button_music_sub.color = BLUE;
            button_music_sub.text_color = WHITE;
        }

        if (Is_Mouse_Over_Button(button_MainMenu))
        {
            button_MainMenu.color = TransRed;
            button_MainMenu.text_color = BLACK;
        }
        else
        {
            button_MainMenu.color = BLUE;
            button_MainMenu.text_color = WHITE;
        }
    }

    void draw_main()
    {
        DrawRectangleRounded(button_play.rec, 0.8, 10, button_play.color);
        DrawText("PLAY", button_play.rec.x + button_play.rec.width / 2 - MeasureText("PLAY", 30) / 2, button_play.rec.y + button_play.rec.height / 2 - 30 / 2, 30, button_play.text_color);

        DrawRectangleRounded(button_music_main.rec, 0.8, 10, button_music_main.color);
        DrawText("MUSIC", button_music_main.rec.x + button_music_main.rec.width / 2 - MeasureText("MUSIC", 30) / 2, button_music_main.rec.y + button_music_main.rec.height / 2 - 30 / 2, 30, button_music_main.text_color);

        DrawRectangleRounded(button_exit.rec, 0.8, 10, button_exit.color);
        DrawText("EXIT", button_exit.rec.x + button_exit.rec.width / 2 - MeasureText("EXIT", 30) / 2, button_exit.rec.y + button_exit.rec.height / 2 - 30 / 2, 30, button_exit.text_color);
    }

    void draw_submain()
    {
        DrawText(SubMenu_text, screen_width / 2 - MeasureText(SubMenu_text, 50) / 2, screen_height / 10, 50, WHITE);

        DrawRectangleRounded(Rectangle{364.6, float(screen_height - 385.7) / 2, 287.6, 385.7}, 0.5, 6, TransBlack);

        DrawRectangleRounded(button_play_sub.rec, 0.8, 10, button_play_sub.color);
        DrawText(first_button_text, button_play_sub.rec.x + button_play_sub.rec.width / 2 - MeasureText(first_button_text, 25) / 2, button_play_sub.rec.y + button_play_sub.rec.height / 2 - 25 / 2, 25, button_play_sub.text_color);

        DrawRectangleRounded(button_music_sub.rec, 0.8, 10, button_music_sub.color);
        DrawText("MUSIC", button_music_sub.rec.x + button_music_sub.rec.width / 2 - MeasureText("MUSIC", 25) / 2, button_music_sub.rec.y + button_music_sub.rec.height / 2 - 25 / 2, 25, button_music_sub.text_color);

        DrawRectangleRounded(button_MainMenu.rec, 0.8, 10, button_MainMenu.color);
        DrawText("MAIN MENU", button_MainMenu.rec.x + button_MainMenu.rec.width / 2 - MeasureText("MAIN MENU", 25) / 2, button_MainMenu.rec.y + button_MainMenu.rec.height / 2 - 25 / 2, 25, button_MainMenu.text_color);
    }
};

int main()
{
    Game game = Game();
    Menu menu = Menu();

    InitWindow(screen_width, screen_height, "Pong Game");
    SetTargetFPS(60);

    textureStart = LoadTexture("src/image/GameScreenBG.png");
    texturePlay = LoadTexture("src/image/GamePlayingScreenBG.png");
    
    while (WindowShouldClose() == false && exitPressed == false)
    {
        BeginDrawing();

        // It is to pause the game and to open a menu but game should not be in starting phase (Main Menu)
        if (IsKeyPressed(KEY_SPACE) && start)
        {
            gamePause = !gamePause;
            first_button_text = "RESUME";
            SubMenu_text = "GAME PAUSED";
        }

        // It is to play the background music but music should not be stopped
        if (!IsSoundPlaying(game.background_outer) && !musicStop && !IsSoundPlaying(game.background_inner))
        {
            PlaySound(game.background_outer);
        }

        // For MAIN MENU, when the game starts
        if (!start)
        {
            // Screen and Picture is of the same dimensions thatswhy 'source' and 'destination' is same here and also we need full picture in our gamescreen
            DrawTexturePro(textureStart, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0, 0}, 0, WHITE);
            DrawRectangleRounded(Rectangle{364.6, 28.3, 287.6, 385.7}, 0.5, 6, TransWhite);
            menu.Create_MAIN();
            menu.draw_main();
            game.checkClick();
            
            // If music is stopped by the player after clicking the button, then Pause the music and change the background color of the music button
            if (musicStop)
            {
                DrawRectangle(419.8, 215.1, 184, 3, button_music_main.text_color);
                PauseSound(game.background_outer);
            }
        }

        if (button_clicked_play)
        {
            start = true;
            ClearBackground(BLACK);

            // Screen and Picture is of the same dimensions thatswhy 'source' and 'destination' is same here and also we need full picture in our gamescreen
            DrawTexturePro(texturePlay, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0, 0}, 0, WHITE);

            game.draw();


            if (!gamePause)
            {   
                PauseSound(game.background_outer);
                
                if (!IsSoundPlaying(game.background_inner) && !musicStop)
                {   
                    PlaySound(game.background_inner);
                }
                
                SetSoundVolume(game.background_inner, 0.1);
                
                DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
                DrawCircle(screen_width / 2, screen_height / 2, 120, TransWhite);
                DrawText(TextFormat("%i", cpuScore), screen_width / 4 - 20, 20, 60, WHITE);
                DrawText(TextFormat("%i", playerScore), 3 * screen_width / 4 - 20, 20, 60, WHITE);
                game.update();
                game.CheckCollision(); 
                game.GameEnd();
            }

            if (gamePause)
            {   
                menu.Create_SUBMAIN();
                menu.draw_submain();

                PauseSound(game.background_inner);
                
                if (!musicStop)
                {
                    ResumeSound(game.background_outer);
                }

                if (musicStop)
                {   
                    PauseSound(game.background_outer);
                    DrawRectangle(419.8, 216.1 + 78.85, 184, 3, button_music_sub.text_color);
                }
                game.checkClick();
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}