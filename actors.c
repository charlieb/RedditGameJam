#include "actors.h"

#define MAX(x,y) ((x) > (y) ? (x) : (y))

void mag(float x, float y, float *mag)
{
	*mag = sqrt(dx*dx + dy*dy);
}

void diff(float x1, float y1 ,float x2, float y2, float *dx, float *dy)
{
	*dx = x2 - x1;
	*dy = y2 - y1;
}

void norm(float x, float y ,float *nx, float *ny)
{
	float mag; 
	mag(x, y, &mag);
	*nx = x / mag;
	*ny = y / mag;
}

void dist(float x1, float y1 ,float x2, float y2, float *d)
{
	float dx, dy;
	diff(x1, y1, x2, y2, &dx, &dy);
	mag(dx, dy, d);
}

void update_player(struct player *player)
{
	player->x += player->vx;
	player->y += player->vy;
}

void update_enemy(struct enemy *enemy)
{
	enemy->x += enemy->vx;
	enemy->y += enemy->vy;
}

void update_attacker(struct attacker *attacker)
{
	attacker->x += attacker->vx;
	attacker->y += attacker->vy;
}

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies)
{
	int i, j;
	float d, dx, dy, min_dist, nx, ny;
	for(i = 0; i < nenemies; ++i) {
		diff(player->x, player->y,
				 enemies[i].x, enemies[i].y,
				 &dx, &dy);
		mag(dx, dy, &d);
		min_dist = MAX(player->size, enemies[i].size);
		norm(dx, dy, &nx, &ny);
		if(d < min_dist) {
			player->x -= min_dist * nx / 2;
			player->y -= min_dist * ny / 2;
			enemies[i].x += min_dist * nx / 2;
			enemies[i].y += min_dist * ny / 2;
		}
	}

	/*
	for(i = 0; i < nenemies; ++i) {
		for(j = i; j < nenemies; ++j) {
			
		}
	}
	*/
}
