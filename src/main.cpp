#include <SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <omp.h>

#include "program_options.h"


enum CellState {
    TREE, FIRE, EMPTY
};

const int SIZE = 5;
const int WIDTH = 1024;
const int HEIGHT = 1024;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

std::vector<std::vector<CellState>> forest(WIDTH, std::vector<CellState>(HEIGHT, EMPTY));

bool initForest(double p_tree) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }
    window = SDL_CreateWindow("Forest Fire", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * SIZE,
                              HEIGHT * SIZE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng_init(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            forest[i][j] = (dist(rng_init) < p_tree) ? TREE : EMPTY;
        }
    }
    return true;
}

void drawSquare(int x, int y, SDL_Color color) {
    SDL_Rect fillRect = {x * SIZE, y * SIZE, SIZE, SIZE};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &fillRect);
}

void stepForest(double p_fire, double p_grow) {
    std::vector<std::vector<CellState>> newForest = forest;

    // Prepare RNGs for each thread
    int max_threads = omp_get_max_threads();
    std::vector<std::mt19937> rngs(max_threads);
    for (int i = 0; i < max_threads; ++i) {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() + i;
        rngs[i].seed(seed);
    }
    std::uniform_real_distribution<double> dist(0.0, 1.0);

#pragma omp parallel for collapse(2) default(none) shared(forest, newForest, p_fire, p_grow, rngs, WIDTH, HEIGHT) private(dist)
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            // Use the RNG for the current thread
            std::mt19937 &rng_local = rngs[omp_get_thread_num()];

            if (forest[x][y] == FIRE) {
                newForest[x][y] = EMPTY;
            } else if (forest[x][y] == TREE) {
                bool fireNearby =
                        (x > 0 && forest[x - 1][y] == FIRE) ||
                        (y > 0 && forest[x][y - 1] == FIRE) ||
                        (x < WIDTH - 1 && forest[x + 1][y] == FIRE) ||
                        (y < HEIGHT - 1 && forest[x][y + 1] == FIRE);

                if (fireNearby || dist(rng_local) < p_fire) {
                    newForest[x][y] = FIRE;
                }
            } else if (forest[x][y] == EMPTY) {
                if (dist(rng_local) < p_grow) {
                    newForest[x][y] = TREE;
                }
            }
        }
    }

    forest = newForest;
}

int main(int argc, char *argv[]) {
    double p_tree = 0.5, p_fire = 0.001, p_grow = 0.01;

    const std::vector<std::string_view> args(argv, argv + argc);
    bool measurement = program_options::has(args, "-m");
    bool help = program_options::has(args, "-h");

    if (help) {
        program_options::description();
        return EXIT_SUCCESS;
    }

    if (!initForest(p_tree)) {
        // Handle error
        return EXIT_FAILURE;
    }

    bool quit = false;
    SDL_Event e;

    if (measurement) {
        int num_steps[] = {1, 10, 100, 1000, 10000}; // array to hold different step values

        // Iterate over all elements in num_steps
        for (int steps: num_steps) {
            // Take note of start time
            auto start = std::chrono::high_resolution_clock::now();

            // Perform steps and draw accordingly
            for (int s = 0; s < steps; ++s) {
                // Drawing could be optional when you're measuring performance
                stepForest(p_fire, p_grow);
            }

            // Take note of stop time and calculate the duration
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            // Output time taken to the console
            std::cout << "Time taken for " << steps << " steps: " << duration.count() << "ms" << std::endl;
        }

        return EXIT_SUCCESS;
    }


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {  // Detects mouse button press
                if (e.button.button == SDL_BUTTON_LEFT) { // Checks if the left button was pressed
                    int x, y;
                    SDL_GetMouseState(&x, &y); // Gets the mouse position at the time of the click

                    // Convert pixel coordinates to grid coordinates
                    int gridX = x / SIZE;
                    int gridY = y / SIZE;

                    // Start a fire at the clicked location
                    if (forest[gridX][gridY] == TREE) {
                        forest[gridX][gridY] = FIRE;
                    }
                }
            }
        }

        stepForest(p_fire, p_grow);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < WIDTH; ++i) {
            for (int j = 0; j < HEIGHT; ++j) {
                if (forest[i][j] == TREE) {
                    drawSquare(i, j, {0, 128, 0, 255}); // Green for tree
                } else if (forest[i][j] == FIRE) {
                    drawSquare(i, j, {255, 0, 0, 255}); // Red for fire
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100); // Wait time before updating the frame (milliseconds)
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
