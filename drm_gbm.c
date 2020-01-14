// Code from Raspberry Pi forums on example OpenGLES fullscreen application

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "drm_gbm.h"

#define EXIT(msg) { fputs (msg, stderr); exit (EXIT_FAILURE); }

// global variables declarations

static int device;
static drmModeRes *resources;
static drmModeConnector *connector;
static uint32_t connector_id;
static drmModeEncoder *encoder;
static drmModeModeInfo mode_info;
static drmModeCrtc *crtc;
static struct gbm_device *gbm_device;
static EGLDisplay display;
static EGLContext context;
static struct gbm_surface *gbm_surface;
static EGLSurface egl_surface;
       EGLConfig config;
       EGLint num_config;
       EGLint count=0;
       EGLConfig *configs;
       int config_index;
       int i;
       
static struct gbm_bo *previous_bo = NULL;
static uint32_t previous_fb;       

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

struct gbm_bo *bo;	
uint32_t handle;
uint32_t pitch;
uint32_t fb;
uint64_t modifier;


static drmModeConnector *find_connector (drmModeRes *resources) {

for (i=0; i<resources->count_connectors; i++) {
  drmModeConnector *connector = drmModeGetConnector (device, resources->connectors[i]);
  if (connector->connection == DRM_MODE_CONNECTED) {return connector;}
  drmModeFreeConnector (connector);
  }
return NULL; // if no connector found
}

static drmModeEncoder *find_encoder (drmModeRes *resources, drmModeConnector *connector) {

if (connector->encoder_id) {return drmModeGetEncoder (device, connector->encoder_id);}
return NULL; // if no encoder found
}

static void swap_buffers () {

eglSwapBuffers (display, egl_surface);
bo = gbm_surface_lock_front_buffer (gbm_surface);
handle = gbm_bo_get_handle (bo).u32;
pitch = gbm_bo_get_stride (bo);
drmModeAddFB (device, mode_info.hdisplay, mode_info.vdisplay, 24, 32, pitch, handle, &fb);
drmModeSetCrtc (device, crtc->crtc_id, fb, 0, 0, &connector_id, 1, &mode_info);
if (previous_bo) {
  drmModeRmFB (device, previous_fb);
  gbm_surface_release_buffer (gbm_surface, previous_bo);
  }
previous_bo = bo;
previous_fb = fb;
}

static void draw (float progress) {

glClearColor (1.0f-progress, progress, 0.0, 1.0);
glClear (GL_COLOR_BUFFER_BIT);
swap_buffers ();
}

static int match_config_to_visual(EGLDisplay egl_display, EGLint visual_id, EGLConfig *configs, int count) {

EGLint id;
for (i = 0; i < count; ++i) {
  if (!eglGetConfigAttrib(egl_display, configs[i], EGL_NATIVE_VISUAL_ID,&id)) continue;
  if (id == visual_id) return i;
  }
return -1;
}

void drm_gbm_start () {
	device = open ("/dev/dri/card1", O_RDWR);
	if (device < 0)
	  device = open ("/dev/dri/card0", O_RDWR);
	if (device < 0) {
	  puts("Unable to open /dev/dri/card[01]. Is vc4-fkms-v3d enabled?");
	  exit(1);
	}
	resources = drmModeGetResources (device);
	connector = find_connector (resources);
	connector_id = connector->connector_id;
	mode_info = connector->modes[0];
	encoder = find_encoder (resources, connector);
	crtc = drmModeGetCrtc (device, encoder->crtc_id);
	drmModeFreeEncoder (encoder);
	drmModeFreeConnector (connector);
	drmModeFreeResources (resources);
	gbm_device = gbm_create_device (device);
	gbm_surface = gbm_surface_create (gbm_device, mode_info.hdisplay, mode_info.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
	display = eglGetDisplay (gbm_device);
	eglInitialize (display, NULL ,NULL);
	eglBindAPI (EGL_OPENGL_API);
	eglGetConfigs(display, NULL, 0, &count);
	configs = malloc(count * sizeof *configs);
	eglChooseConfig (display, attributes, configs, count, &num_config);
	config_index = match_config_to_visual(display,GBM_FORMAT_XRGB8888,configs,num_config);
	context = eglCreateContext (display, configs[config_index], EGL_NO_CONTEXT, context_attribs);
	egl_surface = eglCreateWindowSurface (display, configs[config_index], gbm_surface, NULL);
	free(configs);
	eglMakeCurrent (display, egl_surface, egl_surface, context);
}

int drm_width(void) {
	return mode_info.hdisplay;
}

int drm_height(void) {
	return mode_info.vdisplay;
}

void drm_gbm_test(void) {
	int i;
	for (i = 0; i < 600; i++) draw (i / 600.0f);
}

void drm_gbm_swap(void) {
	swap_buffers();
}

void drm_gbm_finish(void) {
	//printf("Error status=%d\n",glGetError());
	
	drmModeSetCrtc (device, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &connector_id, 1, &crtc->mode);
	drmModeFreeCrtc (crtc);
	if (previous_bo) {
	  drmModeRmFB (device, previous_fb);
	  gbm_surface_release_buffer (gbm_surface, previous_bo);
	  }
	eglDestroySurface (display, egl_surface);
	gbm_surface_destroy (gbm_surface);
	eglDestroyContext (display, context);
	eglTerminate (display);
	gbm_device_destroy (gbm_device);

	close (device);
}
