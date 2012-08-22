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
	
#include "global.h"
#include "shaders.h"
#include "buffer.h"
#include "matrix.h"

/*
Create a new buffer object from an array of vec3s for pts, and vec3s for texture coordinates.
Faces is an array of 3 shorts for each face.
*/ 
BUFFER_T *buffer_new(float pts[][3], float tex[][3], short faces[][3], int numpts, int numfaces)
{
	int i;
	float normals[numpts][3];
	BUFFER_T *buf;
	NEW(buf);

	for(i=0;i<numpts;i++)
	{
		normals[i][0]=0;
		normals[i][1]=0;
		normals[i][2]=1;
	}
	for(i=0;i<numfaces;i++)
	{
		int a = faces[i][0];
		int b = faces[i][1];
		int c = faces[i][2];
		float tmp[3];
		float tmp2[3];
		float n[3];
		vec3_sub(tmp,pts[b],pts[a]);
		vec3_sub(tmp2,pts[c],pts[a]);
		vec3_cross(n,tmp,tmp2);
		vec3_normal(n,n);
		vec3_copy(normals[a],n);
		vec3_copy(normals[b],n);
		vec3_copy(normals[c],n);
	}
	float B[3*3*numpts];
	for(i=0;i<numpts;i++)
	{
		for(int j=0;j<3;j++)
		{
			B[i*3*3+j]=pts[i][j];
			B[i*3*3+3+j]=tex[i][j];
			B[i*3*3+6+j]=normals[i][j];
		}
	}

	buf->ntris=numfaces;
	glGenBuffers(1,&buf->vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, buf->vbuf);
	glBufferData(GL_ARRAY_BUFFER, 36*numpts, B, GL_STATIC_DRAW);
	buf->vbuf_num = numpts;
	
	glGenBuffers(1,&buf->ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*numfaces, faces, GL_STATIC_DRAW);
	buf->ebuf_num = numfaces*3;
	
	return buf;
}

/* Deallocate resources for the given buffer */
void buffer_free(BUFFER_T *b)
{
	glDeleteBuffers(1,&b->vbuf);
	glDeleteBuffers(1,&b->ebuf);	
}

/*
Activate the shader first, then call this function to bind the buffer to the shader
*/
void buffer_select (BUFFER_T *buf, SHADER_T *s)
{
	assert(buf);
	assert(s);
	glBindBuffer(GL_ARRAY_BUFFER, buf->vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->ebuf);
	if (s->attr_normal>=0) glVertexAttribPointer(s->attr_normal, 3, GL_FLOAT, 0, 36, (void*)24); 
	if (s->attr_tex>=0) glVertexAttribPointer(s->attr_tex, 2, GL_FLOAT, 0, 36, (void*)12);
	if (s->attr_vertex>=0) glVertexAttribPointer(s->attr_vertex, 3, GL_FLOAT, 0, 36, (void*)0);
	if (s->attr_normal>=0) glEnableVertexAttribArray(s->attr_normal);
	if (s->attr_tex>=0) glEnableVertexAttribArray(s->attr_vertex);
	if (s->attr_normal>=0) glEnableVertexAttribArray(s->attr_tex);
}

void buffer_draw(BUFFER_T *b, SHADER_T *s)
{
	buffer_select(b,s);
	glDrawElements ( GL_TRIANGLES, b->ebuf_num, GL_UNSIGNED_SHORT, 0 );
}

void buffer_drawfan(BUFFER_T *b, SHADER_T *s)
{
	buffer_select(b,s);
	glDrawArrays ( GL_TRIANGLE_FAN, 0, b->vbuf_num );
}

void buffer_drawstrip(BUFFER_T *b, SHADER_T *s)
{
	buffer_select(b,s);
	glDrawArrays ( GL_TRIANGLE_STRIP, 0, b->vbuf_num );
}
