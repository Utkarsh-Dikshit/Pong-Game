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
int Player2_Score = 0, Player1_Score = 0;

Texture2D textureStart, texturePlay;

bool start = false, multiplayer = false;
bool sound_wall = false, musicStop = false;
bool button_clicked_play = false, exitPressed = false;
bool gamePause = false, gameEnd = true;
bool mouse_over_button = false;

const char *SubMain_text;
const char *first_button_text;

struct Button
{
    Rectangle rec;
    Color color;
    Color text_color;
};

Button button_play, button_singlePlayer, button_twoPlayer, button_back, button_exit, button_music_main, button_play_sub, button_music_sub, button_MainMenu;

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

class Right_Paddle
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
    int speed = 5.5;

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

class Left_Paddle
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
    float speed;

    void getSpeed()
    {
        if (multiplayer)
        {
            speed = 5.5;
        }
        else
        {
            speed = 7.2;
        }
    }

    void draw()
    {
        Rectangle rec = Rectangle{position_x, position_y, width, height};
        DrawRectangleRounded(rec, 0.8, 7, WHITE);
    }

    void update(int ballPosition_y, bool multiplayer)
    {
        // If not multiplayer then player 1 (Left)  will acts as a CPU player
        if (multiplayer)
        {
            if (IsKeyDown(KEY_W))
            {
                position_y -= speed;
            }
            if (IsKeyDown(KEY_S))
            {
                position_y += speed;
            }
        }
        else
        {
            if (position_y + height / 2 > ballPosition_y)
            {
                position_y -= speed;
            }
            if (position_y + height / 2 < ballPosition_y)
            {
                position_y += speed;
            }
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
    Left_Paddle player1 = Left_Paddle();
    Right_Paddle player2 = Right_Paddle();

    Sound buttonHover, buttonClick, collision_paddle, gameOver, WinSound, background_outer, background_inner, collision_walls, PointEarned, PointLoose;

    Game()
    {
        InitAudioDevice();
        buttonHover = LoadSound("src/sounds/Button Hover.mp3");
        buttonClick = LoadSound("src/sounds/Button Click.mp3");
        collision_paddle = LoadSound("src/sounds/ball collision sound.wav");
        PointEarned = LoadSound("src/sounds/Achievement.wav");
        PointLoose = LoadSound("src/sounds/loose point sound.mp3");
        gameOver = LoadSound("src/sounds/Gameover.wav");
        WinSound = LoadSound("src/sounds/WinningSound.wav");
        collision_walls = LoadSound("src/sounds/ball with wall.wav");
        background_outer = LoadSound("src/sounds/Background music1.mp3");
        background_inner = LoadSound("src/sounds/Background music2.mp3");
    }
    ~Game()
    {   
        CloseAudioDevice();
        UnloadSound(buttonHover);
        UnloadSound(buttonClick);
        UnloadSound(collision_paddle);
        UnloadSound(PointEarned);
        UnloadSound(PointLoose);
        UnloadSound(gameOver);
        UnloadSound(WinSound);
        UnloadSound(collision_walls);
        UnloadSound(background_outer);
        UnloadSound(background_inner);
    }

    void draw()
    {
        ball.draw();
        player1.draw();
        player2.draw();
    }

    void update()
    {
        ball.update();

        player2.update();

        if (((ball.position_x < screen_width / 3 + 50 || ball.position_x > screen_width / 2) && ball.position_x < 2 * screen_width / 3) && ball.speed_x < 0 && !multiplayer)
        {
            player1.update(ball.position_y, multiplayer);
        }
        if (multiplayer)
        {
            player1.update(ball.position_y, multiplayer);
        }

        if (ball.position_x - ball.radius > screen_width)
        {
            if (!musicStop)
            {
                if (multiplayer)
                    PlaySound(PointEarned);
                else
                    PlaySound(PointLoose);
            }

            Player1_Score++;
            ball.ballReset();
            player1.getSpeed();
            player2.speed = 5.5;
        }

        if (ball.position_x + ball.radius < 0)
        {
            if (!musicStop)
            {
                PlaySound(PointEarned);
            }

            Player2_Score++;
            ball.ballReset();
            player1.getSpeed();
            player2.speed = 5.5;
        }

        if (sound_wall == true && !musicStop)
        {
            PlaySound(collision_walls);
            sound_wall = false;
        }
    }

    void checkClick_Main()
    {
        if (Is_Mouse_Over_Button(button_play) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            button_clicked_play = true;
        }
        if (Is_Mouse_Over_Button(button_music_main) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            musicStop = !musicStop;
            ResumeSound(background_outer);
        }
        if (Is_Mouse_Over_Button(button_exit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            exitPressed = true;
        }
    }

    void CheckClick_SubMenu()
    {
        if (Is_Mouse_Over_Button(button_singlePlayer) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            gameEnd = true;
            gamePause = false;
            multiplayer = false;
            start = true;
            reset();
        }
        if (Is_Mouse_Over_Button(button_twoPlayer) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            gameEnd = true;
            gamePause = false;
            multiplayer = true;
            start = true;
            reset();
        }
        if (Is_Mouse_Over_Button(button_back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            button_clicked_play = false;
        }
    }

    void CheckClick_SubMain()
    {
        if (Is_Mouse_Over_Button(button_play_sub) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            gamePause = false;
        }
        if (Is_Mouse_Over_Button(button_music_sub) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            musicStop = !musicStop;
            ResumeSound(background_outer);
        }
        if (Is_Mouse_Over_Button(button_MainMenu) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {   
            if (!musicStop)
            {
                PlaySound(buttonClick);
            }
            start = false;
            button_clicked_play = false;
        }
    }

    void CheckCollision()
    {
        // Checking for collision of ball with the Right paddle
        if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{player2.position_x, player2.position_y, player2.width, player2.height}))
        {   
            // Limiting Case to avoid multiple collision within the paddle
            ball.position_x = player2.position_x - ball.radius; 
            
            ball.speed_x *= -1;

            if (ball.speed_x < 0)
            {
                if (multiplayer)
                {
                    ball.speed_x -= 0.4;
                    if (ball.speed_y > 0)
                    {
                        ball.speed_y += 0.4;
                    }
                    else
                    {
                        ball.speed_y -= 0.4;
                    }
                }
            }
            
            player2.speed += 0.3;

            if (!musicStop)
            {
                PlaySound(collision_paddle);
            }
        }
        // Checking for collision of ball with the Left Paddle
        if (CheckCollisionCircleRec(Vector2{ball.position_x, ball.position_y}, ball.radius, Rectangle{player1.position_x, player1.position_y, player1.width, player1.height}))
        {
            // Limiting Case to avoid multiple collision within the paddle
            ball.position_x = player1.position_x + player1.width + ball.radius;

            ball.speed_x *= -1;

            if (ball.speed_x > 0)
            {
                if (multiplayer)
                {
                    ball.speed_x += 0.4;
                    if (ball.speed_y > 0)
                    {
                        ball.speed_y += 0.4;
                    }
                    else
                    {
                        ball.speed_y -= 0.4;
                    }
                }
                else
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
            }

            if (multiplayer)
            {
                player1.speed += 0.3;
            }
            else
            {
                player1.speed += 0.4;
            }

            if (!musicStop)
            {
                PlaySound(collision_paddle);
            }
        }
    }

    void reset()
    {
        ball.ballReset();
        player1.getSpeed();
        player2.speed = 5.5;
        Player1_Score = 0;
        Player2_Score = 0;
        player2.position_x = screen_width - player2.width - 7;
        player2.position_y = screen_height / 2 - player2.height / 2;
        player1.position_x = 7;
        player1.position_y = screen_height / 2 - player1.height / 2;
    }

    void GameEnd()
    {
        if (Player1_Score == 5)
        {
            reset();
            gamePause = true;
            gameEnd = true;

            if (multiplayer)
            {
                SubMain_text = "P L A Y E R - 1  W I N S !";
                first_button_text = "RESTART";
            }
            else
            {
                SubMain_text = "Y O U  L O S E :(";
                first_button_text = "RETRY";
            }

            if (!musicStop)
            {
                if (multiplayer)
                    PlaySound(WinSound);
                else
                    PlaySound(gameOver);
            }
            // Sound will not play even if it is written outside in case of music is stopped.
            // It is written outside to play that music from beginning after the game ended.
            PlaySound(background_outer);
        }

        if (Player2_Score == 5)
        {
            reset();
            gamePause = true;
            gameEnd = true;

            if (multiplayer)
            {
                SubMain_text = "P L A Y E R - 2  W I N S !";
                first_button_text = "RESTART";
            }
            else
            {
                SubMain_text = "Y O U  W I N :)";
                first_button_text = "RESTART";
            }

            if (!musicStop)
            {
                PlaySound(WinSound);
            }
            // Sound will not play even if it is written outside in case of music is stopped.
            // It is written outside to play that music from beginning after the game ended.
            PlaySound(background_outer);
        }
    }
};

class Menu
{
public:
    Game game = Game();

    void Create_MAIN()
    {
        Init_Button(&button_play, Rectangle{419, 64, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_music_main, Rectangle{419, 187.1, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_exit, Rectangle{419, 310.9, 184.5, 63.4}, BLUE, WHITE);

        if (!Is_Mouse_Over_Button(button_play) && !Is_Mouse_Over_Button(button_music_main) && !Is_Mouse_Over_Button(button_exit))
        {
            mouse_over_button = true;
        }

        // Changing the color of the button when hovering
        if (Is_Mouse_Over_Button(button_play))
        {   
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
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
            if (mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }

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
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
            button_exit.color = TransRed;
            button_exit.text_color = BLACK;
        }
        else
        {
            button_exit.color = BLUE;
            button_exit.text_color = WHITE;
        }

    }

    void Create_SubMenu()
    {
        Init_Button(&button_singlePlayer, Rectangle{419, 64, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_twoPlayer, Rectangle{419, 187.1, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_back, Rectangle{419, 310.9, 184.5, 63.4}, BLUE, WHITE);

        if (!Is_Mouse_Over_Button(button_singlePlayer) && !Is_Mouse_Over_Button(button_twoPlayer) && !Is_Mouse_Over_Button(button_back))
        {
            mouse_over_button = true;
        }

        // Changing the color of the button when hovering
        if (Is_Mouse_Over_Button(button_singlePlayer))
        {   
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
            button_singlePlayer.color = TransBlue;
            button_singlePlayer.text_color = BLACK;
        }
        else
        {
            button_singlePlayer.color = BLUE;
            button_singlePlayer.text_color = WHITE;
        }

        if (Is_Mouse_Over_Button(button_twoPlayer))
        {   
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
            button_twoPlayer.color = TransBlue;
            button_twoPlayer.text_color = BLACK;
        }
        else
        {
            button_twoPlayer.color = BLUE;
            button_twoPlayer.text_color = WHITE;
        }

        if (Is_Mouse_Over_Button(button_back))
        {   
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
            button_back.color = TransRed;
            button_back.text_color = BLACK;
        }
        else
        {
            button_back.color = BLUE;
            button_back.text_color = WHITE;
        }
    }

    void Create_SUBMAIN()
    {
        Init_Button(&button_play_sub, Rectangle{419, 64 + 78.85, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_music_sub, Rectangle{419, 187.1 + 78.85, 184.5, 63.4}, BLUE, WHITE);
        Init_Button(&button_MainMenu, Rectangle{419, 310.9 + 78.85, 184.5, 63.4}, BLUE, WHITE);

        if (!Is_Mouse_Over_Button(button_play_sub) && !Is_Mouse_Over_Button(button_music_sub) && !Is_Mouse_Over_Button(button_MainMenu))
        {
            mouse_over_button = true;
        }

        // Changing the color of the button when hovering
        if (Is_Mouse_Over_Button(button_play_sub))
        {   
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
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
            if (mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }

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
            if (!musicStop && mouse_over_button)
            {
                PlaySound(game.buttonHover);
                mouse_over_button = false;
            }
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

    void draw_submenu()
    {
        DrawRectangleRounded(button_singlePlayer.rec, 0.8, 10, button_singlePlayer.color);
        DrawText("One Player", button_singlePlayer.rec.x + button_singlePlayer.rec.width / 2 - MeasureText("One Player", 30) / 2, button_singlePlayer.rec.y + button_singlePlayer.rec.height / 2 - 30 / 2, 30, button_singlePlayer.text_color);

        DrawRectangleRounded(button_twoPlayer.rec, 0.8, 10, button_twoPlayer.color);
        DrawText("Two Player", button_twoPlayer.rec.x + button_twoPlayer.rec.width / 2 - MeasureText("Two Player", 30) / 2, button_twoPlayer.rec.y + button_twoPlayer.rec.height / 2 - 30 / 2, 30, button_twoPlayer.text_color);

        DrawRectangleRounded(button_back.rec, 0.8, 10, button_back.color);
        DrawText("Back", button_back.rec.x + button_back.rec.width / 2 - MeasureText("Back", 30) / 2, button_back.rec.y + button_back.rec.height / 2 - 30 / 2, 30, button_back.text_color);
    }

    void draw_submain()
    {
        DrawText(SubMain_text, screen_width / 2 - MeasureText(SubMain_text, 50) / 2, screen_height / 10, 50, WHITE);

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
    Menu menu = Menu();

    bool Is_Previously_GamePaused = false;

    InitWindow (screen_width, screen_height, "PongVerse");

    Image image = LoadImage ("src/icon/Icon.png");
    SetWindowIcon(image);

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
            SubMain_text = "GAME PAUSED";
        }

        if (!IsSoundPlaying(menu.game.background_outer) && !musicStop && !IsSoundPlaying(menu.game.background_inner))
        {
            PlaySound(menu.game.background_outer);
        }

        // For MAIN MENU, when the game starts
        if (!start)
        {
            // Screen and Picture is of the same dimensions thatswhy 'source' and 'destination' is same here and also we need full picture in our gamescreen
            DrawTexturePro(textureStart, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0, 0}, 0, WHITE);
            DrawRectangleRounded(Rectangle{364.6, 28.3, 287.6, 385.7}, 0.5, 6, TransWhite);

            if (!button_clicked_play)
            {
                menu.Create_MAIN();
                menu.draw_main();
                menu.game.checkClick_Main();

                // If music is stopped by the player after clicking the button, then Pause the music and change the background color of the music button
                if (musicStop)
                {
                    DrawRectangle(419.8, 215.1, 184, 3, button_music_main.text_color);
                    PauseSound(menu.game.background_outer);
                }
            }
            else
            {
                menu.Create_SubMenu();
                menu.draw_submenu();
                menu.game.CheckClick_SubMenu();
            }
        }

        if (start)
        {   
            ClearBackground(BLACK);
            // Screen and Picture is of the same dimensions thatswhy 'source' and 'destination' is same here and also we need full picture in our gamescreen
            DrawTexturePro(texturePlay, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Rectangle{0, 0, (float)screen_width, (float)screen_height}, Vector2{0, 0}, 0, WHITE);

            menu.game.draw();

            if (!gamePause)
            {
                PauseSound(menu.game.background_outer);

                if (!IsSoundPlaying(menu.game.background_inner) && !musicStop)
                {
                    if (gameEnd)
                    {
                        PlaySound(menu.game.background_inner);
                        gameEnd = false;
                    }
                    else {
                        // Restarting background_inner audio file if it is completed
                        if (!Is_Previously_GamePaused)
                        {
                            PlaySound(menu.game.background_inner);
                        }
                        // Resuming the background_inner audio file if it is paused
                        if (Is_Previously_GamePaused)
                        {
                            ResumeSound(menu.game.background_inner);
                            Is_Previously_GamePaused = false;
                        }
                    }
                }

                SetSoundVolume(menu.game.background_inner, 0.2);

                DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
                DrawCircle(screen_width / 2, screen_height / 2, 120, TransWhite);
                DrawText(TextFormat("%i", Player1_Score), screen_width / 4 - 20, 20, 60, WHITE);
                DrawText(TextFormat("%i", Player2_Score), 3 * screen_width / 4 - 20, 20, 60, WHITE);

                menu.game.update();
                menu.game.CheckCollision();
                menu.game.GameEnd();
            }

            if (gamePause)
            {   
                Is_Previously_GamePaused = true;
                menu.Create_SUBMAIN();
                menu.draw_submain();
                
                PauseSound(menu.game.background_inner);
                if (!musicStop)
                {
                    ResumeSound(menu.game.background_outer);
                }
                if (musicStop)
                {
                    PauseSound(menu.game.background_outer);
                    DrawRectangle(419.8, 216.1 + 78.85, 184, 3, button_music_sub.text_color);
                }
                menu.game.CheckClick_SubMain();
            }
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}