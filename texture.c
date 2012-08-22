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
* This contains code to load and select a texture.
* We only actually need an integer to specify a texture.
*/

#include <stdio.h>
#include "global.h"
#include "texture.h"
#include "embed_file.h"

int texture_new(char *texname) 
{
	GLuint tex=0;
	glGenTextures(1,&tex);	
	int w,h,off,sz;
	char *pixels;
	EMBED_FILE *fid=embed_fopen(texname,"rb");
	assert(fid);
	embed_fseek(fid,10,SEEK_SET);
	embed_fread(&off,sizeof(int),1,fid);
	embed_fread(&sz,sizeof(int),1,fid);
	embed_fread(&w,sizeof(int),1,fid);
	embed_fread(&h,sizeof(int),1,fid);
	embed_fseek(fid,off,SEEK_SET);
	pixels = malloc(w*h*3);
	assert(pixels);
	// Read in reverse order as BMP stores textures upside down
	// Also convert BGR to RGB 
	for(int y=0;y<h;y++)
	{
		char *p=&pixels[w*3*(h-1-y)];
		embed_fread(p,1,w*3,fid);
		for(int x=0;x<w;x++)
		{
			char t=p[x*3];
			p[x*3]=p[x*3+2];
			p[x*3+2]=t;
		}
	}
	
	glBindTexture(GL_TEXTURE_2D, tex);
	//glPixelStorei(GL_UNPACK_FLIP_Y_WEBGL, 1);  // not normally needed
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	embed_fclose(fid);
	free(pixels);
	return tex;
}
	
/* Use this texture in the current shader*/
void texture_select(int tex) 
{
	glBindTexture(GL_TEXTURE_2D, tex);
}
