/*                                                               */
/* This test is based on nurbshowto_curves.cpp by Dave Griffiths */
/* Copyright (C) 2005 Dave Griffiths, GNU GPL license            */
/*                                                               */
/* // Mike Gorchak, 2009. GLU ES test                            */
/*                                                               */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengles.h>

#define __USE_SDL_GLES__
#include "glues.h"

/* screen width, height, and bit depth */
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

int window_width=WINDOW_WIDTH;
int window_height=WINDOW_HEIGHT;

#define order    4          /* make a cubic spline                                  */
#define cvcount  10         /* number of cvs                                        */
#define stride   3          /* just refers to the number of float values in each cv */
#define numknots (cvcount+order)
#define numcvs   (cvcount*stride)

// our globals
float knots[numknots];      /* somewhere to keep the knots   */
float cvs[numcvs];          /* somewhere to keep the cvs     */
int frame=0;                /* for cheezy animation purposes */

GLUnurbsObj* mynurbs=NULL;  /* the nurbs tesselator (or rather a pointer to what will be it) */

void init_scene(int width, int height)
{
   int knot;

   /* Setup our viewport for OpenGL ES */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);

   /* setup a perspective projection */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustumf(-1.0f, 1.0f, -0.75f, 0.75f, 1.0f, 100.0f);

   /* set the modelview matrix */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   /* set point size for the cv rendering */
   glPointSize(4.0f);

   /* set the clear colour to black */
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   /* no lighting needed */
   glDisable(GL_LIGHTING);

   /* position the camera in the world */
   glTranslatef(0.0f, 0.0f, -1.0f);
   glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
   glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

   /* now, make the nurbs tesselator we are going to use */
   mynurbs=gluNewNurbsRenderer();

   /* these lines set how to tesselate the curve -    */
   /* simply by chopping it up into 100 line segments */
   gluNurbsProperty(mynurbs, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
   gluNurbsProperty(mynurbs, GLU_U_STEP, 100);
   gluNurbsProperty(mynurbs, GLU_DISPLAY_MODE, GLU_FILL);

   /* set up the knot vector - make it continuous */
   for (knot=0; knot<numknots; knot++)
   {
      knots[knot]=knot;
   }
}

void resize(int width, int height)
{
   /* Setup our new viewport */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);
}

void render_scene()
{
   int count=0;
   int u=0;

   glClear(GL_COLOR_BUFFER_BIT);

   /* Curve color */
   glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

   /* tells GLU we are going to describe a nurbs curve */
   gluBeginCurve(mynurbs);
      /* send it the definition and pointers to cv and knot data */
      gluNurbsCurve(mynurbs, numknots, knots, stride, cvs, order, GLU_MAP1_VERTEX_3);
   /* thats all... */
   gluEndCurve(mynurbs);

   /* Control points color */
   glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

   /* Enable vertex array */
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, cvs);

   /* render the control vertex positions */
   glDrawArrays(GL_POINTS, 0, cvcount);
   glDisableClientState(GL_VERTEX_ARRAY);

   /* Flush all drawings */
   glFlush();

   /* Update knots */
   for (u=0; u<cvcount; u++)
   {
      cvs[count++]=0;
      cvs[count++]=cos(5.2f*sqrt(u*u)+frame*0.01f)*0.5f;
      cvs[count++]=sin(10.0f*sqrt(u*u)+frame*0.013f)*0.5f;
   }
   frame++;
}

int main(int argc, char** argv)
{
   int status;
   SDL_WindowID window;
   SDL_GLContext glcontext=NULL;
   SDL_Event event;
   SDL_bool done=SDL_FALSE;

   status=SDL_Init(SDL_INIT_VIDEO);
   if (status<0)
   {
      fprintf(stderr, "Can't init default SDL video driver: %s\n", SDL_GetError());
      exit(-1);
   }

   /* Select first display */
   status=SDL_SelectVideoDisplay(0);
   if (status<0)
   {
      fprintf(stderr, "Can't attach to first display: %s\n", SDL_GetError());
      exit(-1);
   }

   window=SDL_CreateWindow("SDL GLU ES Nurbs Curves test",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
   if (window==0)
   {
      fprintf(stderr, "Can't create window: %s\n", SDL_GetError());
      exit(-1);
   }

   glcontext=SDL_GL_CreateContext(window);
   if (glcontext==NULL)
   {
      fprintf(stderr, "Can't create OpenGL ES context: %s\n", SDL_GetError());
      exit(-1);
   }

   status=SDL_GL_MakeCurrent(window, glcontext);
   if (status<0)
   {
      fprintf(stderr, "Can't set current OpenGL ES context: %s\n", SDL_GetError());
      exit(-1);
   }

   init_scene(window_width, window_height);

   do {
      /* handle the events in the queue */
      while (SDL_PollEvent(&event))
      {
         switch(event.type)
         {
            case SDL_WINDOWEVENT:
                 switch (event.window.event)
                 {
                    case SDL_WINDOWEVENT_CLOSE:
                         done=SDL_TRUE;
                         break;
                    case SDL_WINDOWEVENT_RESIZED:
                         resize(event.window.data1, event.window.data2);
                         break;
                 }
                 break;
            case SDL_KEYDOWN:
                 switch (event.key.keysym.sym)
                 {
                    case SDLK_ESCAPE:
                         done=SDL_TRUE;
                         break;
                 }
                 break;
            case SDL_QUIT:
                 done=SDL_TRUE;
                 break;
         }
      }

      if (done==SDL_TRUE)
      {
         break;
      }

      render_scene();
      SDL_GL_SwapWindow(window);
   } while(1);

   /* Destroy NURBS renderer */
   gluDeleteNurbsRenderer(mynurbs);

   SDL_GL_DeleteContext(glcontext);
   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
