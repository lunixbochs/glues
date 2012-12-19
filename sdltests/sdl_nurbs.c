/*                                                              */
/* This test is based on Mark J. Kilgard's molehill example     */
/*                                                              */
/* // Mike Gorchak, 2009. GLU ES test                           */
/*                                                              */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengles.h>

#define __USE_SDL_GLES__
#include "glues.h"

/* screen width, height, and bit depth */
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

int window_width=WINDOW_WIDTH;
int window_height=WINDOW_HEIGHT;

GLfloat mat_red_diffuse[]={0.7f, 0.0f, 0.1f, 1.0f};
GLfloat mat_green_diffuse[]={0.0f, 0.7f, 0.1f, 1.0f};
GLfloat mat_blue_diffuse[]={0.0f, 0.1f, 0.7f, 1.0f};
GLfloat mat_yellow_diffuse[]={0.7f, 0.8f, 0.1f, 1.0f};
GLfloat mat_ambient[4]={0.2f, 0.2f, 0.2f, 0.2f};
GLfloat mat_specular[]={1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[]={100.0f};
GLfloat knots[8]={0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat pts1[4][4][3], pts2[4][4][3];
GLfloat pts3[4][4][3], pts4[4][4][3];
GLUnurbsObj* nurb;

void init_scene(int width, int height)
{
   int u, v;

   /* Setup our viewport for OpenGL ES */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);

   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);

   /* Since GL ES has no GL_AUTO_NORMAL, we implement it in GLU */
   gluEnable(GLU_AUTO_NORMAL);

   nurb=gluNewNurbsRenderer();
   gluNurbsProperty(nurb, GLU_SAMPLING_TOLERANCE, 25.0f);
   gluNurbsProperty(nurb, GLU_DISPLAY_MODE, GLU_FILL);

   /* Build control points for NURBS mole hills. */
   for(u=0; u<4; u++)
   {
      for(v=0; v<4; v++)
      {
         /* Red. */
         pts1[u][v][0]=2.0f*((GLfloat)u);
         pts1[u][v][1]=2.0f*((GLfloat)v);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            /* Stretch up middle. */
            pts1[u][v][2]=6.0f;
         }
         else
         {
            pts1[u][v][2]=0.0f;
         }

         /* Green. */
         pts2[u][v][0]=2.0f*((GLfloat)u-3.0f);
         pts2[u][v][1]=2.0f*((GLfloat)v-3.0f);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            if (u==1 && v==1)
            {
               /* Pull hard on single middle square. */
               pts2[u][v][2]=15.0f;
            }
            else
            {
                /* Push down on other middle squares. */
                pts2[u][v][2]=-2.0f;
            }
         }
         else
         {
            pts2[u][v][2]=0.0f;
         }

         /* Blue. */
         pts3[u][v][0]=2.0f*((GLfloat)u-3.0f);
         pts3[u][v][1]=2.0f*((GLfloat)v);
         if ((u==1 || u==2) && (v==1 || v==2))
         {
            if (u==1 && v==2)
            {
               /* Pull up on single middple square. */
               pts3[u][v][2]=11.0f;
            }
            else
            {
               /* Pull up slightly on other middle squares. */
               pts3[u][v][2]=2.0f;
            }
         }
         else
         {
            pts3[u][v][2]=0.0f;
         }

         /* Yellow. */
         pts4[u][v][0]=2.0f*((GLfloat)u);
         pts4[u][v][1]=2.0f*((GLfloat)v-3.0f);
         if ((u==1 || u==2 || u==3) && (v==1 || v==2))
         {
            if (v==1)
            {
               /* Push down front middle and right squares. */
               pts4[u][v][2]=-2.0f;
            }
            else
            {
               /* Pull up back middle and right squares. */
               pts4[u][v][2]=5.0f;
            }
         }
         else
         {
            pts4[u][v][2]=0.0f;
         }
      }
   }

   /* Stretch up red's far right corner. */
   pts1[3][3][2]=6;
   /* Pull down green's near left corner a little. */
   pts2[0][0][2]=-2;
   /* Turn up meeting of four corners. */
   pts1[0][0][2]=1;
   pts2[3][3][2]=1;
   pts3[3][0][2]=1;
   pts4[0][3][2]=1;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(55.0f, (GLfloat)width/(GLfloat)height, 2.0f, 24.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0f, 0.0f, -15.0f);
   glRotatef(330.0f, 1.0f, 0.0f, 0.0f);
}

void resize(int width, int height)
{
   /* Avoid division by zero */
   if (height==0)
   {
      height=1;
   }

   /* Setup our new viewport for OpenGL ES */
   glViewport(0, 0, (GLint)width, (GLint)height);
   /* Setup our new viewport for GLU ES (required when using OpenGL ES 1.0 only) */
   gluViewport(0, 0, (GLint)width, (GLint)height);

   /* Setup new aspect ratio */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(55.0f, (GLfloat)width/(GLfloat)height, 2.0f, 24.0f);
   glMatrixMode(GL_MODELVIEW);
}

void render_scene()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* Render red hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_red_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts1[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render green hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_green_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts2[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render blue hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_blue_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts3[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Render yellow hill. */
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_yellow_diffuse);
   gluBeginSurface(nurb);
      gluNurbsSurface(nurb, 8, knots, 8, knots,
                      4 * 3, 3, &pts4[0][0][0],
                      4, 4, GLU_MAP2_VERTEX_3);
   gluEndSurface(nurb);

   /* Flush all drawings */
   glFlush();
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

   window=SDL_CreateWindow("SDL GLU ES Nurbs test",
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
   gluDeleteNurbsRenderer(nurb);

   SDL_GL_DeleteContext(glcontext);
   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
