/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to load and draw 3d models.
*
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "global.h"
#include "shaders.h"
#include "buffer.h"
#include "model.h"
#include "texture.h"
#include "embed_file.h"

enum {TRIS,FAN,STRIP};

MODEL_T *models[NUMMODELS];

// Allocates a model object to use the given texture.
// All points are scaled by scale after being loaded.
// Assigns the model to the given id in an array of models
MODEL_T *model_new(char *name, int texid, int id, float scale)
{
	int numpts;
	int numfaces;
	EMBED_FILE *fid;
	MODEL_T *m;
	NEW(m);
	fid=embed_fopen(name,"rb");
	if (!fid)
	{
		//fprintf(stderr,"Cannot find %s",name);
		exit(-1);
	}
	assert(fid);
	embed_fread(&numpts,sizeof(int),1,fid);
	embed_fread(&numfaces,sizeof(int),1,fid);
	float pts[numpts][3];
	float tex[numpts][3];
	short faces[numpts][3];
	embed_fread(pts,sizeof(float),3*numpts,fid);
	embed_fread(tex,sizeof(float),3*numpts,fid);
	embed_fread(faces,sizeof(short),3*numfaces,fid);
	for(int i=0;i<numpts;i++)
	for(int j=0;j<3;j++)
		pts[i][j]*=scale;
	m->buf = buffer_new(pts, tex, faces, numpts, numfaces);
	m->tex = texid;
	m->type = TRIS;
	assert(id<NUMMODELS);
	models[id]=m;
	embed_fclose(fid);
	return m;
}

void model_draw(MODEL_T *m,SHADER_T *s)
{
	assert(m);
	texture_select(m->tex);
	if (m->type==TRIS)
		buffer_draw(m->buf,s);
	else if (m->type==FAN)
		buffer_drawfan(m->buf,s);
	else if (m->type==STRIP)
		buffer_drawstrip(m->buf,s);
	else
		assert(0);
}

void model_drawid(int id,SHADER_T *s)
{
	model_draw(models[id],s);
}

// Generate a model consisting of a triangle fan for the ocean
MODEL_T *model_sea(int t,float sz,float depth,float texscale)
{
	int numpts=30;  // Number of points around circumference
	float pts[numpts+2][3];
	float tex[numpts+2][3];
	short faces[1][3];
	MODEL_T *m;
	NEW(m);
	m->tex=t;

	pts[0][0]=0;
	pts[0][1]=0;
	pts[0][2]=depth;
	tex[0][0]=0;
	tex[0][0]=0;
	tex[0][0]=1.0;

	for(int i=0;i<=numpts;i++)
	{
		float angle=2*3.1416f*i/numpts;
		float x = cos(angle);
		float y = sin(angle);
		pts[i+1][0] = x*sz+400;
		pts[i+1][1] = y*sz+400;
		pts[i+1][2] = depth;
		tex[i+1][0] = x*texscale;
		tex[i+1][1] = y*texscale;
		tex[i+1][2] = 1.0f;
	}
	m->buf = buffer_new(pts,tex,faces,numpts+2,0);
	m->type = FAN;
	return m;
}


// Generate a model consisting of a triangle strip for the sky
MODEL_T *model_sky(int t,float sz,float depth,float texscale)
{
	int numpts=30;  // Number of points around circumference
	float pts[2*(numpts+1)][3];
	float tex[2*(numpts+1)][3];
	short faces[1][3];
	MODEL_T *m;
	NEW(m);
	m->tex=t;

	for(int i=0;i<=numpts;i++)
	{
		float angle=2*3.1416f*i/numpts;
		float x = cos(angle);
		float y = sin(angle);
		pts[2*i][0] = x*sz+400;
		pts[2*i][1] = y*sz+400;
		pts[2*i][2] = depth;
		tex[2*i][0] = i*texscale/numpts;
		tex[2*i][1] = 1.0f;
		tex[2*i][2] = 1.0f;
		
		pts[2*i+1][0] = x*sz+400;
		pts[2*i+1][1] = y*sz+400;
		pts[2*i+1][2] = sz;
		tex[2*i+1][0] = i*texscale/numpts;
		tex[2*i+1][1] = 0.0f;
		tex[2*i+1][2] = 1.0f;
		
	}
	m->buf = buffer_new(pts,tex,faces,2*(numpts+1),0);
	m->type = STRIP;
	return m;
}


MODEL_T *model_explosion(int t)
{
	int numpts=300;  // Number of points around circumference
	float pts[4*numpts][3];
	float tex[4*numpts][3];
	short faces[4*numpts][3];
	float deltas[4][2] = {{0,0},{1,0},{1,1},{0,1}};
	MODEL_T *m;
	NEW(m);
	m->tex=t;

	for(int i=0;i<numpts;i++)
	{
		float x = (rand()&31)-16;
		float y = (rand()&31)-16;
		float z = (rand()&31);
		int N=i*4;
		for(int j=0;j<4;j++)
		{
			pts[N+j][0] = x;
			pts[N+j][1] = y;
			pts[N+j][2] = z;
			tex[N+j][0] = deltas[j][0];
			tex[N+j][1] = deltas[j][1];
			tex[N+j][2] = 1.0f;
		}
		faces[N][0]=N;
		faces[N][1]=N+1;
		faces[N][2]=N+2;
		faces[N+1][0]=N;
		faces[N+1][1]=N+2;
		faces[N+1][2]=N+3;
		faces[N+2][0]=N;
		faces[N+2][1]=N+2;
		faces[N+2][2]=N+1;
		faces[N+3][0]=N;
		faces[N+3][1]=N+3;
		faces[N+3][2]=N+2;
	}
	m->buf = buffer_new(pts,tex,faces,numpts*4,numpts*4);
	return m;
}

