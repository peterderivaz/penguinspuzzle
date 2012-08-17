/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* Code to load data from a file embedded in the binary.
*
*/

typedef struct embed_file_s
{
	char *start;
	int pos;
} EMBED_FILE;

extern EMBED_FILE *embed_fopen(char *a,char *type);

extern void embed_fseek(EMBED_FILE *f, int pos, int dir);

extern int embed_fread(void *p, int size, int n, EMBED_FILE *f);

extern void embed_fclose(EMBED_FILE *f);

