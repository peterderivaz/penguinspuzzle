/*
 Copyright (c) 2012 Peter de Rivaz

 WebGL version at http://penguinspuzzle.appspot.com

 Key detection
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
		char *h;
		if (!r) break;
		h=strstr(r,"Handlers");
		if (!h) continue;
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
		if (c==KEY_ESCAPE)
			exit(0);
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
