# Another implementation of Conway's Game of Life

This game of life is "double tail screen" (the end of corner go to the oposite corner side"). The rules are the same as descripted in wikipedia, [click in this link](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life#Rules) to know if you don't know.

## Dependencies

For can render 2D graphics was used SDL2 to do the dirty job, you can install directly from most populars package managers. See the official page SDL to get the tutorial instalation, [link here](https://wiki.libsdl.org/Installation).

## Installation

Just let make do their magics, run `make` to compile and `make run` to get your Game of Life on screen. Or if you prefer download the compiled version on releases.


## Playing the game

Click on squares to mark and click again to unmark. When you finish yours marks press -> (right arrow) to go to the next step. Is possible too keep the -> (right arrow) pressed to run "infinites" steps and release to stop. 

For close click `ESC` and the game will be closed.


## Game size

For while the game size is hard coded. The grid size default is 640 x 640 with tiles of 10 pixels. In case you compile by your self, can change `WIDTH`, `HIGHT` and `BOX_SIZE` in `constants.h`.


## Known limitations

For apply the rules in all boxes in same time is used a stack to storage what boxes will die, live or stay. The size of this stack is fixed in 200 but easily some patterns can ultrapasses this limit and to avoid memory leaks their are ignored and lost.

