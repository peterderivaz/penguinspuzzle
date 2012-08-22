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
* This contains code to load and draw 3d models.
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
