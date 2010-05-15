#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>

#include "actors.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define CIRCLE_SEGMENTS 50

static GLfloat pos[4]   = {5.0, 5.0, 10.0, 0.0};
static GLfloat red[4]   = {0.8, 0.1, 0.0, 1.0};
static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
static GLfloat blue[4]  = {0.2, 0.2, 1.0, 1.0};
static GLfloat grey[4]  = {0.7, 0.7, 0.7, 1.0};
static GLfloat black[4] = {0.7, 0.7, 0.7, 1.0};


static void circle(int segments, float radius)
{
	GLfloat angle;
	int i, j;

  glShadeModel(GL_FLAT);
	
  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
	for(j = 0; j < 3; ++j) {  
		glBegin(GL_LINE_LOOP);
		for(i = 0; i <= segments; i++) {
			angle = i * 2.0 * M_PI / segments;
			glVertex3f(radius * cos(angle), radius * sin(angle), j * 0.01);
		}
		glEnd();
	}
	
}

void generate_graphics(struct player *player, 
											 struct enemy *enemies, int nenemies,
											 struct attacker *attackers, int max_attackers)
{
	int i;

	player->lines = glGenLists(1 + nenemies + max_attackers);
	glNewList(player->lines, GL_COMPILE);
	circle(CIRCLE_SEGMENTS, player->size);
	glEndList();		

	for(i = 0; i < nenemies; ++i) {
		enemies[i].lines = player->lines + i;
		glNewList(enemies[i].lines, GL_COMPILE);
		circle(CIRCLE_SEGMENTS, enemies[i].size);
		glEndList();
	}

	for(i = 0; i < max_attackers; ++i) {
		attackers[i].lines = player->lines + nenemies + i;
		glNewList(attackers[i].lines, GL_COMPILE);
		circle(CIRCLE_SEGMENTS, 0.125);
		glEndList();
	}
	

}

static void draw_player(struct player *player)
{
	/*print_player(player);*/
	glPushMatrix();
	glTranslatef(player->x, player->y, 0.01);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	/* glRotatef(angle, 0.0, 0.0, 1.0); */
	glCallList(player->lines);
	glPopMatrix();
}
												
static void draw_enemy(struct enemy *enemy)
{
	/*print_enemy(&(enemies[i]));*/
	glPushMatrix();
	glTranslatef(enemy->x, enemy->y, 0.01);
	/* glRotatef(angle, 0.0, 0.0, 1.0); */
	if(enemy->under_attack)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	else
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grey);
	glCallList(enemy->lines);
	glPopMatrix();
}

static void draw_attacker(struct attacker *attacker)
{
	/*print_enemy(&(enemies[i]));*/
	glPushMatrix();
	glTranslatef(attacker->x, attacker->y, 0.01);
	glScalef(0.1, 0.1, 0.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	glCallList(attacker->lines);
	glPopMatrix();
}

static void draw(struct player *player, 
								 struct enemy *enemies, int nenemies,
								 struct attacker *attackers, int max_attackers)
{
	int i;
	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw the ground plane */
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
	glNormal3f(0.0, 0.0, -1.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
  glBegin(GL_QUADS);
	{
		glVertex2f(-1000.0, 1000.0);
		glVertex2f(-1000.0, -1000.0);
		glVertex2f(1000.0, -1000.0);
		glVertex2f(1000.0, 1000.0);
	}
  glEnd();
	glPopMatrix();	

	draw_player(player);

	for(i = 0; i < nenemies; ++i) {
		if(enemies[i].health <= 0) continue;		 
		draw_enemy(&enemies[i]);
	}

	for(i = 0; i < max_attackers; ++i) {
		if(!attackers[i].alive) continue;		 
		draw_attacker(&attackers[i]);
	}

  SDL_GL_SwapBuffers();

}

/* new window size or exposure */
static void reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -h, h, -60.0, 60.0);
	/*glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0); */
	
	/* glFrustum(-h, h, -h, h, 5.0, 60.0); */
	/* glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0); */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
	/*glTranslatef(0.0, -35.0, -40.0);*/
	glRotatef(35.264f, 1.0f, 0.0f, 0.0f);
	/*glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);*/

}

static void init(struct player *player, 
								 struct enemy *enemies, int nenemies,
								 struct attacker *attackers, int max_attackers)
{

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

	generate_graphics(player, enemies, nenemies, attackers, max_attackers);

  glEnable(GL_NORMALIZE);

	/*
  if (argc > 1 && strcmp(argv[1], "-info")==0) {
		printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
		printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
		printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
		printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }
	*/
}


static float game_speed = 1.0;
float get_game_speed()
{
	return game_speed;
}

void calc_game_speed()
{
	static int frames = 0;
	static GLint start_time = 0;
 
	frames++;
	
	GLint t = SDL_GetTicks();
	if (t - start_time >= 1000) {
		GLfloat seconds = (t - start_time) / 1000.0;
		GLfloat fps = frames / seconds;
		start_time = t;
		frames = 0;
		game_speed = 60.0 / fps;
		printf("%d frames in %g seconds = %g FPS: speed: %f\n",
					 frames, seconds, fps, game_speed);
	}
}

void get_gl_pos(int x, int y, double *glx, double *gly, double *glz)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winx, winy, winz;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winx = (float)x;
	winy = (float)viewport[3] - (float)y;
	glReadPixels(x, winy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);

	gluUnProject(winx, winy, winz,
							 modelview, projection, viewport,
							 glx, gly, glz);

}


void run(struct player *player, 
				 struct enemy *enemies, int nenemies,
				 struct attacker *attackers, int max_attackers)
{
  SDL_Surface *screen;
  int done;
  Uint8 *keys;
	double glx, gly, glz;

  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Title", "title");

  init(player, enemies, nenemies, attackers, max_attackers);
  reshape(screen->w, screen->h);
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    while ( SDL_PollEvent(&event) ) {
      switch(event.type) {
			case SDL_VIDEORESIZE:
				screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
																	SDL_OPENGL|SDL_RESIZABLE);
				if ( screen ) {
					reshape(screen->w, screen->h);
				}
				break;
				
			case SDL_QUIT:
				done = 1;
				break;
      case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					exit(0);
				default:
					break;
				}
			case SDL_MOUSEBUTTONDOWN:
				printf("Mouse button %d pressed at (%d,%d)\n",
							 event.button.button, event.button.x, event.button.y);
				if(1 == event.button.button) {
					get_gl_pos(event.button.x, event.button.y,
										 &glx, &gly, &glz);
					printf("%lf, %lf, %lf\n", glx, gly, glz);
					player->destx = (float)glx;
					player->desty = (float)gly;

					player->target = get_enemy_at(player->destx, player->desty,
																				enemies, nenemies);

				}
				else if(3 == event.button.button) {
					player->far_attack = TRUE;

					get_gl_pos(event.button.x, event.button.y,
										 &glx, &gly, &glz);
					printf("%lf, %lf, %lf\n", glx, gly, glz);
					player->far_targetx = (float)glx;
					player->far_targety = (float)gly;
				}
				print_player(player);
				
				break;
			default:
				break;
      }
		}

		keys = SDL_GetKeyState(NULL);

		if ( keys[SDLK_ESCAPE] ) {
			done = 1;
		}

		/*
		int x, y;
		SDL_GetMouseState(&x, &y);
		double glx, gly, glz;
		get_gl_pos(x, y, &glx, &gly, &glz);
		printf("%lf, %lf, %lf\n", glx, gly, glz);
		player->x = (float)glx;
		player->y = (float)gly;
		print_player(player);
		*/

		calc_game_speed();
		
		update_player(player, attackers, max_attackers);
		update_enemies(enemies, nenemies);
		update_attackers(attackers, max_attackers,
										 player, enemies, nenemies);
		apply_exclusion(player, enemies, nenemies);

		draw(player, enemies, nenemies, attackers, max_attackers);
	}

	SDL_Quit();
}

void test()
{
	struct player player;
	int nenemies = 10, i;
	struct enemy enemies[nenemies];
	int max_attackers = 2000;
	struct attacker attackers[max_attackers];

	memset(enemies, 0, nenemies * sizeof(struct enemy));
	memset(&player, 0, sizeof(struct player));
	memset(attackers, 0, max_attackers * sizeof(struct attacker));

	init_player(&player);
	for(i = 0; i < nenemies; ++i) {
		init_enemy(&enemies[i]);
		print_enemy(&enemies[i]);
	}
	run(&player, enemies, nenemies, attackers, max_attackers);
}



int main(int argc, char *argv[])
{
	test();

	return 0;
}
