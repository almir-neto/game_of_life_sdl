#include <malloc.h>
#include "./utils.h"
#include "./types.h"
#include "./constants.h"
#include "./globals.h"

extern SDL_Renderer* renderer;
extern tile_t grid[WIDTH/BOX_SIZE][HEIGHT/BOX_SIZE];
extern size_t stack_index;
extern action_tile_t stack_action[200];
extern slider_t zoom_slider;

void apply_rules(){
    int i, j;
    for(i = 0; i < WIDTH-1; i+= BOX_SIZE){ 
        for(j = 0; j < HEIGHT-1; j+= BOX_SIZE){ 
            tile_t* neighbors = neighborhood(i/BOX_SIZE, j/BOX_SIZE); 
            size_t alives_neighbors = count_neighbors(neighbors);
            action_tile_t tmp = {
                i/BOX_SIZE,
                j/BOX_SIZE,
                false
            };
#ifdef _DEBUG
            if(grid[i/BOX_SIZE][j/BOX_SIZE].active)
                tmp.show_neighbors = true;
#endif

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

            free(neighbors);
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

    result[0] = grid[safe_index(i-1, WIDTH/BOX_SIZE)][j];
    result[1] = grid[safe_index(i-1, WIDTH/BOX_SIZE)][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[2] = grid[i][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[3] = grid[safe_index(i+1, WIDTH/BOX_SIZE)][safe_index(j+1, HEIGHT/BOX_SIZE)];
    result[4] = grid[safe_index(i+1, WIDTH/BOX_SIZE)][j];
    result[5] = grid[safe_index(i+1, WIDTH/BOX_SIZE)][safe_index(j-1, HEIGHT/BOX_SIZE)];
    result[6] = grid[i][safe_index(j-1, HEIGHT/BOX_SIZE)];
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

int safe_index(int index, int max_index){
    if(index >= max_index)
        return 0;
    if(index < 0)
        return max_index - 1;
    return index;
}

void push_action(action_tile_t action){
    printf("Pushing in index: %li\n", stack_index);
    stack_action[stack_index] = action;
    stack_index = stack_index + 1;
    if(stack_index > 200)
        stack_index = 0;
}

void pop_action(){
    stack_index = stack_index - 1;
}


void render_slider(SDL_Renderer *renderer, slider_t* slider){
    SDL_SetRenderDrawColor(
            renderer,
            slider->color_line.r,
            slider->color_line.g,
            slider->color_line.b,
            slider->color_line.a
            );
    SDL_RenderFillRect(renderer, &slider->line);

    SDL_SetRenderDrawColor(
            renderer,
            slider->color_mark.r,
            slider->color_mark.g,
            slider->color_mark.b,
            slider->color_mark.a
            );
    SDL_RenderFillRect(renderer, &slider->mark);
}
