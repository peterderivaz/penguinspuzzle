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
* Key detection
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


