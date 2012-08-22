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
* Header file for shader creation.
*/
	
typedef struct shader_s
{
	int program              ;
	int attr_tex             ;
	int attr_vertex          ;
	int attr_normal          ;
	int unif_world           ;
	int unif_view            ;
	int unif_view_reflect    ;
	int unif_color           ;
	int unif_texture         ;
	int unif_texture_reflect ;
	int unif_blend           ;
} SHADER_T;


/* 
Creates a shader object with the given vertex and fragment shaders 
*/
extern SHADER_T *shader_new(const GLchar *vs, const GLchar *fs);

/*
Make this shaders program active
*/
void shader_select(SHADER_T *s);

/* 
Configure the blending equations (some variables used in some of the shaders)
*/
void shader_blend(SHADER_T *s,float bx,float by,float bz,float bw);

/*
Configure the world matrix
*/	
void shader_world(SHADER_T *s, float M[4][4]);

/*
Configure the view matrix
*/	
void shader_view(SHADER_T *s, float M[4][4]);

/*
Configure the reflected view matrix
*/	
void shader_view2(SHADER_T *s, float M[4][4]);

