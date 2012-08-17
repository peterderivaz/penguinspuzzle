/*
* Copyright (c) 2012 Peter de Rivaz
*
* WebGL version at http://penguinspuzzle.appspot.com
*
* This contains code to load and select a texture.
* We only actually need an integer to specify a texture.
*
*/

/* Allocate a new texture and load it from a 24bit BMP file named <texname> */
extern int texture_new(char *texname);
	
/* Use this texture in the current shader*/
extern void texture_select(int tex);
