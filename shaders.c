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
* This file contains the functions to create and use shaders.
*/

#include <stdio.h>
#include "global.h"
#include "shaders.h"

/*
Print out the compile log for a shader
*/
static void showlog(GLint shader)
{
   // Prints the compile log for a shader
   char log[1024];
   glGetShaderInfoLog(shader,sizeof log,NULL,log);
   printf("%d:shader:\n%s\n", shader, log);
}

/*
Print out the link log for a program
*/
static void showprogramlog(GLint shader)
{
   // Prints the information log for a program object
   char log[1024];
   glGetProgramInfoLog(shader,sizeof log,NULL,log);
   printf("%d:program:\n%s\n", shader, log);
}

/*
Creates and compiles a new shader.

<shader_src> is a text string containing the source for the shader.
<is_frag> should be 1 for fragment shaders, or 0 for vertex shaders.
*/
int getShader(const GLchar *shader_src,int is_frag) 
{
	GLint ret;
	int shader = glCreateShader(is_frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
	
	glShaderSource(shader, 1, &shader_src, 0);
	glCompileShader(shader);

	//if (!glGetShaderParameter(shader, GL_COMPILE_STATUS))
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		showlog(shader);
	}

	return shader;
}


/* Creates a shader object with the given vertex and fragment shaders */
SHADER_T *shader_new(const GLchar *vs, const GLchar *fs) 
{
	SHADER_T *s;
	NEW(s);
    int fragmentShader = getShader(fs,1);
    int vertexShader = getShader(vs,0);

    int program = glCreateProgram();
	s->program = program; 
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    {
	int ret;
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	printf("Link status %d\n",ret);
    }

    //if (!glGetProgramParameter(program, GL_LINK_STATUS)) 
	//showprogramlog(program);

	s->attr_tex             = glGetAttribLocation(program, "tex");
	s->attr_vertex          = glGetAttribLocation(program, "vertex");
	s->attr_normal          = glGetAttribLocation(program, "normal");
	s->unif_world           = glGetUniformLocation(program, "world");
	s->unif_view            = glGetUniformLocation(program, "view");
	s->unif_view_reflect    = glGetUniformLocation(program, "view_reflect");
	s->unif_color           = glGetUniformLocation(program, "color");
	s->unif_texture         = glGetUniformLocation(program, "texture");
	s->unif_texture_reflect = glGetUniformLocation(program, "texture_reflect");
	s->unif_blend           = glGetUniformLocation(program, "blend");
	
    //printf("tex=%d\n",s->attr_tex);
    //printf("v=%d\n",s->attr_vertex);
    //printf("n=%d\n",s->attr_normal);
    //printf("w=%d\n",s->unif_world);
    //printf("view=%d\n",s->unif_view);
    //printf("view reflect=%d\n",s->unif_view_reflect);
    return s;
}

/*
Make this shaders program active
*/
void shader_select(SHADER_T *s)
{
	assert(s);
	glUseProgram ( s->program );

	glUniform1i(s->unif_texture, 0);
	glUniform1i(s->unif_texture_reflect, 1);
	shader_blend(s,0.0,0.0,0.0,0.0);
}

/* 
Configure the blending equations (some variables used in some of the shaders)
*/
void shader_blend(SHADER_T *s,float bx,float by,float bz,float bw)
{
	assert(s);
	glUniform4f(s->unif_blend, bx, by, bz, bw);
}

/*
Configure the world matrix
*/	
void shader_world(SHADER_T *s, float M[4][4])
{
	glUniformMatrix4fv(s->unif_world,1,GL_FALSE,(float*)M); // Note that Raspberry Pi does not support the input of transposed matrices
}

/*
Configure the view matrix
*/	
void shader_view(SHADER_T *s, float M[4][4])
{
	glUniformMatrix4fv(s->unif_view,1,GL_FALSE,(float*)M); // Note that Raspberry Pi does not support the input of transposed matrices
}

/*
Configure the reflected view matrix
*/	
void shader_view2(SHADER_T *s, float M[4][4])
{
	glUniformMatrix4fv(s->unif_view_reflect,1,GL_FALSE,(float*)M); // Note that Raspberry Pi does not support the input of transposed matrices
}

