#ifndef ACTORS_H
#define ACTORS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <SDL.h>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

float get_game_speed();
#define PLAYER_SPEED 0.0125
#define ATTACKER_SPEED 0.03125
#define ENEMY_SPEED 0.0025 
#define MIN_DIST 0.000001
#define TRUE 1
#define FALSE 0
#define ATTACK_DISPLAY_DURATION 100

enum attacker_types { missile, laser };
struct attacker {
	float x, y, vx, vy, destx, desty;
	float range_remaining;
	float turn_rate;

	int damage;
	int alive;
	int player;

	enum attacker_types type;
};

struct enemy {
	float x, y, vx, vy;
	float size;
	int health;

	float turn_rate;

	float attra;
	float align;
	float chase;

	float attract_dist;
	float align_dist;
	
	/* outgoing attacks */
	float near_attack_range;
	Uint32 near_attack_duration;
	Uint32 near_attack_started;
	Uint32 near_attack_damage;


	/* incoming attacks */
	int under_attack;
	struct attacker *hit_by;
	Uint32 hit_started;
};

struct player {
	float x, y, vx, vy, dx, dy;
	float destx, desty;	
	float size;
	int health;

	/* outgoing attacks */
	int far_attack;
	float far_attack_range;
	Uint32 far_attack_started;
	float far_targetx, far_targety;
	Uint32 far_attack_duration;
	int far_attack_damage;

	float near_attack_range;
	Uint32 near_attack_duration;
	struct enemy *target;
	Uint32 near_attack_started;
	int near_attack_damage;

	/* incoming attacks */
	struct attacker *hit_by;
	Uint32 hit_started;
	
	unsigned int score;
};



void mag(float x, float y, float *mag);
void diff(float x1, float y1 ,float x2, float y2, float *dx, float *dy);
void norm(float x, float y ,float *nx, float *ny);
void dist(float x1, float y1 ,float x2, float y2, float *d);
void dir(float x1, float y1 ,float x2, float y2, float *dx, float *dy);
void heading(float x, float y, float *h);

struct enemy *get_enemy_at(float x, float y, 
													 struct enemy *enemies, int nenemies);

void init_player(struct player *player);
void init_enemy(struct enemy *enemy);


void update_player(struct player *player, 
									 struct attacker *attackers, int max_attackers);
void update_enemies(struct enemy *enemies, int nenemies,
										struct player *player,
										struct attacker *attackers, int max_attackers);
void update_enemy(struct enemy *enemy);
void update_attacker(struct attacker *attacker,
										 struct player *player, 
										 struct enemy *enemies, int nenemies);

void print_enemy(struct enemy *enemy);
void print_player(struct player *player);

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies);


#endif
