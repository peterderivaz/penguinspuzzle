/*
This file contains the source code for the different shaders used in the game.
_vs are the vertex shaders.
_fs are the fragment shaders.
*/

typedef struct buffer_s
{
	GLuint vbuf;
	int vbuf_num;
	GLuint ebuf;
	int ebuf_num;
	int ntris;
} BUFFER_T;

/*
Create a new buffer object from an array of vec3s for pts, and vec3s for texture coordinates.
Faces is an array of 3 shorts for each face.
*/ 
extern BUFFER_T *buffer_new(float pts[][3], float tex[][3], short faces[][3], int numpts, int numfaces);

/* Deallocate resources for the given buffer */
extern void buffer_free(BUFFER_T *b);

/*
Activate the shader first, then call this function to bind the buffer to the shader
*/
extern void buffer_select (BUFFER_T *buf, SHADER_T *s);

extern void buffer_draw(BUFFER_T *b, SHADER_T *s);

extern void buffer_drawfan(BUFFER_T *b, SHADER_T *s);

extern void buffer_drawstrip(BUFFER_T *b, SHADER_T *s);
