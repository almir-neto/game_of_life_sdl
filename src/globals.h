#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./types.h"
#include "./constants.h"

SDL_Window* window = NULL; 
SDL_Renderer* renderer = NULL;
tile_t grid[WIDTH/BOX_SIZE][HEIGHT/BOX_SIZE];

int game_is_running = false;
int last_frame_time = 0;
int time_to_wait = 0;
float delta_time = 0;
size_t stack_index = 0;
action_tile_t stack_action[WIDTH * HEIGHT];
slider_t zoom_slider;

#endif
