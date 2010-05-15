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

void dir(float x1, float y1 ,float x2, float y2, float *dx, float *dy)
{
	diff(x1, y1, x2, y2, dx, dy);
	norm(*dx, *dy, dx, dy);
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


void print_attacker(struct attacker *attacker)
{
	printf("Attacker:\n"
				 "\tPos: %f, %f\n"
				 "\tDest: %f, %f\n"
				 "\tVel: %f, %f\n",
				 attacker->x, attacker->y,
				 attacker->destx, attacker->desty,
				 attacker->vx, attacker->vy);
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

struct attacker *find_unused_attacker(struct attacker *attackers, 
																			int max_attackers)
{
	int i;
	for(i = 0; i < max_attackers; ++i)
		if(!attackers[i].alive)
			return &attackers[i];
	return NULL;
}

void init_player(struct player *player)
{
	player->size = 0.05;
	player->destx = player->desty = 0.0;
	player->health = 20;
	player->close_attack_range = 0.00001;
	player->attack_duration = 100; /* 1/10 secs */
}

void init_enemy(struct enemy *enemy)
{
	enemy->x = 0.5 - 1.0 * (float)rand() / (float) RAND_MAX;
	enemy->y = 0.5 - 1.0 * (float)rand() / (float) RAND_MAX;
	enemy->size = 0.0125 + 0.0125 * (float)rand() / (float) RAND_MAX;
	enemy->health = 2;
}

void player_attack(struct player *player)
{
	player->target->health -= 1;
	print_enemy(player->target);
}

void update_player(struct player *player,
									 struct attacker *attackers, int max_attackers)
{
	float dx, dy, n;
	struct attacker *attacker = NULL;

	player->x += player->vx;
	player->y += player->vy;

	/* Chase the target */
	if(player->target) {
		player->destx = player->target->x;
		player->desty = player->target->y;
	}

	/* how far from our destination are we? */
	diff(player->x, player->y, 
			 player->destx, player->desty, 
			 &dx, &dy);
	/* are we there yet? */
	mag(dx, dy, &n);
	if(n > PLAYER_SPEED)
		norm(dx, dy, &dx, &dy);
	else 
		dx = dy = 0.0;

	if(player->target && player->target->health > 0)
		printf("Target Range: %f\n", n);

	/* do we have a close range attack to carry out */
	if(player->target && 
		 player->target->health > 0 &&
		 n <= player->close_attack_range + player->size + player->target->size) {
		if(!player->attack_started) {
			player->attack_started = SDL_GetTicks();
			player->target->under_attack = TRUE;
		}
		else if(SDL_GetTicks() > player->attack_started + player->attack_duration) {
			player_attack(player);
			player->attack_started = FALSE;
			player->target->under_attack = FALSE;
			player->target = NULL;
		}
	}
	
	/* do we have a long range attack to launch */
	if(player->far_attack) {
		attacker = find_unused_attacker(attackers, max_attackers);
		
		attacker->alive = TRUE;
		attacker->x = player->x;
		attacker->y = player->y;
		attacker->vx = player->vx;
		attacker->vy = player->vy;
		attacker->destx = player->far_targetx;
		attacker->desty = player->far_targety;
		attacker->damage = 2;
		/* can't attack yourself */
		attacker->player = TRUE;
		/* we initiated the attack so stop */
		player->far_attack = FALSE;
	}

	/* have we been hit */
	if(player->far_hit_by) {
		player->far_hit_started = SDL_GetTicks();
	}

	/* finally update the player position */
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
	if(enemy->health <= 0) return;
	
	if(enemy->under_attack && enemy->far_hit_by) {
		printf("%i vs %i\n", 
					 SDL_GetTicks(),
					 enemy->far_hit_started + ATTACK_DISPLAY_DURATION);
		
		if(SDL_GetTicks() > enemy->far_hit_started + ATTACK_DISPLAY_DURATION) {
			enemy->under_attack = FALSE;
			enemy->health -= enemy->far_hit_by->damage;
			enemy->far_hit_by->alive = FALSE;
			enemy->far_hit_by = NULL;
		}
	}
			
	enemy->x += enemy->vx;
	enemy->y += enemy->vy;
}

void update_attacker(struct attacker *attacker,
										 struct player *player, 
										 struct enemy *enemies, int nenemies)
{
	int i;
	float d, dx, dy, m, dvx, dvy;

	if(!attacker->player) {
		dist(player->x, player->y, attacker->x, attacker->y, &d);
		if(d <= player->size) {
			player->far_hit_by = attacker;
			player->far_hit_started = SDL_GetTicks();
			printf("Reached player\n");
			attacker->alive = FALSE;
			return;
		}
	}
	else 
		for(i = 0; i < nenemies; ++i) {
			if(enemies[i].health <= 0) continue;
			dist(enemies[i].x, enemies[i].y, attacker->x, attacker->y, &d);
			if(d <= enemies[i].size) {
				enemies[i].far_hit_by = attacker;
				enemies[i].far_hit_started = SDL_GetTicks();
				enemies[i].under_attack = TRUE;
				printf("Reached enemy\n");
				print_enemy(&enemies[i]);
				attacker->alive = FALSE;
				return;
			}
		}

	/* we haven't hit anything */
	
	diff(attacker->x, attacker->y,
			 attacker->destx, attacker->desty,
			 &dx, &dy);
	mag(dx, dy, &m);
	if(m <= MIN_DIST) {
		printf("Reached destimation\n");
		attacker->alive = FALSE;
		return;
	}

	if(m > ATTACKER_SPEED) {
		norm(dx, dy, &dx, &dy);
		dx *= ATTACKER_SPEED;
		dy *= ATTACKER_SPEED;
	}
	attacker->vx = dx;
	attacker->vy = dy;
	
	print_attacker(attacker);
	
	attacker->x += attacker->vx;
	attacker->y += attacker->vy;
}

void update_attackers(struct attacker *attackers, int max_attackers,
											struct player *player, 
											struct enemy *enemies, int nenemies)
{
	int i;
	for(i = 0; i < max_attackers; ++i)
		if(attackers[i].alive)
			update_attacker(&attackers[i], player, enemies, nenemies);
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
