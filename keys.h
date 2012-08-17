/*
 Copyright (c) 2012 Peter de Rivaz

 WebGL version at http://penguinspuzzle.appspot.com

 Key detection
*/

#if(0)
#define KEY_Q  81      
#define KEY_U  85     
#define KEY_R  82
#define KEY_Z  90
#define KEY_BACKSPACE  8
#define KEY_SPACE  32	
#define KEY_LEFT  37     
#define KEY_UP  38    
#define KEY_RIGHT  39     
#define KEY_DOWN  40
#define KEY_ESCAPE  27 
#else
#define KEY_ESCAPE 1
#define KEY_LEFT 0x69
#define KEY_UP 0x67
#define KEY_RIGHT 0x6a
#define KEY_DOWN 0x6c
#define KEY_Q 0x10
#define KEY_R 0x13
#define KEY_U 0x16
#define KEY_P 0x19
#define KEY_A 0x1e
#define KEY_N 0x31
#define KEY_Z 0x2c
#define KEY_S 0x1f
#define KEY_SPACE 0x39
#define KEY_BACKSPACE 0xe
#endif
extern int currentlyPressedKeys[256];
extern int key_pressed;

extern void update_keys(void);
extern void clear_keys(void);


