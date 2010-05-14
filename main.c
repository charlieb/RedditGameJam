#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>

#include <SDL.h>

#include "actors.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif

static void circle(int segments)
{
	GLfloat angle;

  glShadeModel(GL_FLAT);
	
  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i <= segments; i++) {
    angle = i * 2.0 * M_PI / segments;
    glVertex2f(cos(angle), sin(angle));
  }
  glEnd();
	
}


static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint circle_list;
static GLfloat angle = 0.0;


static void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);
	*/

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(circle_list);
  glPopMatrix();

	/*
  glPopMatrix();
	*/

  SDL_GL_SwapBuffers();

  /* Frames++; */
  /* { */
  /*    GLint t = SDL_GetTicks(); */
  /*    if (t - T0 >= 5000) { */
  /*       GLfloat seconds = (t - T0) / 1000.0; */
  /*       GLfloat fps = Frames / seconds; */
  /*       printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps); */
  /*       T0 = t; */
  /*       Frames = 0; */
  /*    } */
  /* } */
}

/* new window size or exposure */
static void reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}
static void idle(void)
{
  angle += 2.0;
}

static void init(int argc, char *argv[])
{
  static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};
  static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0};
  static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
  static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  circle_list = glGenLists(1);
  glNewList(circle_list, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  circle(10);
  glEndList();

  glEnable(GL_NORMALIZE);

  if (argc > 1 && strcmp(argv[1], "-info")==0) {
     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
     printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }
}


int main(int argc, char *argv[])
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
  SDL_WM_SetCaption("Gears", "gears");

  init(argc, argv);
  reshape(screen->w, screen->h);
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    idle();
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
      }
    }
    keys = SDL_GetKeyState(NULL);

    if ( keys[SDLK_ESCAPE] ) {
      done = 1;
    }
    if ( keys[SDLK_UP] ) {
      view_rotx += 5.0;
    }
    if ( keys[SDLK_DOWN] ) {
      view_rotx -= 5.0;
    }
    if ( keys[SDLK_LEFT] ) {
      view_roty += 5.0;
    }
    if ( keys[SDLK_RIGHT] ) {
      view_roty -= 5.0;
    }
    if ( keys[SDLK_z] ) {
      if ( SDL_GetModState() & KMOD_SHIFT ) {
        view_rotz -= 5.0;
      } else {
        view_rotz += 5.0;
      }
    }

    draw();
  }
  SDL_Quit();
  return 0;             /* ANSI C requires main to return int. */
}

void test()
{
	
}
