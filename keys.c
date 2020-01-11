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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keys.h"
#include "drm_gbm.h"

int currentlyPressedKeys[256];
static int fd=-1;
int key_pressed=0;

void clear_keys(void)
{
	int keys[]={KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_SPACE};
	for (int j=0;j<5;j++)
		currentlyPressedKeys[keys[j]]=0;
}

#include "linux/input.h"
static char *get_event_for(const char *name)
{
    char buf[256];
    static char fname[256];
    int i;
    int fd;
    for (i=0; i<8; i++)
    {
        sprintf(fname, "/dev/input/event%d", i);
        fd = open(fname, O_RDONLY|O_NONBLOCK);
        if (fd>=0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            close (fd);
			printf("E%d=%s\n",i,buf);
            if (strstr(buf, name)) {
                return fname;
            }
        }
    }
    return NULL;
}

/* Try to find which event queue is associated with keyboard by searching for lines of the form
H: Handlers=mouse0 event0
in  /proc/bus/input/devices */
static char *find_kbd_event(void)
{
	#define MAXC 100
	char s[MAXC];
	char *devfile="/proc/bus/input/devices";
	char *ans="/dev/input/event0";
	static char ans_str[MAXC];
	FILE *fid=fopen(devfile,"rb");
	if (!fid)
	{
		printf("Cannot open %s\n",devfile);
		return ans;
	}
	while(1)
	{
		char *r=fgets(s,MAXC,fid);
		char *h,*isdock;
		if (!r) break;
		h=strstr(r,"Name=");
		if (h)
		{
			isdock=strstr(r,"Motorola Mobility Motorola HD Dock");
			continue;
		}
		h=strstr(r,"Handlers");
		if (!h || isdock) continue;
		h=strstr(r,"kbd");
		if (!h) continue;
		h=strstr(r,"event");
		if (!h) continue;
		sprintf(ans_str,"/dev/input/event%c",h[5]);
		ans=ans_str;
		printf("Found keyboard handler at %s\n",s);
		break;
	}
	fclose(fid);
	return ans;
}

int flush_in(FILE *file)
{
    int ch;
    int flags;
    int fd;

    fd = fileno(file);
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
        return -1;
    }
    do {
        ch = fgetc(file);
    } while (ch != EOF);
    clearerr(file);
    if (fcntl(fd, F_SETFL, flags)) {
        return -1;
    }
    return 0;
}

void update_keys(void)
{
	if (fd<0)
	{
		//char *fname="/dev/input/event0";
		//char *fname=get_event_for("HID"); // "/dev/input/event0", may want to search for Keyboard as well?
		char *fname=find_kbd_event();
		fd=open(fname,O_RDONLY|O_NONBLOCK);
		assert(fd);
	}
	unsigned char b[16];
	//int readfds[1];
	//readfds[0]=fd;
	//if (select(1,readfds,NULL,NULL,NULL)<=0) return;
	while(1)
	{
		int num=read(fd,b,16);
		if (num<16) return;
		assert(num==16);
		int t=b[8]+256*b[9];
		if (t!=1) return;
		//int c= b[10]+256*b[11];
		int c = b[10];
		if (c==KEY_ESCAPE) {
			drm_gbm_finish();
			// drain keys
			flush_in(stdin);
			exit(0);
		}
		//printf("Pressed 0x%x\n",c);
		if (b[12]==1)
		{
			currentlyPressedKeys[c]=1;
			key_pressed=1;
		}
		else
		{
			//currentlyPressedKeys[c]=0;  // Sometimes this may lose keypresses, so may not want this?
		}
	}
}
