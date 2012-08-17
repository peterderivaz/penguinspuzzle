/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to load and draw 3d models.
*
*/

typedef struct model_s
{
	int tex;     // The texture to be used for this model
	BUFFER_T *buf; // The buffer containing the 3d data
	int type;    // Internal designator giving the type of data stored
} MODEL_T;

#define NUMMODELS 100
extern MODEL_T *models[NUMMODELS];

// Allocates a model object to use the given texture.
// All points are scaled by scale after being loaded.
// Assigns the model to the given id in an array of models
extern MODEL_T *model_new(char *name, int tex, int id, float scale);

// Draw a model with the given shader
extern void model_draw(MODEL_T *m,SHADER_T *s);

// Draw a model based on an id passed during a call to model_new
extern void model_drawid(int id,SHADER_T *s);

// Generate a model consisting of a triangle fan for the ocean
extern MODEL_T *model_sea(int t,float sz,float depth,float texscale);

// Generate a model consisting of a triangle strip for the sky
extern MODEL_T *model_sky(int t,float sz,float depth,float texscale);

// Generate a model consisting of a set of triangles used for explosions
extern MODEL_T *model_explosion(int t);