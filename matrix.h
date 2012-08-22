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
* Matrix/vector library
*/

/*
C = A * B
*/
extern void mat4_mult(float C[4][4],float A[4][4], float B[4][4]);

/*
C = A * B
B is a matrix
A is a vector
*/	
extern void vec4_mult(float C[4],float A[4], float B[4][4]);

extern void vec4_sub(float C[4],float A[4], float B[4]);
extern void vec3_sub(float C[3],float A[3], float B[3]);
	
extern void vec4_add(float C[4],float A[4], float B[4]);
extern void vec3_add(float C[3],float A[3], float B[3]);

extern void vec4_scale(float C[4],float A[4], float s);
extern void vec3_scale(float C[3],float A[3], float s);
	
extern float vec4_dot(float A[4],float B[4]);
	
extern void vec3_cross(float C[3],float a[3], float b[3]);
	
extern void vec3_normal(float C[3],float A[3]);

extern void vec3_copy(float C[3],float A[3]);

extern void vec3_init(float C[3], float a, float b, float c);
extern void vec4_init(float C[3], float a, float b, float c, float d);

/*
Generate a rotation and translation to represent a camera at <eye> looking towards <at>.
The +z direction is assumed to be vertical.
If reflect is true then generates a reflected view assuming water at depth 20.
*/
extern void mat4_lookat(float C[4][4], float at[3], float eye[3], int reflect);

/*
Generate a projection matrix in <C>.
If <ortho> then make an orthographic projection useful for shadow mapping
<aspect> contains the aspect ratio defined as width/height
*/
extern void mat4_projection(float C[4][4], int ortho, float aspect);

		
extern void mat4_identity(float M[4][4]);

extern void mat4_copy(float D[4][4],float M[4][4]);

// In place translation of a 4x4 matrix
extern void mat4_translate(float M[4][4],float pt[3]);

// In-place Rotate around z axis
extern void mat4_rotatez(float V[4][4],float angle);

// In-place Rotate around x axis
extern void mat4_rotatex(float V[4][4],float angle);

extern void mat4_billboard(float V[4][4]);

extern void vec3_movetowards(float C[3], float old_at[3],float at[3],float speed);
