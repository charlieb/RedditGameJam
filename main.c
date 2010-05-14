#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include <SDL.h>

#include "actors.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define CIRCLE_SEGMENTS 50

static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};
static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0};
static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0};

static void circle(int segments, float radius)
{
	GLfloat angle;

  glShadeModel(GL_FLAT);
	
  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i <= segments; i++) {
    angle = i * 2.0 * M_PI / segments;
    glVertex2f(radius * cos(angle), radius * sin(angle));
  }
  glEnd();
	
}

void generate_graphics(struct player *player, 
											 struct enemy *enemies, int nenemies)
{
	int i;

	player->lines = glGenLists(1 + nenemies);
	glNewList(player->lines, GL_COMPILE);
	circle(CIRCLE_SEGMENTS, player->size);
	glEndList();		

	for(i = 0; i < nenemies; ++i) {
		enemies[i].lines = player->lines + i;
		glNewList(enemies[i].lines, GL_COMPILE);
		circle(CIRCLE_SEGMENTS, enemies[i].size);
		glEndList();
	}
}

static void draw(struct player *player, 
								 struct enemy *enemies, int nenemies)
{
	int i;
	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
		glPushMatrix();
		glRotatef(view_rotx, 1.0, 0.0, 0.0);
		glRotatef(view_roty, 0.0, 1.0, 0.0);
		glRotatef(view_rotz, 0.0, 0.0, 1.0);
	*/

  /* glPushMatrix();
		 glTranslatef(-3.0, -2.0, 0.0);
	*/
	
	/*print_player(player);*/
	glPushMatrix();
	glTranslatef(player->x, player->y, 0.0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	/* glRotatef(angle, 0.0, 0.0, 1.0); */
	glCallList(player->lines);
	glPopMatrix();

	for(i = 0; i < nenemies; ++i) {
		if(enemies[i].health <= 0) continue;		 
		/*print_enemy(&(enemies[i]));*/
		glPushMatrix();
		glTranslatef(enemies[i].x, enemies[i].y, 0.0);
		/* glRotatef(angle, 0.0, 0.0, 1.0); */
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		glCallList(enemies[i].lines);
		glPopMatrix();
	}
	/*
	glPopMatrix();
	*/
	/*
		glPopMatrix();
	*/

  SDL_GL_SwapBuffers();

}

/* new window size or exposure */
static void reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	/* glFrustum(-h, h, -h, h, 5.0, 60.0); */
	/* glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0); */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

static void init(struct player *player, 
								 struct enemy *enemies, int nenemies)
{

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

	generate_graphics(player, enemies, nenemies);

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
		printf("%d frames in %g seconds = %g FPS\n", frames, seconds, fps);
		start_time = t;
		frames = 0;
		game_speed = 60.0 / fps;
	}
}

void run(struct player *player, 
				 struct enemy *enemies, int nenemies)
{
  SDL_Surface *screen;
  int done;
  Uint8 *keys;

  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(300, 300, 16, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Title", "title");

  init(player, enemies, nenemies);
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
				} else {
					/* Uh oh, we couldn't set the new video mode?? */;
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
				player->destx = (- screen->w/2 + event.button.x) / 40.0;
				player->desty = (screen->h/2 - event.button.y) / 40.0;
				
				player->target = get_enemy_at(player->destx, player->desty,
																			enemies, nenemies);
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

		calc_game_speed();
		
		update_player(player);
		update_enemies(enemies, nenemies);
		apply_exclusion(player, enemies, nenemies);

		draw(player, enemies, nenemies);
	}

	SDL_Quit();
}

void test()
{
	struct player player;
	int nenemies = 200, i;
	struct enemy enemies[nenemies];

	memset(enemies, 0, nenemies * sizeof(struct enemy));
	memset(&player, 0, sizeof(struct player));

	init_player(&player);
	for(i = 0; i < nenemies; ++i) {
		init_enemy(&enemies[i]);
		print_enemy(&enemies[i]);
	}
	run(&player, enemies, nenemies);
}



int main(int argc, char *argv[])
{
	test();

	return 0;
}
