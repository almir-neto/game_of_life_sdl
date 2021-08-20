#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <SDL2/SDL.h>
#include "./constants.h"
#include "./types.h"
#include "./utils.h"

//GLOBAL VARIABLES
extern SDL_Window* window ; 
extern SDL_Renderer* renderer;
extern tile_t grid[WIDTH/BOX_SIZE][HEIGHT/BOX_SIZE];
extern int game_is_running;
extern int last_frame_time;
extern int time_to_wait;
extern float delta_time;
extern size_t stack_index;
extern action_tile_t stack_action[WIDTH * HEIGHT];
extern slider_t zoom_slider;

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

            if(event.key.keysym.sym == SDLK_RIGHT){
#ifdef _DEBUG
                stack_index = 0;
#endif
                apply_rules();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&i, &j);
            i = i - 1; 
            j = j - 1;
            if(grid[i/BOX_SIZE][j/BOX_SIZE].active){
                grid[i/BOX_SIZE][j/BOX_SIZE].active = false;
            }else{
                grid[i/BOX_SIZE][j/BOX_SIZE].active = true;
            }
            break;
        case SDL_MOUSEWHEEL:
            if(event.wheel.y < 0){
                if(zoom_slider.current_position < zoom_slider.end){
                    zoom_slider.current_position += 5;
                    zoom_slider.mark.x += 6;
                }
            }else if(event.wheel.y > 0){
                if(zoom_slider.current_position > zoom_slider.start){
                    zoom_slider.current_position -= 5;
                    zoom_slider.mark.x -= 6;
                }
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

    // Init slider zoom
    SDL_Color slide_color = {147, 33, 151};
    SDL_Color mark_color = {229, 150, 232};
    SDL_Rect slide_axis = { 70, 70, 500, 5 };
    SDL_Rect slide_mark = { 70, 60, 5, 25 };

    slider_t zslider = {
        100, //start
        500, //end
        100, //current position
        slide_axis,
        slide_mark,
        slide_color,
        mark_color
    };
    zoom_slider = zslider;
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
}

void render(){
    int i, j;
    SDL_SetRenderDrawColor(renderer, 0, 0,0, 100);
    //LIMA O BUFFER PRA PODER DESENHAR EM CIMA
    SDL_RenderClear(renderer);

    // AREA PARA O DESENHO DOS OBJETOS DO JOGO

    SDL_SetRenderDrawColor(renderer, 100, 100,100, 100);
    for(i = 0; i < WIDTH-1; i+=BOX_SIZE){
        for(j = 0; j < HEIGHT-1; j+= BOX_SIZE){
            if(grid[i/BOX_SIZE][j/BOX_SIZE].active){
                SDL_RenderFillRect(renderer, &grid[i/BOX_SIZE][j/BOX_SIZE].rectangle);
            }else{
                SDL_RenderDrawRect(renderer, &grid[i/BOX_SIZE][j/BOX_SIZE].rectangle);
            }
        }
    }

#ifdef _DEBUG
    for(i=0; i < stack_index; i++){
        if(stack_action[i].show_neighbors){
            tile_t* n = neighborhood(stack_action[i].i, stack_action[i].j);
            for(j=0; j<8; j++){
                switch(j){
                    case 0:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
                        break;
                    case 1:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
                        break;
                    case 2:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
                        break;
                    case 3:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
                        break;
                    case 4:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
                        break;
                    case 5:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 100);
                        break;
                    case 6:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 100);
                        break;
                    case 7:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
                        break;
                }
                SDL_RenderFillRect(renderer, &n[j].rectangle);
            }
            free(n);
        }
    }
#else
    stack_index = 0;
#endif

    //render_slider(renderer, &zoom_slider);
    // MOSTRA O BUFFER NA TELA
    SDL_RenderPresent(renderer);
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
