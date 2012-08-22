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
* This file contains the source code for the different shaders used in the game.
* _vs are the vertex shaders.
* _fs are the fragment shaders.
*/

typedef struct buffer_s
{
	GLuint vbuf;
	int vbuf_num;
	GLuint ebuf;
	int ebuf_num;
	int ntris;
} BUFFER_T;

/*
Create a new buffer object from an array of vec3s for pts, and vec3s for texture coordinates.
Faces is an array of 3 shorts for each face.
*/ 
extern BUFFER_T *buffer_new(float pts[][3], float tex[][3], short faces[][3], int numpts, int numfaces);

/* Deallocate resources for the given buffer */
extern void buffer_free(BUFFER_T *b);

/*
Activate the shader first, then call this function to bind the buffer to the shader
*/
extern void buffer_select (BUFFER_T *buf, SHADER_T *s);

extern void buffer_draw(BUFFER_T *b, SHADER_T *s);

extern void buffer_drawfan(BUFFER_T *b, SHADER_T *s);

extern void buffer_drawstrip(BUFFER_T *b, SHADER_T *s);
