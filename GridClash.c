#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 300
#define BOARD_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / BOARD_SIZE)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

enum Mode {
    PLAYER_VS_PLAYER,
    PLAYER_VS_COMPUTER
};

enum State {
    EMPTY,
    PLAYER_X,
    PLAYER_O
};

int board[BOARD_SIZE][BOARD_SIZE] = {{EMPTY}};

enum Mode mode = PLAYER_VS_PLAYER;
int currentPlayer = PLAYER_X;
int winner = EMPTY;

int minimax(int depth, int isMaximizing);
int checkWinner();
void drawBoard();
void playerMove(int row, int col);
void computerMove();

void drawEllipse(SDL_Renderer* renderer, int x, int y, int radiusX, int radiusY) {
    int centerX = x + radiusX;
    int centerY = y + radiusY;

    for (int i = 0; i < 360; i++) {
        double angle = i * M_PI / 180;
        int newX = centerX + radiusX * cos(angle);
        int newY = centerY + radiusY * sin(angle);
        SDL_RenderDrawPoint(renderer, newX, newY);
    }
}

void drawBoard() {
    // Set background color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Draw grid lines
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);
    }

    // Draw X's and O's
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == PLAYER_X) {
                SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + CELL_SIZE, rect.y + CELL_SIZE);
                SDL_RenderDrawLine(renderer, rect.x + CELL_SIZE, rect.y, rect.x, rect.y + CELL_SIZE);
            } else if (board[i][j] == PLAYER_O) {
                SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                drawEllipse(renderer, rect.x + CELL_SIZE / 2, rect.y + CELL_SIZE / 2, CELL_SIZE / 2, CELL_SIZE / 2);
            }
        }
    }

    // Draw sidebar for mode selection
    SDL_Rect sidebar = {SCREEN_WIDTH, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &sidebar);

    // Present the renderer
    SDL_RenderPresent(renderer);
}




int checkWinner() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i][0] != EMPTY && board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            return board[i][0];
        }
        if (board[0][i] != EMPTY && board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            return board[0][i];
        }
    }

    if (board[0][0] != EMPTY && board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != EMPTY && board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        return board[0][2];
    }

    int tie = 1;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY) {
                tie = 0;
                break;
            }
        }
    }
    if (tie) {
        return -1;
    }

    return EMPTY;
}

int minimax(int depth, int isMaximizing) {
    int result = checkWinner();
    if (result != EMPTY) {
        if (result == PLAYER_O) return 1;
        else if (result == PLAYER_X) return -1;
        else return 0;
    }

    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER_O;
                    int score = minimax(depth + 1, 0);
                    board[i][j] = EMPTY;
                    bestScore = (score > bestScore) ? score : bestScore;
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER_X;
                    int score = minimax(depth + 1, 1);
                    board[i][j] = EMPTY;
                    bestScore = (score < bestScore) ? score : bestScore;
                }
            }
        }
        return bestScore;
    }
}

void computerMove() {
    int bestScore = -1000;
    int row = -1;
    int col = -1;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY) {
                board[i][j] = PLAYER_O;
                int score = minimax(0, 0);
                board[i][j] = EMPTY;
                if (score > bestScore) {
                    bestScore = score;
                    row = i;
                    col = j;
                }
            }
        }
    }
    board[row][col] = PLAYER_O;
}

void playerMove(int row, int col) {
    if (winner != EMPTY || board[row][col] != EMPTY) {
        return;
    }

    board[row][col] = currentPlayer;
    winner = checkWinner();
    if (winner == EMPTY) {
        if (mode == PLAYER_VS_PLAYER) {
            currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
        } else {
            computerMove();
            winner = checkWinner();
            if (winner == EMPTY) {
                currentPlayer = PLAYER_X;
            }
        }
    }
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH + SCREEN_WIDTH / 4, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf", 24);
    if (!font) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / CELL_SIZE;
                int col = x / CELL_SIZE;
                if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
                    playerMove(row, col);
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_2) {
                    mode = PLAYER_VS_COMPUTER;
                    currentPlayer = PLAYER_X;
                    winner = EMPTY;
                    for (int i = 0; i < BOARD_SIZE; ++i) {
                        for (int j = 0; j < BOARD_SIZE; ++j) {
                            board[i][j] = EMPTY;
                        }
                    }
                }
            }
        }

        drawBoard();

        if (winner != EMPTY) {
            char message[50];
            if (winner == PLAYER_X) {
                sprintf(message, "Player X wins!");
            } else if (winner == PLAYER_O) {
                sprintf(message, "Player O wins!");
            } else {
                sprintf(message, "It's a tie!");
            }
            SDL_Color color = {255, 255, 255};
            SDL_Surface* surface = TTF_RenderText_Solid(font, message, color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            int texW = 0;
            int texH = 0;
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            SDL_Rect dstrect = {SCREEN_WIDTH / 2 - texW / 2, SCREEN_HEIGHT / 2 - texH / 2, texW, texH};
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            SDL_RenderPresent(renderer);

            SDL_Delay(2000);
            currentPlayer = PLAYER_X;
            winner = EMPTY;
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    board[i][j] = EMPTY;
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
