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

void heading(float x, float y, float *h)
{
	if(x < 0 && y < 0)
		*h = M_PI + atan(x / -y);
	else if(x < 0 && y == 0) 
		*h = M_PI / 2;
	else if(x < 0 && y > 0) 
		*h = M_PI / 2 + atan(y / x);
		
	else if(x == 0 && y < 0) 
		*h = M_PI;
	/* Don't change anything in this case */
	/* else if(x == 0 && y == 0) */
	else if(x == 0 && y > 0) 
		*h = 0;
		
	else if(x > 0 && y > 0) 
		*h = atan(x / -y);
	else if(x > 0 && y == 0) 
		*h = 3 * M_PI / 2;
	else if(x > 0 && y < 0) 
		*h = 3 * M_PI / 2 + atan(y / x);
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
				 "\tOld: %f, %f\n"
				 "\tDest: %f, %f\n"
				 "\tTarget: %p\n"
				 "\tSize: %f\n"
				 "\tHealth: %i\n",
				 player->x, player->y,
				 player->vx, player->vy,
				 player->dx, player->dy,
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

int count_active_attackers(struct attacker *attackers, 
													 int max_attackers)
{
	int i, c = 0;
	for(i = 0; i < max_attackers; ++i) 
		if(attackers[i].alive)	
			c++;
	return c;
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
	player->near_attack_range = 0.1;
	player->far_attack_range = 1.0;
	player->far_attack_duration = 500; /* 1/10 secs */
	player->near_attack_duration = 333; /* 1/10 secs */
	player->far_attack_damage = 1;
	player->near_attack_damage = 2;
}

void init_enemy(struct enemy *enemy)
{
	enemy->x = 0.5 - 1.0 * (float)rand() / (float) RAND_MAX;
	enemy->y = 0.5 - 1.0 * (float)rand() / (float) RAND_MAX;
	enemy->vx = 0.0;
	enemy->vy = 0.0;
	enemy->size = 0.05;//0.0125 + 0.0125 * (float)rand() / (float) RAND_MAX;
	enemy->health = 2;

	enemy->turn_rate = 0.01;
	enemy->attra = 0.20;
	enemy->align = 0.20;
	enemy->chase = 0.25;
	enemy->attract_dist = 0.75;
	enemy->align_dist = 0.25;

	enemy->near_attack_range = 0.1;
	enemy->near_attack_duration = 333;
	enemy->near_attack_started = FALSE;
	enemy->near_attack_damage = 1;

}

void laser_attack(float x, float y, float vx, float vy, 
									int damage, float range, int player,
									struct attacker *attackers, int max_attackers)
{
	struct attacker *attacker = find_unused_attacker(attackers, max_attackers);

	attacker->type = laser;
	attacker->alive = TRUE;
	attacker->x = x;
	attacker->y = y;
	attacker->vx = vx;
	attacker->vy = vy;
	norm(attacker->vx, attacker->vy, &attacker->vx, &attacker->vy);
	attacker->vx *= ATTACKER_SPEED;
	attacker->vy *= ATTACKER_SPEED;
	attacker->damage = damage;
	attacker->range_remaining = range;
	attacker->player = player;
}


void missile_attack(float x, float y, 
										float vx, float vy, 
										float destx, float desty,
										int damage, float range, float turn_rate, 
										int player,
										struct attacker *attackers, int max_attackers)
{
	struct attacker *attacker = find_unused_attacker(attackers, max_attackers);

	attacker->type = missile;
	attacker->alive = TRUE;
	attacker->x = x;
	attacker->y = y;
	attacker->vx = vx;
	attacker->vy = vy;
	attacker->destx = destx;
	attacker->desty = desty;
	attacker->damage = damage;
	attacker->turn_rate = turn_rate;
	attacker->range_remaining = range;
	attacker->player = player;
}

void update_player(struct player *player,
									 struct attacker *attackers, int max_attackers)
{
	float dx, dy, n;
	struct attacker *attacker = NULL;
	Uint32 dt;

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

	/* don't chase the deaders */
	if(player->target && player->target->health <= 0)
		player->target = NULL;

	/* do we have a near range attack to carry out */
	if(player->target && n < player->near_attack_range) {
		dt = SDL_GetTicks() - player->near_attack_started;
		if(dt > player->near_attack_duration) {
			laser_attack(player->x, player->y, 
									 player->dx, player->dy,
									 player->near_attack_damage, 
									 player->near_attack_range,
									 TRUE,
									 attackers, max_attackers);
			player->near_attack_started = SDL_GetTicks();
			player->target = NULL;
		}
	}
	
	/* do we have a long range attack to launch */
	if(player->far_attack) {
		dt = SDL_GetTicks() - player->far_attack_started;
		if(dt > player->far_attack_duration) {
			missile_attack(player->x, player->y, 
										 player->dx, player->dy,
										 player->far_targetx, player->far_targety,
										 player->far_attack_damage, 
										 player->far_attack_range,
										 0.75,
										 TRUE,
										 attackers, max_attackers);
			/* fire and forget */
			player->far_attack = FALSE;
			player->far_attack_started = SDL_GetTicks();
		}
	}

	/* have we been hit */
	if(player->hit_by) {
		if(SDL_GetTicks() > player->hit_started + ATTACK_DISPLAY_DURATION) {
			player->health -= player->hit_by->damage;
			player->hit_by = NULL;
			player->hit_started = FALSE;
		}
	}

	/* update the player direction (used for drawing) */
	if(dx != 0.0 || dy != 0.0) {
		player->dx = dx;
		player->dy = dy;
	}

	/* finally update the player position */
	player->vx = dx * PLAYER_SPEED;
	player->vy = dy * PLAYER_SPEED;
	/*print_player(player);*/
}

void update_enemies(struct enemy *enemies, int nenemies,
										struct player *player,
										struct attacker *attackers, int max_attackers)
{
	int i, j, try_attack = TRUE;
	float x, y, vx, vy;
	float d, dt, attx, atty, alix, aliy, chsx, chsy, totalx, totaly;
	
	for(i = 0; i < nenemies; ++i) {
		if(enemies[i].health <= 0) continue;

		attx = atty = alix = aliy = chsx = chsy = 0.0;
		dist(enemies[i].x, enemies[i].y, player->x, player->y, &d);
		dir(enemies[i].x, enemies[i].y, player->x, player->y,
				&chsx, &chsy);

		/* if we are close to the player we do not flock */
		if(d < enemies[i].near_attack_range) {
			dt = SDL_GetTicks() - enemies[i].near_attack_started;
			if(!enemies[i].near_attack_started)
				enemies[i].near_attack_started = SDL_GetTicks();
			else if(dt > player->near_attack_duration) {
				laser_attack(enemies[i].x, enemies[i].y,
										 enemies[i].vx, enemies[i].vy,
										 enemies[i].near_attack_damage,
										 enemies[i].near_attack_range,
										 FALSE,
										 attackers, max_attackers);
				enemies[i].near_attack_started = FALSE;
			}
		}
		else {
			enemies[i].near_attack_started = FALSE;
			for(j = 0; j < nenemies; ++j) {
				if(i == j || enemies[j].health <= 0) continue;
				dist(enemies[i].x, enemies[i].y,
						 enemies[j].x, enemies[j].y, &d);
				if(d <= enemies[i].attract_dist) {
					if(d <= enemies[i].align_dist) {
						if(enemies[j].vx != 0.0 || enemies[j].vy != 0.0)
							norm(enemies[j].vx, enemies[j].vy, &x, &y);
						alix += x;
						aliy += y;
					}
					else {
						norm(enemies[j].x, enemies[j].y, &x, &y);
						attx += x;
						atty += y;
					}
				}
			}
		
			if(attx != 0.0 || atty != 0.0)
				norm(attx, atty, &attx, &atty);
			if(alix != 0.0 || aliy != 0.0)
				norm(alix, aliy, &alix, &aliy);
		}
		/*
			printf("attract: %f, %f\n"
			"align: %f, %f\n"
			"chase: %f, %f\n",
			attx, atty, alix, aliy, chsx, chsy);
		*/

		vx = 
			attx * enemies[i].attra + 
			alix * enemies[i].align + 
			chsx * enemies[i].chase; 
		vy = 
			atty * enemies[i].attra + 
			aliy * enemies[i].align + 
			chsy * enemies[i].chase;

		enemies[i].vx += vx * enemies[i].turn_rate;
		enemies[i].vy += vy * enemies[i].turn_rate;
	
		norm(enemies[i].vx, enemies[i].vy, 
				 &enemies[i].vx, &enemies[i].vy);

		enemies[i].vx *= ENEMY_SPEED;
		enemies[i].vy *= ENEMY_SPEED;

		update_enemy(&enemies[i]);
	}
}

void update_enemy(struct enemy *enemy)
{
	if(enemy->hit_by)
		if(SDL_GetTicks() > enemy->hit_started + ATTACK_DISPLAY_DURATION) {
			enemy->health -= enemy->hit_by->damage;
			enemy->hit_by = NULL;
			enemy->hit_started = FALSE;
		}
	
	enemy->x += enemy->vx;
	enemy->y += enemy->vy;

	/*print_enemy(enemy);*/
}

void update_attacker(struct attacker *attacker,
										 struct player *player, 
										 struct enemy *enemies, int nenemies)
{
	int i;
	float d, dx, dy, m;

	if(!attacker->player) {
		dist(player->x, player->y, attacker->x, attacker->y, &d);
		if(d <= player->size) {
			attacker->alive = FALSE;
			player->hit_by = attacker;
			player->hit_started = SDL_GetTicks();
			printf("Reached player\n");
			return;
		}
	}
	else 
		for(i = 0; i < nenemies; ++i) {
			if(enemies[i].health <= 0) continue;
			dist(enemies[i].x, enemies[i].y, attacker->x, attacker->y, &d);
			if(d <= enemies[i].size) {
				attacker->alive = FALSE;
				enemies[i].hit_by = attacker;
				enemies[i].hit_started = SDL_GetTicks();
				printf("Reached enemy\n");
				return;
			}
		}

	/* can we go any further */
	if(attacker->range_remaining <= 0.0) {
		printf("Missed!\n");
		attacker->alive = FALSE;
		return;
	}		

	/* we haven't hit anything */
 	if(attacker->type != laser) { /* we are a missile -> we can turn */
		diff(attacker->x, attacker->y,
				 attacker->destx, attacker->desty,
				 &dx, &dy);
		mag(dx, dy, &m);
		if(m <= MIN_DIST) {
			printf("Reached destimation\n");
			attacker->alive = FALSE;
			return;
		}

		/* we are far enough away from our dest for at least one more
			 timestep */
		if(m > ATTACKER_SPEED) {
			norm(dx, dy, &dx, &dy);
			dx *= ATTACKER_SPEED;
			dy *= ATTACKER_SPEED;

			dx = attacker->vx + dx * attacker->turn_rate;
			dy = attacker->vy + dy * attacker->turn_rate;

			norm(dx, dy, &dx, &dy);
			dx *= ATTACKER_SPEED;
			dy *= ATTACKER_SPEED;
		
		}
		attacker->vx = dx;
		attacker->vy = dy;
	}

	/*print_attacker(attacker);*/
	
	attacker->x += attacker->vx;
	attacker->y += attacker->vy;

	mag(dx, dy, &m);
	attacker->range_remaining -= m;
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
