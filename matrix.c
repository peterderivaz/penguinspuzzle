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
Matrix/vector library
*/

#include "global.h"
#include <math.h>
#include "matrix.h"

/*
C = A * B
*/
void mat4_mult(float C[4][4],float A[4][4], float B[4][4])
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			float t=0.0f;
			for(int k=0;k<4;k++)
				t+=A[i][k]*B[k][j];
			C[i][j]=t;
		}
	}
}

/*
C = A * B
B is a matrix
A is a vector
*/	
void vec4_mult(float C[4],float A[4], float B[4][4])
{
	for(int j=0;j<4;j++)
	{
		float t=0.0f;
		for(int k=0;k<4;k++)
			t+=A[k]*B[k][j];
		C[j]=t;
	}
}

void vec4_sub(float C[4],float A[4], float B[4])
{
	for(int i=0;i<4;i++)
		C[i] = A[i]-B[i];
}

void vec3_sub(float C[3],float A[3], float B[3])
{
	for(int i=0;i<3;i++)
		C[i] = A[i]-B[i];
}
	
void vec4_add(float C[4],float A[4], float B[4])
{
	for(int i=0;i<4;i++)
		C[i] = A[i]+B[i];
}

void vec3_add(float C[3],float A[3], float B[3])
{
	for(int i=0;i<3;i++)
		C[i] = A[i]+B[i];
}

void vec4_scale(float C[4],float A[4], float s)
{
	for(int i=0;i<4;i++)
		C[i] = A[i]*s;
}

void vec3_scale(float C[3],float A[3], float s)
{
	for(int i=0;i<3;i++)
		C[i] = A[i]*s;
}
	
float vec4_dot(float A[4],float B[4])
{
	float t=0.0f;
	for(int i=0;i<4;i++)
		t+=A[i]*B[i];
	return t;
}	

float vec3_dot(float A[3],float B[3])
{
	float t=0.0f;
	for(int i=0;i<3;i++)
		t+=A[i]*B[i];
	return t;
}

void vec3_cross(float C[3],float a[3], float b[3])
{
	C[0]=a[1]*b[2]-a[2]*b[1];
	C[1]=a[2]*b[0]-a[0]*b[2];
	C[2]=a[0]*b[1]-a[1]*b[0];
}
	
void vec3_normal(float C[3],float A[3])
{
	float n=sqrt(vec3_dot(A,A))+0.0001f;
	float inv = 1.0f/n;
	for(int i=0;i<3;i++)
		C[i]=A[i]*inv;
}

void vec3_copy(float C[3],float A[3])
{
	for(int i=0;i<3;i++)
		C[i]=A[i];
}

void vec3_init(float C[3], float a, float b, float c)
{
	C[0]=a;
	C[1]=b;
	C[2]=c;
}

void vec4_init(float C[3], float a, float b, float c, float d)
{
	C[0]=a;
	C[1]=b;
	C[2]=c;
	C[3]=d;
}

/*
Generate a rotation and translation to represent a camera at <eye> looking towards <at>.
The +z direction is assumed to be vertical.
If reflect is true then generates a reflected view assuming water at depth 20.
*/
void mat4_lookat(float C[4][4], float at[3], float eye[3], int reflect)
{ 
	float up[3]={0,0,1};
	if (reflect)
	{
		float depth=-20.0f;
		eye[2]=2*depth-eye[2];
		at[2]=2*depth-at[2];
	}
	float zaxis[3];
	float xaxis[3];
	float yaxis[3];
	float tmp[3];
	vec3_sub(tmp,at,eye);
	vec3_normal(zaxis,tmp);
	vec3_cross(tmp,up,zaxis);
	vec3_normal(xaxis,tmp);
	vec3_cross(yaxis,zaxis,xaxis);

	for(int a=0;a<4;a++)
	{
		C[a][0]=xaxis[a];
		C[a][1]=yaxis[a];
		C[a][2]=zaxis[a];
		C[a][3]=0.0f;
	}
	C[3][0]=-vec3_dot(xaxis,eye);
	C[3][1]=-vec3_dot(yaxis,eye);
	C[3][2]=-vec3_dot(zaxis,eye);
	C[3][3]=1.0f;
	if (reflect)
	{
		float depth=-20.0;
		eye[2]=2*depth-eye[2];
		at[2]=2*depth-at[2];
	}
}

/*
Generate a projection matrix in <C>.
If <ortho> then make an orthographic projection useful for shadow mapping
<aspect> contains the aspect ratio defined as width/height
*/
void mat4_projection(float C[4][4], int ortho, float aspect)
{
	float near=10;
	float far=2000.0f;
	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
		C[i][j]=0;
		
	if (ortho)
	{
		float s = 0.002f;
		// Scale z to far
		// Keep w as unity
		for(int i=0;i<3;i++)
			C[i][i]=s;
		C[3][3]=1.0f;
		return;
	}
	float fov_v=1.4f;
	float h=1.0f/tan(fov_v*0.5f);
	float w = h /aspect;
	float Q=far/(far-near);
	C[0][0]=w;
	C[1][1]=h;
	C[2][2]=Q;
	C[2][3]=1.0f;
	C[3][2]=-Q*near;	
}
		
void vec3_movetowards(float C[3], float old_at[3],float at[3],float speed) 
{
	float d[3];
	vec3_sub(d,at,old_at);
	float dist = sqrt(vec3_dot(d,d))+0.001f;
	float move = min(dist,speed);
	vec3_scale(d,d,move/dist);
	vec3_add(C,old_at,d);
}


void mat4_identity(float M[4][4])
{
	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
	{
		M[i][j]=0;
	}
	for(int j=0;j<4;j++)
		M[j][j]=1;
}

void mat4_copy(float D[4][4],float M[4][4])
{
	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
	{
		D[i][j]=M[i][j];
	}
}

// In place translation of a 4x4 matrix
void mat4_translate(float M[4][4],float pt[3])
{
	for(int i=0;i<4;i++)
	{
		float t=M[3][i];
		for(int j=0;j<3;j++)
			t+=pt[j]*M[j][i];
		M[3][i]=t;
	}
}

// In-place Rotate around z axis
void mat4_rotatez(float V[4][4],float angle)
{
	float c=cos(angle*3.1415f/180.0f);
	float s=sin(angle*3.1415f/180.0f);
	float M[4][4] = {{c,s,0,0},{-s,c,0,0},{0,0,1,0},{0,0,0,1}};
	float C[4][4];
	mat4_mult(C,M,V);
	mat4_copy(V,C);
}

// In-place Rotate around x axis
void mat4_rotatex(float V[4][4],float angle)
{
	float c=cos(angle*3.1415f/180.0f);
	float s=sin(angle*3.1415f/180.0f);
	float M[4][4] = {{1,0,0,0},{0,c,s,0},{0,-s,c,0},{0,0,0,1}};
	float C[4][4];
	mat4_mult(C,M,V);
	mat4_copy(V,C);
}

void mat4_billboard(float V[4][4])
{
	//Define a matrix that copies x,y and sets z to 0.9
	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
	{
		V[i][j]=0;
	}
	V[0][0]=1;
	V[1][1]=1;
	V[3][2]=0.9f;
	V[3][3]=1.0f;
}
