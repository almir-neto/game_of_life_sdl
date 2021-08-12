#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <SDL2/SDL.h>
#include "./constants.h"

//TYPES
typedef struct {
    SDL_Rect rectangle;
    bool active;
} tile_t;

typedef struct{
    size_t i;
    size_t j;
    bool active;
} action_tile;

//GLOBAL VARIABLES
SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL;
tile_t grid[WIDTH/BOX_SIZE][HEIGHT/BOX_SIZE];

int game_is_running = false;
int last_frame_time = 0;
int time_to_wait = 0;
float delta_time = 0;
int safe_index(int possible_invalid_index, int max_index);
action_tile stack_action[200];
size_t stack_index = 0;

//FUNCTIONS DEFINITIONS
void apply_rules();
tile_t* neighborhood(int i, int j);
size_t count_neighbors(tile_t* neighborhood);
void push_action(action_tile action);
void pop_action();

//GAME OF LIFE LOGIC
int initializeWindow(void){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Fail to init SDL!\n");
        return false;
    }
    window = SDL_CreateWindow(
            NULL, 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_BORDERLESS);

    if(!window){
        fprintf(stderr, "Fail to create a window.");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if(!renderer){
        fprintf(stderr, "Fail to create renderer.\n");
        return false;
    }
    return true;
}
void processInput(){
    SDL_Event event;
    SDL_PollEvent(&event);

    int i, j;
    switch(event.type){
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = false;

            if(event.key.keysym.sym == SDLK_RIGHT)
                apply_rules();
            break;
        case SDL_MOUSEBUTTONDOWN:
            i = (event.motion.x / BOX_SIZE) * BOX_SIZE;
            j = (event.motion.y / BOX_SIZE) * BOX_SIZE;
            i = i - 1; 
            j = j - 1;
            if(grid[i/BOX_SIZE+1][j/BOX_SIZE+1].active){
                grid[i/BOX_SIZE+1][j/BOX_SIZE+1].active = false;
            }else{
                grid[i/BOX_SIZE+1][j/BOX_SIZE+1].active = true;
            }
            break;
    }
}

void setup() {
    int i, j;
    for(i = 0; i < WIDTH-1; i+=BOX_SIZE){
        for(j = 0; j < HEIGHT-1; j+= BOX_SIZE){
            SDL_Rect tmp = {
                i, j,
                BOX_SIZE, BOX_SIZE
            };
            grid[i/BOX_SIZE][j/BOX_SIZE].rectangle = tmp;
            grid[i/BOX_SIZE][j/BOX_SIZE].active = false;
        }
    }
}

void update(){

//    time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
//    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
//        SDL_Delay(time_to_wait);
//
//    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
//
//    last_frame_time = SDL_GetTicks();
    int i;
    for(i = 0; i < stack_index; i++){
        grid[stack_action[i].i][stack_action[i].j].active = stack_action[i].active;
    }
    stack_index = 0;
}

void render(){
    int i, j;
    SDL_SetRenderDrawColor(renderer, 0, 0,0, 100);
    //LIMA O BUFFER PRA PODER DESENHAR EM CIMA
    SDL_RenderClear(renderer);

    // AREA PARA O DESENHO DOS OBJETOS DO JOGO

    SDL_SetRenderDrawColor(renderer, 3, 0,200, 100);
    for(i = 0; i < WIDTH-1; i+=BOX_SIZE){
        for(j = 0; j < HEIGHT-1; j+= BOX_SIZE){
            if(grid[i/BOX_SIZE][j/BOX_SIZE].active){
                SDL_RenderFillRect(renderer, &grid[i/BOX_SIZE][j/BOX_SIZE].rectangle);
            }else{
                SDL_RenderDrawRect(renderer, &grid[i/BOX_SIZE][j/BOX_SIZE].rectangle);
            }
        }
    }

    // MOSTRA O BUFFER NA TELA
    SDL_RenderPresent(renderer);
}

void apply_rules(){
    int i, j;
    for(i = 0; i < WIDTH-1; i+= BOX_SIZE){ 
        for(j = 0; j < HEIGHT-1; j+= BOX_SIZE){ 
            tile_t* neighbors = neighborhood(i/BOX_SIZE, j/BOX_SIZE); 
            size_t alives_neighbors = count_neighbors(neighbors);
            action_tile tmp = {
                i/BOX_SIZE,
                j/BOX_SIZE,
                false
            };
            if(alives_neighbors < 2 && grid[i/BOX_SIZE][j/BOX_SIZE].active){
                tmp.active = false;
                push_action(tmp);
            }else if(alives_neighbors > 3 && grid[i/BOX_SIZE][j/BOX_SIZE].active){
                tmp.active = false;
                push_action(tmp);
            }else if(alives_neighbors == 3 && !grid[i/BOX_SIZE][j/BOX_SIZE].active){
                tmp.active = true;
                push_action(tmp);
            }
        }
    }
}
tile_t* neighborhood(int i, int j){
    // * Eight neighbors, the diagonals are counted as neighbors too.
    // * Returns the neighbors in clockwise starting from top center.
    // Positions in result are as follow:
    // 7 0 1
    // 6 * 2
    // 5 4 3
    tile_t* result = (tile_t*)malloc(sizeof(tile_t)*8);
    result[0] = grid[i][j];   
    result[0] = grid[i][safe_index(j-1, HEIGHT/BOX_SIZE)];
    result[1] = grid[safe_index(i+1, WIDTH/BOX_SIZE)][safe_index(j-1, HEIGHT/BOX_SIZE)];
    result[2] = grid[safe_index(i+1,WIDTH/BOX_SIZE)][j];
    result[3] = grid[safe_index(i+1, WIDTH/BOX_SIZE)][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[4] = grid[safe_index(i, WIDTH/BOX_SIZE)][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[5] = grid[safe_index(i-1, WIDTH/BOX_SIZE)][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[6] = grid[safe_index(i-1, WIDTH/BOX_SIZE)][safe_index(j, HEIGHT/BOX_SIZE)];
    result[7] = grid[safe_index(i-1, WIDTH/BOX_SIZE)][safe_index(j-1, HEIGHT/BOX_SIZE)];

    return result;
}

size_t count_neighbors(tile_t* neighbors){
    int i;
    size_t counter = 0;
    for(i = 0; i < 8; i++){
        if(neighbors[i].active)
            counter = counter + 1;
    }
    return counter;
}

int safe_index(int possible_invalid_index, int max_index){
    if(possible_invalid_index > max_index)
        return 0;
    if(possible_invalid_index < 0)
        return max_index-1;
    return possible_invalid_index;
}

void push_action(action_tile action){
    printf("Pushing in index: %li\n", stack_index);
    stack_action[stack_index] = action;
    stack_index = stack_index + 1;
    if(stack_index > 200)
        stack_index = 0;
}

void pop_action(){
    stack_index = stack_index - 1;
}
void destroyWindow(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(){
    game_is_running = initializeWindow();

    setup();

    while(game_is_running){
        processInput();
        update();
        render();
    }

    destroyWindow();
    return 0;
}
