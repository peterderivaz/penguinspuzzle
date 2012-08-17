/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to handle a view matrix including gradually moving it from an old to a new position.
*
*/

typedef struct view_s
{
	float old_at[3];
	float old_eye[3];
	float L[4][4];
	float M[4][4];
	float L_reflect[4][4];
	float M_reflect[4][4];
	float P[4][4];
} VIEW_T;

extern int zoom_out;
extern float aspect;

extern VIEW_T *view_new();
	
extern void view_look_at(VIEW_T *v, float at[3],float eye[3],int ortho,int adapt);

// Returns the angle 0,90,180,270 that most moves the penguin up the screen
extern int view_find_up(VIEW_T *v);


