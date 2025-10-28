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

constexpr int ballWidth = 30;
constexpr int ballHeight = 30;
constexpr int ballXLeftBoundary = 5;
constexpr int ballXRightBoundary = SCREEN_WIDTH - ballWidth - 5;
int ballX = (SCREEN_WIDTH - ballWidth) / 2;
constexpr int ballY = 510;
constexpr int ballSpeed = 10;

const char* FONT_PATH = "images/consolas.ttf";
constexpr int FONT_SIZE = 32;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Texture* ballTexture = nullptr;
SDL_Texture* gameOverTexture = nullptr;
SDL_Texture* replayTexture = nullptr;

TTF_Font* font = nullptr;

bool gameOver = false;
bool quit = false;

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
    ballTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("ball.png")).c_str());
    if (!ballTexture)
    {
        cout << "IMG_LoadTexture images/ball.png error: " << IMG_GetError() << endl;
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
    gameOver = false;
}

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_f)
            {
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYUP:
            if (gameOver)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                    ResetGame();
            }
            break;
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    if (currentKeyStates[SDL_SCANCODE_LEFT]) // left arrow key
    {
        // ballX = ballX > ballXLeftBoundary
        //                  ? ballX - ballSpeed // within left boundary
        //                  : ballXLeftBoundary;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) // right arrow key
    {
        // ballX = (ballX < ballXRightBoundary)
        //                  ? ballX + ballSpeed // within right boundary
        //                  : ballXRightBoundary;
    }
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

    while (!quit)
    {
        handleEvents();

        //reset
        SDL_RenderClear(renderer);

        if (!gameOver)
        {
            // render ball
            SDL_Rect ballRect = {ballX, ballY, ballWidth, ballHeight};
            SDL_RenderCopy(renderer, ballTexture, nullptr, &ballRect);

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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
