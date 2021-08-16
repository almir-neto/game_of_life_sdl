#ifndef _UTILS_H
#define _UTILS_H

#include <SDL2/SDL.h>
#include "./types.h"

void apply_rules();
tile_t* neighborhood(int i, int j);
size_t count_neighbors(tile_t* neighborhood);
void push_action(action_tile_t action);
void pop_action();
void render_slider(SDL_Renderer* render, slider_t* slider);
int safe_index(int index, int max_index);

#endif
