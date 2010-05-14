#ifndef ACTORS_H
#define ACTORS_H

#include <math.h>
#include <stdio.h>
#include <sdtlib.h>

struct player {
	float x, y, vx, vy;
	float size;
	int health;
};

struct enemy {
	float x, y, vx, vy;
	float size;
	int health;
};

struct attacker {
	float x, y, vx, vy;
	int damage;
};

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies);

#endif
