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

const GLchar *shader_vs =	
"	attribute vec2 tex;                                            \n"
"	attribute vec3 vertex;                                         \n"
"	attribute vec3 normal;                                         \n"
"	varying vec2 tcoord;                                           \n"
"	uniform mat4 view;                                             \n"
"	uniform mat4 world;                                            \n"
"	uniform vec4 blend;                                            \n"
"	varying float light;                                           \n"
"	void main(void) {                                              \n"
"		vec4 normal2 = world * vec4(normal,0.0);                   \n"
"		light = 0.5+max(0.0,0.5*dot(normal2.xyz,vec3(0.7,0,0.7))); \n"
"		tcoord = tex+blend.xy;                                     \n"
"		vec4 vertex2 = world * vec4(vertex,1.0);                   \n"
"		gl_Position = view * vertex2;                              \n"
"	}                                                              \n";

const GLchar *shader_fs =	
"    precision mediump float;	                                 \n"
"	uniform vec4 color;                                          \n"
"	uniform sampler2D texture;                                   \n"
"	varying vec2 tcoord;                                         \n"
"	varying float light;                                         \n"
"	void main(void) {                                            \n"
"	    mediump vec4 col = texture2D(texture,tcoord);            \n"
"		if (col.x>0.9 && col.y>0.6 && col.y<0.61 && col.z<0.1)   \n"
"			 discard;                                            \n"
"		gl_FragColor = col*light;                                \n"
"		gl_FragColor.a = 1.0;                                    \n"
"	}                                                            \n";

const GLchar *explshader_vs =	
"	attribute vec2 tex;                                                                                \n"
"	attribute vec3 vertex;                                                                             \n"
"	attribute vec3 normal;                                                                             \n"
"	varying vec2 tcoord;                                                                               \n"
"	uniform mat4 view;                                                                                 \n"
"	uniform mat4 world;                                                                                \n"
"	uniform vec4 blend;                                                                                \n"
"	varying float light;                                                                               \n"
"	void main(void) {                                                                                  \n"
"		// blend.x gives time to simulate                                                              \n"
"		light = 0.5+max(0.0,0.5*dot(vertex.xyz,vec3(0.7,0,0.7)));                                      \n"
"		tcoord = tex*0.23+vec2(0.51,0.26);                                                             \n"
"		// Update the position                                                                         \n"
"		vec3 vt = vertex + vertex*blend.x*0.06;                                                        \n"
"		vt[2] -= blend.x*blend.x*0.05;                                                                 \n"
"		vec4 vertex2 = world * vec4(vt,1.0);                                                           \n"
"		vec4 pos = view * vertex2;                                                                     \n"
"		// Now we want to translate the x,y depending on tex.xy                                        \n"
"		pos.xy += 0.05*pos.z*tex.xy;                                                                   \n"
"		gl_Position = pos;                                                                             \n"
"	}                                                                                                  \n";

const GLchar *explshader_fs =	
"    precision mediump float;	                      \n"
"	uniform vec4 color;                               \n"
"	uniform sampler2D texture;                        \n"
"	varying vec2 tcoord;                              \n"
"	varying float light;                              \n"
"	void main(void) {                                 \n"
"	    mediump vec4 col = texture2D(texture,tcoord); \n"
"		gl_FragColor = col;                           \n"
"		gl_FragColor.a = col.b*0.8;                   \n"
"	}                                                 \n";

const GLchar *shadowshader_vs =	
"	attribute vec2 tex;                                                              \n"
"	attribute vec3 vertex;                                                           \n"
"	attribute vec3 normal;                                                           \n"
"	varying vec2 tcoord;                                                             \n"
"	uniform mat4 view;                                                               \n"
"	uniform mat4 view_reflect;                                                       \n"
"	uniform mat4 world;                                                              \n"
"	uniform vec4 blend;                                                              \n"
"	varying float light;                                                             \n"
"	varying float light_dist;                                                        \n"
"	varying vec4 reflect_pos;                                                        \n"
"	void main(void) {                                                                \n"
"		vec4 pos = world * vec4(vertex,1.0);                                         \n"
"		vec4 normal2 = world * vec4(normal,0.0);                                     \n"
"		light = 0.5+max(0.0,0.5*dot(normal2.xyz,vec3(0.7,0,0.7)));                   \n"
"		tcoord = tex+blend.xy;                                                       \n"
"		light_dist = (pos.z+20.0)*0.004;  // Should really be after world transform  \n"
"		reflect_pos = view_reflect * pos;                                            \n"
"		gl_Position = view * pos;                                                    \n"
"	}                                                                                \n";

const GLchar *shadowshader_fs =
"    precision mediump float;	                                                    \n"
"	uniform vec4 color;                                                             \n"
"	uniform sampler2D texture;                                                      \n"
"	varying vec2 tcoord;                                                            \n"
"	varying float light;                                                            \n"
"	varying float light_dist;                                                       \n"
"	varying vec4 reflect_pos;                                                       \n"
"	uniform sampler2D texture_reflect;                                              \n"
"	void main(void) {                                                               \n"
"	    mediump vec4 col = texture2D(texture,tcoord);                               \n"
"		if (col.x>0.9 && col.y>0.6 && col.y<0.61 && col.z<0.1)                      \n"
"			 discard;                                                               \n"
"		mediump vec4 col2;                                                          \n"
"		vec2 reflect_sample;                                                        \n"
"		reflect_sample = (vec2(reflect_pos.x,reflect_pos.y)*0.5)/reflect_pos.w+0.5; \n"
"		col2 = texture2D(texture_reflect,reflect_sample);                           \n"
"		col = col*light;                                                            \n"
"		if (light_dist+0.01 < col2.r)                                               \n"
"			col = col * 0.5;                                                        \n"
"		gl_FragColor = col;                                                         \n"
"		gl_FragColor.a = 1.0;                                                       \n"
"	}                                                                               \n";

const GLchar *sunshader_vs =
"	attribute vec2 tex;                      \n"
"	attribute vec3 vertex;                   \n"
"	attribute vec3 normal;                   \n"
"	uniform mat4 world;                      \n"
"	uniform mat4 view;                       \n"
"	varying float light;	                 \n"
"	varying vec2 tcoord;                     \n"
"	void main(void) {                        \n"
"		vec4 pos = world * vec4(vertex,1.0); \n"
"		light = (pos.z+20.0)*0.004;          \n"
"		tcoord = tex;                        \n"
"		gl_Position = view * pos;            \n"
"	}                                        \n";

const GLchar *sunshader_fs =
"    precision mediump float;                                  \n"
"    uniform sampler2D texture;	                               \n"
"	varying vec2 tcoord;                                       \n"
"	varying float light;                                       \n"
"	void main(void) {                                          \n"
"	    mediump vec4 col = texture2D(texture,tcoord);          \n"
"		if (col.x>0.9 && col.y>0.6 && col.y<0.61 && col.z<0.1) \n"
"			 discard;                                          \n"
"		gl_FragColor = vec4(light,light,light,1.0);            \n"
"	}                                                          \n";

const GLchar *watershader_vs =	
"    attribute vec2 tex;                    \n"
"	attribute vec3 vertex;                  \n"
"	varying vec2 tcoord;                    \n"
"	varying vec2 tcoord2;                   \n"
"	uniform mat4 view;                      \n"
"	uniform mat4 view_reflect;              \n"
"	uniform vec4 blend;                     \n"
"	varying vec4 reflect_pos;               \n"
"	void main(void) {                       \n"
"		vec4 pos = vec4(vertex,1.0);        \n"
"		tcoord = tex+blend.xy;              \n"
"		tcoord2 = tex.ts+blend.zw;          \n"
"		reflect_pos = view_reflect * pos;   \n"
"		gl_Position = view * pos;           \n"
"	}                                       \n";

const GLchar *watershader_fs =
"    precision mediump float;	                                                                     \n"
"	uniform sampler2D texture;                                                                       \n"
"	uniform sampler2D texture_reflect;                                                               \n"
"	varying vec2 tcoord;                                                                             \n"
"	varying vec2 tcoord2;                                                                            \n"
"	varying vec4 reflect_pos;                                                                        \n"
"	void main(void) {                                                                                \n"
"		mediump vec3 eye;                                                                            \n"
"		float light2;                                                                                \n"
"		mediump vec4 col;                                                                            \n"
"		mediump vec4 col2;                                                                           \n"
"		vec2 reflect_sample;                                                                         \n"
"		col = texture2D(texture,tcoord);                                                             \n"
"		col2 = texture2D(texture,tcoord2);                                                           \n"
"		col=col*0.4+col2*0.6-0.5;                                                                    \n"
"		reflect_sample = (vec2(reflect_pos.x,reflect_pos.y)*0.5)/reflect_pos.w+0.5;                  \n"
"		reflect_sample = vec2(reflect_sample.s+col.s*0.1,reflect_sample.t+col.t*0.1+0.005);          \n"
"		col2 = texture2D(texture_reflect,reflect_sample);                                            \n"
"		eye = vec3(gl_FragCoord.x-320.0+(col.r)*1000.0,gl_FragCoord.y+(col.g)*1000.0-540.0,600.0);   \n"
"		eye=normalize(eye);                                                                          \n"
"		light2=dot(eye,vec3(0.1,0.3,0.8));                                                           \n"
"		light2=2.0*pow(light2,4.0);                                                                  \n"
"		gl_FragColor = col2*0.7+vec4(1.0,0.8,0.6,1.0)*light2;                                        \n"
"		gl_FragColor.a = 1.0;                                                                        \n"
"	}                                                                                                \n";

