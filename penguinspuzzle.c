/*
Copyright (c) 2012, Peter de Rivaz
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
* WebGL version at http://penguinspuzzle.appspot.com
*
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

//#include "bcm_host.h"
#include "drm_gbm.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "global.h"
#include "shaders.h"
#include "buffer.h"
#include "model.h"
#include "texture.h"
#include "matrix.h"
#include "view.h"
#include "penguins.h"
#include "keys.h"
#include "framebuffer.h"
#include "shader_src.h"
#include "audio.h"

static int frame=0;
static int high_quality=1;
static int slow_mode=0;
static LEVEL_T *level=NULL;
static FRAMEBUFFER_T *shadow_fb=NULL;
static FRAMEBUFFER_T *mirror_fb=NULL;
static SHADER_T *shadow_shader=NULL;
static SHADER_T *sun_shader=NULL;
static SHADER_T *shader=NULL;
static SHADER_T *water_shader=NULL;
static SHADER_T *expl_shader=NULL;
static VIEW_T *sun_view;
static float world[4][4];
static MODEL_T *sea,*expl2,*sky;

typedef struct
{
   //uint32_t screen_width;   // Dimensions of the physical screen
   //uint32_t screen_height;
   uint32_t render_width;   // Dimensions that we will render to
   uint32_t render_height;
// OpenGL|ES objects
   //EGLDisplay display;
   //EGLSurface surface;
   //EGLContext context;

   GLuint verbose;
   GLuint vshader;
   GLuint fshader;
   GLuint mshader;
   GLuint program;
   GLuint program2;
   GLuint tex_fb;
   GLuint tex;
   GLuint buf;
// julia attribs
   GLuint unif_color, attr_vertex, unif_scale, unif_offset, unif_tex, unif_centre; 
// mandelbrot attribs
   GLuint attr_vertex2, unif_scale2, unif_offset2, unif_centre2;
} CUBE_STATE_T;

static CUBE_STATE_T _state, *state=&_state;

#define check() assert(glGetError() == 0)

    
/***********************************************************
 * Name: init_ogl
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
static void init_ogl(CUBE_STATE_T *state)
{
#if 0
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;  // Must be static as OpenGL relies on this being persistent.

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	  EGL_DEPTH_SIZE, 16,
	  //EGL_SAMPLE_BUFFERS,  1,  // Adds antialiasing but runs slowly
      EGL_NONE
   };
   
   static const EGLint context_attributes[] = 
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);
   check();

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);
   check();

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);
   check();

   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);
   check();

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(state->context!=EGL_NO_CONTEXT);
   check();

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );
   
   state->render_width = 1280;
   state->render_height = 720;
   
   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->render_width << 16;
   src_rect.height = state->render_height << 16;    

   // Set aspect ratio
   aspect = (float)state->render_width / state->render_height;
   

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->render_width;
   nativewindow.height = state->render_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   check();

   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);
   check();

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);
   check();

#endif
   state->render_width = 1280;
   state->render_height = 720;
   aspect = 1280.0f/720.0f; // How to render to smaller surface?, how to get correct aspect ratio?

   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
   glClear( GL_COLOR_BUFFER_BIT );

   check();

}


void drawScene(void) 
{
	VIEW_T *view = level->view;
	frame++;
	
	if (currentlyPressedKeys[KEY_Q])
	{
		high_quality = !high_quality;
		currentlyPressedKeys[KEY_Q] = 0;
	}
	if (currentlyPressedKeys[KEY_S])
	{
		slow_mode = !slow_mode;
		currentlyPressedKeys[KEY_S] = 0;
	}
	if (currentlyPressedKeys[KEY_Z])
	{
		zoom_out = !zoom_out;
		currentlyPressedKeys[KEY_Z] = 0;
	}
	if (high_quality)
	{
		glColorMask(1,1,1,1);
		framebuffer_select(shadow_fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mat4_identity(world);
		shader_select(sun_shader);
		shader_view(sun_shader,sun_view->M);
		shader_world(sun_shader,world);
		level_draw_tiles(level,sun_shader);
		level_draw_objects(level,sun_shader,0);
		level_draw_objects(level,sun_shader,1);
		glFinish();
	}
	
	float br=288;
	glClearColor(129/br, 168/br, 207/br, 1.0);
	if (high_quality)
	{
		framebuffer_select(mirror_fb);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//shader.selectView(billboardMatrix());
		//sky.draw(shader);
		mat4_identity(world);
		shader_select(shader);
		shader_blend(shader,frame*0.0001f,0,0,0);
		shader_view(shader,view->M_reflect);
		shader_world(shader,world);
		model_draw(sky,shader);
		shader_select(shader);
		level_draw_objects(level,shader,0);
		level_draw_objects(level,shader,1);
		mat4_identity(world);
		shader_world(shader,world);
		level_draw_tiles(level,shader);
		glFinish();
	}
	
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0, 0, state->render_width, state->render_height);
	if (level->mode==3)
	{
		glClearColor(0, 129, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColorMask(0,1,0,1);
	}
	else	
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColorMask(1,1,1,1);
	}
	framebuffer_select_texture(shadow_fb);
	mat4_identity(world);
	SHADER_T *level_shader=shader;
	if (high_quality)
		level_shader = shadow_shader;
	shader_select(level_shader);
	shader_view(level_shader,view->M);
	shader_view2(level_shader,sun_view->M);
	shader_world(level_shader,world);
	level_draw_tiles(level,level_shader);
	shader_select(shader);
	shader_blend(shader,frame*0.0001f,0,0,0);
	shader_view(shader,view->M);
	shader_world(shader,world);
	model_draw(sky,shader);
	shader_select(shader);
	level_draw_objects(level,shader,0);
	float slow=0.4f; 
	framebuffer_select_texture(	mirror_fb);
	mat4_identity(world);
	if (high_quality)
	{
		shader_select(water_shader);
		shader_blend(water_shader,frame*0.001f*slow,frame*0.002f*slow,-frame*0.0017f*slow,-frame*0.003f*slow);
		shader_view(water_shader,view->M);
		shader_view2(water_shader,view->M_reflect);     
		shader_world(water_shader,world);
		model_draw(sea,water_shader);
	}
	// Now draw transparent objects
	shader_select(shader);
	shader_view(shader,view->M);
	if (high_quality)
	{
		glDepthMask(0);
		glBlendEquation(GL_FUNC_ADD);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE_MINUS_CONSTANT_ALPHA);
		glBlendColor(0,0,0,0.7f);
	}
	level_draw_objects(level,shader,1);
	
	if (high_quality)
	{
		//gl.blendFunc(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		shader_select(expl_shader);
		shader_view(expl_shader,view->M);
		level_draw_fragments(level,expl_shader,expl2);
	}
	glDisable(GL_BLEND);
	glDepthMask(1);
	
	glFinish();
}


void tick(void) 
{
	update_keys();
	level_update_view(level);
	level_update(level);
	if (!slow_mode)
		level_update(level);
	drawScene();
}

int tm,ts,tw,tc;

#define ALTWIDTH 512

void init_gl(void) 
{
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthRangef(-1.0f,1.0f); 
	glClearDepthf(1.0f);
	
	mirror_fb = framebuffer_new(ALTWIDTH,ALTWIDTH,1);
	shadow_fb = framebuffer_new(ALTWIDTH,ALTWIDTH,1); 
	shader = shader_new(shader_vs,shader_fs);
	expl_shader = shader_new(explshader_vs,explshader_fs);
	shadow_shader = shader_new(shadowshader_vs,shadowshader_fs);
	sun_shader = shader_new(sunshader_vs,sunshader_fs);
	water_shader = shader_new(watershader_vs,watershader_fs);
#define TEXTUREREF(x) extern char _binary_data_ ## x ## _bmp_start
#define TEXTURE(x) texture_new(&_binary_data_ ## x ## _bmp_start)
	TEXTUREREF(texmain24);
	TEXTUREREF(texsecond24);
	TEXTUREREF(water24);
	TEXTUREREF(clouds24);
	tm = TEXTURE(texmain24);
	ts = TEXTURE(texsecond24);
	tw = TEXTURE(water24);
	tc = TEXTURE(clouds24);
	
	level = level_new(tm);
	level_load(level);
#define MODELREF(x) extern char _binary_data_ ## x ## _pi_start
#define MODEL(x,p1,p2,p3) model_new(&_binary_data_ ## x ## _pi_start,p1,p2,p3)
	MODELREF(iceblock);
	MODELREF(tree);
	MODELREF(mainpeng);
	MODELREF(babypeng);
	MODELREF(bucket);
	MODEL(iceblock,tm,CHEST,0.75f);
	MODEL(tree,tm,TREE,1.0f);
	MODEL(mainpeng,ts,PENG,0.75f);
	MODEL(babypeng,ts,BABYPENG,1.0f);
	MODEL(bucket,tm,FISHPAIL,1.0f);
	expl2 = model_explosion(ts);
	sea = model_sea(tw,1500.0f,-20.0f,15.0f);
	sky = model_sky(tc,1500.0f,-20.0f,2.0f);
	sun_view = view_new();
	float A[3]={0,0,0};
	float A2[3]={300+240,300+240,0};
	float B[3]={0,-100,50};
	float B2[3]={300+30+240,300-20+240,40};
	view_look_at(level->view,A,B,0,0);
	view_look_at(sun_view,A2,B2,1,0);

}


 
//==============================================================================

int main (int argc, char **argv)
{
   // 0=headphones, 1=hdmi
   int audio_dest = 1;

   if (argc > 1)
      audio_dest = atoi(argv[1]);

   audio_init(audio_dest);

   // Clear application state
   memset( state, 0, sizeof( *state ) );
   
   // Start full screen drivers
   drm_gbm_start();
      
   // Start OGLES
   init_ogl(state);
   
   // Load data for level
   init_gl();

   while (1)
   {
	  // Apply game logic and render scene
	  tick();
	  // Audio needs regular updates to work
	  audio_update();
	  // Show rendered scene
	  drm_gbm_swap();
   }
   return 0;
}

