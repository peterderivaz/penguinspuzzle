// Code from Raspberry Pi forums on example OpenGLES fullscreen application

//#include <GL/gl.h>
#include "gbm.h"
#include  <GLES2/gl2.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#include "drm_gbm.h"

#include <EGL/egl.h>

#define EXIT(msg) { fputs (msg, stderr); exit (EXIT_FAILURE); }

// global variables declarations

static EGLDisplay egl_display;
static EGLContext context;
static EGLSurface egl_surface;
       EGLConfig config;
       EGLint num_config;
       EGLint count=0;
       EGLConfig *configs;
       int config_index;
       int i;
       
    

static EGLint attributes[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 0,
		//EGL_ALPHA_SIZE, 8, // This causes a segmentation fault for some reason?
		EGL_DEPTH_SIZE, 16,
		//EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
		};

static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};


Display    *x_display;
Window      win;

int hdisplay = 800;
int vdisplay = 480;

static int match_config_to_visual(EGLDisplay egl_display, EGLint visual_id, EGLConfig *configs, int count) {

EGLint id;
for (i = 0; i < count; ++i) {
  if (!eglGetConfigAttrib(egl_display, configs[i], EGL_NATIVE_VISUAL_ID,&id)) continue;
  if (id == visual_id) return i;
  }
return -1;
}


void drm_gbm_start () {
   x_display = XOpenDisplay ( NULL );   // open the standard display (the primary screen)
   if ( x_display == NULL ) {
      fprintf(stderr, "cannot connect to X server\n");
      exit(1);
   }
 
   Window root  =  DefaultRootWindow( x_display );   // get the root window (usually the whole screen)
 
   XSetWindowAttributes  swa;
   swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
 
   win  =  XCreateWindow (   // create a window with the provided parameters
              x_display, root,
              0, 0, hdisplay, vdisplay,   0,
              CopyFromParent, InputOutput,
              CopyFromParent, CWEventMask,
              &swa );
 
   XSetWindowAttributes  xattr;
   Atom  atom;
   int   one = 1;
 
   xattr.override_redirect = False;
   XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );
 
   atom = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", True );
   XChangeProperty (
      x_display, win,
      XInternAtom ( x_display, "_NET_WM_STATE", True ),
      XA_ATOM,  32,  PropModeReplace,
      (unsigned char*) &atom,  1 );
 
   XChangeProperty (
      x_display, win,
      XInternAtom ( x_display, "_HILDON_NON_COMPOSITED_WINDOW", False ),
      XA_INTEGER,  32,  PropModeReplace,
      (unsigned char*) &one,  1);
 
   XWMHints hints;
   hints.input = True;
   hints.flags = InputHint;
   XSetWMHints(x_display, win, &hints);
 
   XMapWindow ( x_display , win );             // make the window visible on the screen
   XStoreName ( x_display , win , "GL test" ); // give the window a name
 
   //// get identifiers for the provided atom name strings
   Atom wm_state   = XInternAtom ( x_display, "_NET_WM_STATE", False );
   Atom fullscreen = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", False );
 
   XEvent xev;
   memset ( &xev, 0, sizeof(xev) );
 
   xev.type                 = ClientMessage;
   xev.xclient.window       = win;
   xev.xclient.message_type = wm_state;
   xev.xclient.format       = 32;
   xev.xclient.data.l[0]    = 1;
   xev.xclient.data.l[1]    = fullscreen;
   XSendEvent (                // send an event mask to the X-server
      x_display,
      DefaultRootWindow ( x_display ),
      False,
      SubstructureNotifyMask,
      &xev );

	//gbm_surface = gbm_surface_create (gbm_device, 640, 480, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
	egl_display = eglGetDisplay ((EGLNativeDisplayType)x_display);
	if ( egl_display == EGL_NO_DISPLAY ) {
		EXIT("Got no EGL display.");
	}
	if (!eglInitialize (egl_display, NULL ,NULL)) {
		EXIT("Failed to initialize");
	}
	eglBindAPI (EGL_OPENGL_API);
	eglGetConfigs(egl_display, NULL, 0, &count);
	configs = malloc(count * sizeof *configs);
	eglChooseConfig (egl_display, attributes, configs, count, &num_config);
	config_index = match_config_to_visual(egl_display,GBM_FORMAT_XRGB8888,configs,num_config);
	context = eglCreateContext (egl_display, configs[config_index], EGL_NO_CONTEXT, context_attribs);
	egl_surface = eglCreateWindowSurface (egl_display, configs[config_index], (EGLNativeWindowType)win, NULL);
	free(configs);
	eglMakeCurrent (egl_display, egl_surface, egl_surface, context);
}

int drm_width(void) {
	return hdisplay;
}

int drm_height(void) {
	return vdisplay;
}

void drm_gbm_swap(void) {
      while ( XPending ( x_display ) ) {   // check for events from the x-server
 
         XEvent  xev;
         XNextEvent( x_display, &xev );
 
         //if ( xev.type == KeyPress )   quit = true;
      }
      eglSwapBuffers (egl_display, egl_surface);
}

void drm_gbm_finish(void) {
   ////  cleaning up...
   eglDestroyContext ( egl_display, context );
   eglDestroySurface ( egl_display, egl_surface );
   eglTerminate      ( egl_display );
   XDestroyWindow    ( x_display, win );
   XCloseDisplay     ( x_display );
}
