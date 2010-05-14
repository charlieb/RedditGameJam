#include "actors.h"

void mag(float x, float y, float *mag)
{
	*mag = sqrt(x*x + y*y);
}

void diff(float x1, float y1 ,float x2, float y2, float *dx, float *dy)
{
	*dx = x2 - x1;
	*dy = y2 - y1;
}

void norm(float x, float y ,float *nx, float *ny)
{
	float m; 
	mag(x, y, &m);
	*nx = x / m;
	*ny = y / m;
}

void dist(float x1, float y1 ,float x2, float y2, float *d)
{
	float dx, dy;
	diff(x1, y1, x2, y2, &dx, &dy);
	mag(dx, dy, d);
}

struct enemy *get_enemy_at(float x, float y, 
													 struct enemy *enemies, int nenemies)
{
	int i;
	float dx, dy;
	for(i = 0; i < nenemies; ++i) {
		diff(x, y, enemies[i].x, enemies[i].y, &dx, &dy);
		if(enemies[i].size * enemies[i].size >= dx*dx + dy*dy)
			return &enemies[i];
	}
	return NULL;
}

void init_player(struct player *player)
{
	player->size = 0.5;
	player->destx = player->desty = 0.0;
	player->health = 20;
	player->close_attack_range = 0.00001;
	player->attack_duration = 333; /* 1/3 secs */
}

void init_enemy(struct enemy *enemy)
{
	enemy->x = 5.0 - 10.0 * (float)rand() / (float) RAND_MAX;
	enemy->y = 5.0 - 10.0 * (float)rand() / (float) RAND_MAX;
	enemy->size = 0.125 + 0.125 * (float)rand() / (float) RAND_MAX;
	enemy->health = 2;
}

void player_attack(struct player *player)
{
	player->target->health -= 1;
	print_enemy(player->target);
}

void update_player(struct player *player)
{
	float dx, dy, n;

	player->x += player->vx;
	player->y += player->vy;

	/* Chase the target */
	if(player->target) {
		player->destx = player->target->x;
		player->desty = player->target->y;
	}

	diff(player->x, player->y, 
			 player->destx, player->desty, 
			 &dx, &dy);

	mag(dx, dy, &n);
	if(n > PLAYER_SPEED)
		norm(dx, dy, &dx, &dy);
	else 
		dx = dy = 0.0;

	if(player->target && player->target->health > 0)
		printf("Target Range: %f\n", n);

	if(player->target && 
		 player->target->health > 0 &&
		 n <= player->close_attack_range + player->size + player->target->size) {
		if(!player->attack_started)
			player->attack_started = SDL_GetTicks();
		else if(SDL_GetTicks() > player->attack_started + player->attack_duration) {
			player_attack(player);
			player->attack_started = 0;
			player->target = NULL;
		}
	}
	
	player->vx = dx * PLAYER_SPEED;
	player->vy = dy * PLAYER_SPEED;
	/*print_player(player);*/
}

void update_enemies(struct enemy *enemies, int nenemies)
{
	int i;
	for(i = 0; i < nenemies; ++i)
		update_enemy(&enemies[i]);
}

void update_enemy(struct enemy *enemy)
{
	if(enemy->health <= 0) {
		return;
	}

	enemy->x += enemy->vx;
	enemy->y += enemy->vy;
}

void update_attacker(struct attacker *attacker)
{
	attacker->x += attacker->vx;
	attacker->y += attacker->vy;
}

void print_enemy(struct enemy *enemy)
{
	printf("Enemy:\n"
				 "\tPos: %f, %f\n"
				 "\tVel: %f, %f\n"
				 "\tSize: %f\n"
				 "\tHealth: %i\n",
				 enemy->x, enemy->y,
				 enemy->vx, enemy->vy,
				 enemy->size, enemy->health);
}

void print_player(struct player *player)
{
	printf("Player:\n"
				 "\tPos: %f, %f\n"
				 "\tVel: %f, %f\n"
				 "\tDest: %f, %f\n"
				 "\tTarget: %p\n"
				 "\tSize: %f\n"
				 "\tHealth: %i\n",
				 player->x, player->y,
				 player->vx, player->vy,
				 player->destx, player->desty,
				 player->target,
				 player->size, player->health);
}

void apply_exclusion(struct player *player,
										 struct enemy *enemies, int nenemies)
{
	int i, j;
	float d, dx, dy, min_dist, nx, ny;
	float px, py, ex, ey;
	for(i = 0; i < nenemies; ++i) {
		if(enemies[i].health <= 0) continue;
		diff(player->x, player->y,
				 enemies[i].x, enemies[i].y,
				 &dx, &dy);
		mag(dx, dy, &d);
		min_dist = player->size + enemies[i].size;
		norm(dx, dy, &nx, &ny);
		if(d < min_dist / 2) {
			px = enemies[i].x - min_dist * nx / 2;
			py = enemies[i].y - min_dist * ny / 2;

			player->x = px;
			player->y = py;
		}
	}

	for(i = 0; i < nenemies - 1; ++i) {
		if(enemies[i].health <= 0) continue;
		for(j = i + 1; j < nenemies; ++j) {
		if(enemies[j].health <= 0) continue;
			diff(enemies[i].x, enemies[i].y,
					 enemies[j].x, enemies[j].y,
				 &dx, &dy);
			mag(dx, dy, &d);
			min_dist = enemies[i].size + enemies[j].size;
			norm(dx, dy, &nx, &ny);
			if(d < min_dist) {
				px = enemies[j].x - min_dist * nx;
				py = enemies[j].y - min_dist * ny;
				ex = enemies[i].x + min_dist * nx;
				ey = enemies[i].y + min_dist * ny;
				
				enemies[i].x = px;
				enemies[i].y = py;
				enemies[j].x = ex;
				enemies[j].y = ey;
			}	
		}
	}
}
