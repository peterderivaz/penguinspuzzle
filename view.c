/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to handle a view matrix including gradually moving it from an old to a new position.
*
*/

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "matrix.h"
#include "shaders.h"
#include "buffer.h"
#include "model.h"
#include "view.h"
#include "penguins.h"
#include "matrix.h"

int zoom_out=0;
float aspect = 1.0; // TODO

VIEW_T *view_new()
{
	VIEW_T *v;
	NEW(v);
	for(int i=0;i<3;i++)
		v->old_eye[i]=100;
	return v;
}
	
void view_look_at(VIEW_T *v, float at[3],float eye[3],int ortho,int adapt)
{
	if (adapt)
	{
		if (zoom_out)
		{
			float tmp[3];
			vec3_sub(tmp,eye,at);
			vec3_scale(tmp,tmp,1.5f);
			vec3_add(eye,at,tmp);
		}
		vec3_movetowards(at,v->old_at,at,5.0f);
		vec3_movetowards(eye,v->old_eye,eye,5.0f);
	}
	mat4_lookat(v->L,at,eye,0);
	mat4_lookat(v->L_reflect,at,eye,1);
	mat4_projection(v->P,ortho,aspect);
	mat4_mult(v->M,v->L,v->P);
	mat4_mult(v->M_reflect,v->L_reflect,v->P);
	vec3_copy(v->old_at,at);
	vec3_copy(v->old_eye,eye);
}

// Returns the angle 0,90,180,270 that most moves the penguin up the screen
extern PENGUIN_T *heropeng;
int view_find_up(VIEW_T *v)
{
	float pt[4];
	vec4_init(pt,heropeng->x,heropeng->y,0,1);
	float a[4];
	vec4_mult(a,pt,v->M);
	float oldscreeny = 0;
	int best = 0;
	for(int target=0;target<360;target+=90)
	{
		float dx=40*sin(target*3.1415f/180.0f);
		float dy=-40*cos(target*3.1415f/180.0f);			
		float pt2[4];
		vec4_init(pt2,heropeng->x+dx,heropeng->y+dy,0,1);
		float b[4];
		vec4_mult(b,pt2,v->M);
		float screeny = b[1]/b[3];
		if (target==0 || screeny>oldscreeny)
		{
			oldscreeny=screeny;
			best=target;
		}
	}
	return best;
}


