#ifndef ACTORS_H
#define ACTORS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <SDL.h>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

float get_game_speed();
#define PLAYER_SPEED (0.000125 / get_game_speed())
#define ATTACKER_SPEED (0.00125 / get_game_speed())
#define MIN_DIST 0.000001
#define TRUE 1
#define FALSE 0
#define ATTACK_DISPLAY_DURATION 100

struct attacker {
	float x, y, vx, vy, destx, desty;
	float range_remaining;
	float turn_rate;

	int damage;
	int alive;
	int player;

	GLint lines;
};

struct enemy {
	float x, y, vx, vy;
	float size;
	int health;

	int under_attack;
	struct attacker *far_hit_by;
	Uint32 far_hit_started;

	GLint lines;
};

struct player {
	float x, y, vx, vy;
	float destx, desty;	
	float size;
	int health;

	int far_attack;
	Uint32 far_attack_started;
	float far_targetx, far_targety;

	struct attacker *far_hit_by;
	Uint32 far_hit_started;
	
	float close_attack_range;
	Uint32 attack_duration;

	struct enemy *target;
	Uint32 attack_started;
	
	GLint lines;
};



void mag(float x, float y, float *mag);
void diff(float x1, float y1 ,float x2, float y2, float *dx, float *dy);
void norm(float x, float y ,float *nx, float *ny);
void dist(float x1, float y1 ,float x2, float y2, float *d);
void dir(float x1, float y1 ,float x2, float y2, float *dx, float *dy);

struct enemy *get_enemy_at(float x, float y, 
													 struct enemy *enemies, int nenemies);

void init_player(struct player *player);
void init_enemy(struct enemy *enemy);


void update_player(struct player *player, 
									 struct attacker *attackers, int max_attackers);
void update_enemies(struct enemy *enemies, int nenemies);
void update_enemy(struct enemy *enemy);
void update_attacker(struct attacker *attacker,
										 struct player *player, 
										 struct enemy *enemies, int nenemies);

void print_enemy(struct enemy *enemy);
void print_player(struct player *player);

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies);


#endif
