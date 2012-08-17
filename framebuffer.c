/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to create off-screen framebuffers (used for shadow and reflections).
*
*/

#include <stdio.h>
#include "global.h"
#include "framebuffer.h"


// Allocate a new frame buffer of given dimensions <w> * <h>
// and set it up in texture unit 1
// If add_depth is true then we also include a depth buffer
FRAMEBUFFER_T *framebuffer_new(int w,int h,int add_depth)
{
	FRAMEBUFFER_T *f;
	NEW(f);
	glGenTextures(1,&f->tex);
	assert(f->tex>=0);
	f->w=w;
	f->h=h;
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,f->tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenFramebuffers(1,&f->fb);
	assert(f->fb>=0);
	glBindFramebuffer(GL_FRAMEBUFFER,f->fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,f->tex,0);
	glActiveTexture(GL_TEXTURE0);
	if (!add_depth) return f;
	GLuint depthbuf;
	glGenRenderbuffers(1,&depthbuf);
	assert(depthbuf>=0);
	glBindRenderbuffer(GL_RENDERBUFFER,depthbuf);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,w,h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthbuf);
	return f;
}

/* Make this framebuffer the active render target */
void framebuffer_select(FRAMEBUFFER_T *f)
{
	assert(f);
	glBindFramebuffer(GL_FRAMEBUFFER,f->fb);
	glViewport(0, 0, f->w, f->h);
}

/* Select the framebuffer for use in the texture 1 pipeline */ 
void framebuffer_select_texture(FRAMEBUFFER_T *f)
{
	assert(f);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,f->tex);
	glActiveTexture(GL_TEXTURE0);
}

