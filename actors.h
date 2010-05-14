#ifndef ACTORS_H
#define ACTORS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <SDL.h>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

float get_game_speed();
#define PLAYER_SPEED (0.0125 / get_game_speed())
#define TRUE 1
#define FALSE 0

struct enemy {
	float x, y, vx, vy;
	float size;
	int health;

	GLint lines;
};

struct player {
	float x, y, vx, vy;
	float destx, desty;	
	float size;
	int health;

	float close_attack_range;
	Uint32 attack_duration;

	struct enemy *target;
	Uint32 attack_started;
	GLint lines;
};

struct attacker {
	float x, y, vx, vy;
	int damage;

	GLint lines;
};



void mag(float x, float y, float *mag);
void diff(float x1, float y1 ,float x2, float y2, float *dx, float *dy);
void norm(float x, float y ,float *nx, float *ny);
void dist(float x1, float y1 ,float x2, float y2, float *d);

struct enemy *get_enemy_at(float x, float y, 
													 struct enemy *enemies, int nenemies);

void init_player(struct player *player);
void init_enemy(struct enemy *enemy);


void update_player(struct player *player);
void update_enemies(struct enemy *enemies, int nenemies);
void update_enemy(struct enemy *enemy);
void update_attacker(struct attacker *attacker);

void print_enemy(struct enemy *enemy);
void print_player(struct player *player);

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies);


#endif
