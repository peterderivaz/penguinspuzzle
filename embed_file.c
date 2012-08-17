/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to load and select a texture.
* We only actually need an integer to specify a texture.
*
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "embed_file.h"

EMBED_FILE *embed_fopen(char *a,char *type)
{
	EMBED_FILE *f;
	NEW(f);
	f->start=a;
	f->pos=0;
	return f;
}

void embed_fseek(EMBED_FILE *f, int pos, int dir)
{
	assert(dir==SEEK_SET);
	f->pos=pos;
}

int embed_fread(void *p, int size, int n, EMBED_FILE *f)
{
	memcpy(p,f->start+f->pos,size*n);
	f->pos+=size*n;
	return n;
}

void embed_fclose(EMBED_FILE *f)
{
	assert(f);
	free(f);
}

