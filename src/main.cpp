#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

using namespace std;

auto SPRITES_FOLDER = "images/";

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

// ball related
constexpr int ballWidth = 30;
constexpr int ballHeight = 30;
constexpr int ballXLeftBoundary = 0;
constexpr int ballXRightBoundary = SCREEN_WIDTH - ballWidth;
constexpr int ballYTopBoundary = 0;
constexpr int ballYBottomBoundary = SCREEN_HEIGHT - ballHeight;
int ballX = SCREEN_WIDTH / 2;
int ballY = 210;
int ballXVelocity = 400;
int ballYVelocity = 400;

// paddle related
const auto paddleImagePath = SPRITES_FOLDER + string("paddleRed.png");
constexpr int paddleWidth = 104;
constexpr int paddleHeight = 24;
constexpr int paddleXLeftBoundary = 0;
constexpr int paddleXRightBoundary = SCREEN_WIDTH - paddleWidth;
constexpr int paddleYTopBoundary = 0;
constexpr int paddleYBottomBoundary = SCREEN_HEIGHT - paddleHeight - ballHeight;
int paddleX = (paddleXRightBoundary) / 2;
constexpr int paddleY = paddleYBottomBoundary;
constexpr int paddleSpeed = 12;

// brick related
const auto brickImagePath = SPRITES_FOLDER + string("brick.png");
constexpr int brickWidth = 64;
constexpr int brickHeight = 32;
SDL_Texture* brickTexture = nullptr;
constexpr int rows = 3;
constexpr int columns = 10;
SDL_Rect bricks[rows][columns];
int bricksRemaining = rows * columns;


auto FONT_PATH = "images/consolas.ttf";
constexpr int FONT_SIZE = 32;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* ballTexture = nullptr;
SDL_Texture* paddleTexture = nullptr;
SDL_Texture* gameOverTexture = nullptr;
SDL_Texture* replayTexture = nullptr;

TTF_Font* font = nullptr;

bool continueGame = true;
bool continuePlaying = true;

void initialiseBricks()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            bricks[i][j] = {
                33 + j * (brickWidth + 10), 50 + i * (brickHeight + 10),
                brickWidth, brickHeight
            };
        }
    }
}

void renderBricks()
{
    if (bricksRemaining <= 0)
    {
        continueGame = false;
        return;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (bricks[i][j].y > 0) // haven't been knocked out yet
            {
                SDL_RenderCopy(renderer, brickTexture, nullptr, &bricks[i][j]);
            }
        }
    }
}

void detectBallCollisionWithWalls()
{
    if (ballX < ballXLeftBoundary || ballX > ballXRightBoundary)
    {
        ballXVelocity = -ballXVelocity;
    }
    if (ballY < ballYTopBoundary) // hit the top
    {
        ballYVelocity = -ballYVelocity;
    }
    else if (ballY > ballYBottomBoundary) // fell out of the bottom
    {
        continueGame = false;
    }
}

void detectBallCollisionWithPaddle()
{
    if (ballYVelocity < 0 || ballY + ballHeight + 2 < paddleY) // going up or above paddle
    {
        return;
    }

    SDL_Rect ballRect = {ballX, ballY, ballWidth, ballHeight};
    SDL_Rect paddleRect = {paddleX, paddleY, paddleWidth, paddleHeight};
    if (SDL_HasIntersection(&ballRect, &paddleRect)) // bounce up
    {
        ballYVelocity = -ballYVelocity;
    }
}

bool checkCollision(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
    return SDL_HasIntersection(&rect1, &rect2);
}

void detectBallCollisionWithBricks()
{
    SDL_Rect ballRect = {ballX, ballY, ballWidth, ballHeight};
    for (int i = rows - 1; i >= 0; i--)
    {
        for (int j = 0; j < columns; j++)
        {
            SDL_Rect* brick = &bricks[i][j];
            if (brick->y > 0 && checkCollision(ballRect, *brick))
            {
                brick->y = -1000;
                ballYVelocity = -ballYVelocity;

                return;
            }
        }
    }
}

void moveBallAndRender()
{
    ballX += ballXVelocity;
    ballY += ballYVelocity;

    detectBallCollisionWithWalls();

    if (ballYVelocity > 0 && ballY + ballHeight + 2 >= paddleY) // going down
    {
        detectBallCollisionWithPaddle();
    }
    else if (ballY < 200) // near the brick wall bottom
    {
        detectBallCollisionWithBricks();
    }

    SDL_Rect ballRect = {ballX, ballY, ballWidth, ballHeight};
    SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);
}

void paddleRender()
{
    SDL_Rect paddleRect = {paddleX, paddleY, paddleWidth, paddleHeight};
    SDL_RenderCopy(renderer, paddleTexture, nullptr, &paddleRect);
}

bool initialiseSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return false;

    if (TTF_Init() == -1)
    {
        cout << "TTF_Init error" << TTF_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Brick",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                              0);
    if (!window)
    {
        cout << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cout << "SDL_CreateRenderer error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    return true;
}

bool loadMedia()
{
    backgroundTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("bg4a.jpg")).c_str());
    if (!backgroundTexture)
    {
        cout << "IMG_LoadTexture images/bg4a.jpg error: " << IMG_GetError() << endl;
        return false;
    }

    brickTexture = IMG_LoadTexture(renderer, brickImagePath.c_str());
    if (!brickTexture)
    {
        cout << "IMG_LoadTexture images/brick.png error: " << IMG_GetError() << endl;
        return false;
    }

    ballTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("ball.png")).c_str());
    if (!ballTexture)
    {
        cout << "IMG_LoadTexture images/ball.png error: " << IMG_GetError() << endl;
        return false;
    }

    paddleTexture = IMG_LoadTexture(renderer, paddleImagePath.c_str());
    if (!paddleTexture)
    {
        cout << "IMG_LoadTexture images/paddleRed.png error: " << IMG_GetError() << endl;
        return false;
    }

    gameOverTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("gameover.png")).c_str());
    if (!gameOverTexture)
    {
        cout << "IMG_LoadTexture images/gameover.png error: " << IMG_GetError() << endl;
        return false;
    }

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font)
    {
        cout << "TTF_OpenFont error: " << TTF_GetError() << endl;
        return false;
    }

    return true;
}

void RenderText(const char* text, SDL_Texture*& texture, SDL_Rect& destRect)
{
    SDL_Color textColour = {255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColour);
    texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    destRect.w = textSurface->w;
    destRect.h = textSurface->h;

    SDL_FreeSurface(textSurface);

    destRect.x = (SCREEN_WIDTH - destRect.w) / 2;
}

void RenderReplayText()
{
    SDL_Rect replayRect;

    replayRect.y = SCREEN_HEIGHT / 2 + 50;

    RenderText("Press Spacebar to Replay", replayTexture, replayRect);

    SDL_RenderCopy(renderer, replayTexture, nullptr, &replayRect);
}

void ResetGame()
{
    bricksRemaining = rows * columns;
    initialiseBricks();

    ballX = rand() % ballXRightBoundary;
    ballY = 250; // below the bricks

    ballXVelocity = rand() % 2 ? 2 : -2; // go left or right
    ballYVelocity = 2;

    paddleX = (paddleXRightBoundary) / 2; // middle of screen

    continueGame = true;
}

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            continuePlaying = false;
            break;
        case SDL_KEYUP:
            if (!continueGame)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                    ResetGame();
            }
            break;
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    if (currentKeyStates[SDL_SCANCODE_LEFT] && paddleX > paddleXLeftBoundary) // left arrow key
    {
        paddleX -= paddleSpeed;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT] && paddleX < paddleXRightBoundary) // right arrow key
    {
        paddleX += paddleSpeed;
    }
}


void Destroy()
{
    SDL_DestroyTexture(brickTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(ballTexture);
    SDL_DestroyTexture(paddleTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(replayTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

int main()
{
    if (!initialiseSDL())
        return 1;

    if (!loadMedia())
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // random seed
    srand(time(nullptr));

    ResetGame();

    while (continuePlaying)
    {
        handleEvents();

        //reset
        SDL_RenderClear(renderer);

        if (continueGame)
        {
            // render background
            SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);

            renderBricks();

            paddleRender();

            // render ball
            moveBallAndRender();

            SDL_RenderPresent(renderer);
        }
        else
        {
            SDL_Rect gameOverRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);

            RenderReplayText();

            SDL_RenderPresent(renderer);
        }
    }

    Destroy();
}
