#ifndef _TYPES_H
#define _TYPES_H

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Rect rectangle;
    bool active;
} tile_t;

typedef struct{
    size_t i;
    size_t j;
    bool active;
    bool show_neighbors;
} action_tile_t;

typedef struct {
    size_t start;
    size_t end;
    size_t current_position;
    SDL_Rect line;
    SDL_Rect mark;
    SDL_Color color_line;
    SDL_Color color_mark;
} slider_t;

#endif
