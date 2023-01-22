/*
 * lab1b.c
 *
 *  Created on:
 *      Author:
 */

/* include helper functions for game */
#include <stdio.h>
#include "lifegame.h"

/* add whatever other includes here */

/* number of generations to evolve the world */
#define NUM_GENERATIONS 50

/* functions to implement -- can copy from Part A */

/* this function should set the state of all
   the cells in the next generation and call
   finalize_evolution() to update the current
   state of the world to the next generation */
void next_generation(void);

/* this function should return the state of the cell
   at (x,y) in the next generation, according to the
   rules of Conway's Game of Life (see handout) */
int get_next_state(int x, int y);

/* this function should calculate the number of alive
   neighbors of the cell at (x,y) */
int num_neighbors(int x, int y);

int main(int argc, char ** argv)
{

	/* TODO: initialize the world, from file argv[1]
	   if command line argument provided (argc > 1), or
	   using hard-coded pattern (use Part A) otherwise */
	if (argc > 1) {
		initialize_world_from_file(argv[1]);
	} else {
		initialize_world();
	}
	int n;

	for (n = 0; n < NUM_GENERATIONS; n++) {
		output_world();
		next_generation();
	}

	/* TODO: output final world state to console and save
	   world to file "world.txt" in current directory */
	output_world();

	return 0;
}

void next_generation(void) {
	/* TODO: for every cell, set the state in the next
	   generation according to the Game of Life rules

	   Hint: use get_next_state(x,y) */

	for (int x = 0, width = get_world_width(); x < width; ++x) {
		for (int y = 0, height = get_world_height(); y < height; ++y) {
			set_cell_state(x, y, get_next_state(x, y));
		}
	}

	finalize_evolution(); /* called at end to finalize */
}

int get_next_state(int x, int y) {
	/* TODO: for the specified cell, compute the state in
	   the next generation using the rules

	   Use num_neighbors(x,y) to compute the number of live
	   neighbors */
	int	neighbors = num_neighbors(x, y);

	if (get_cell_state(x, y) == ALIVE) {
		if (neighbors >= 2 && neighbors <= 3) {
			return ALIVE;
		} else {
			return DEAD; 
		}
	} else if (neighbors == 3) {
		return ALIVE;
	}
	return DEAD;
}

int num_neighbors(int x, int y) {
	/* TODO: for the specified cell, return the number of
	   neighbors that are ALIVE

	   Use get_cell_state(x,y) */
	int	neighbors_dst = 0;

	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			neighbors_dst += get_cell_state(x + i, y + j);
		}
	}
	if (get_cell_state(x, y)) {
		--neighbors_dst;
	}
	return neighbors_dst;
}
