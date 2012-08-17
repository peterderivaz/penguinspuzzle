/*
Header file for shader creation.
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

