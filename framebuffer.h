/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to create off-screen framebuffers (used for shadow and reflections).
*
*/

typedef struct framebuffer_s
{	
	int w;	 // Width
	int h;   // Height
	GLuint tex; // Texture number
	GLuint fb;  // Framebuffer number
} FRAMEBUFFER_T;

// Allocate a new frame buffer of given dimensions <w> * <h>
// and set it up in texture unit 1
// If add_depth is true then we also include a depth buffer
extern FRAMEBUFFER_T *framebuffer_new(int w,int h,int add_depth);

/* Make this framebuffer the active render target */
extern void framebuffer_select(FRAMEBUFFER_T *f);

/* Select the framebuffer for use in the texture 1 pipeline */ 
extern void framebuffer_select_texture(FRAMEBUFFER_T *f);


