/*
Matrix/vector library
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
