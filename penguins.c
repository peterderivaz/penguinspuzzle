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
* Main game logic
*/

#include "global.h"
#include "matrix.h"
#include "shaders.h"
#include "buffer.h"
#include "model.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "view.h"
#include "penguins.h"
#include "texture.h"
#include "keys.h"
#include "audio.h"
#include "embed_file.h"

#define ICESPEED 3 
int moveable[OLAST];
int fixed[OLAST];
int fakeObjects[OLAST];
float objectHeights[OLAST];
PENGUIN_T *heropeng=NULL;
static int sounds_allowed=1; // Stop making too many whee sounds when sliding on ice

extern char _binary_sound_splosh_pi_start;
extern char _binary_sound_splosh_pi_end;
extern char _binary_sound_woohoo_pi_start;
extern char _binary_sound_woohoo_pi_end;
extern char _binary_sound_wheee_pi_start;
extern char _binary_sound_wheee_pi_end;
extern char _binary_sound_whooo_pi_start;
extern char _binary_sound_whooo_pi_end;
extern char _binary_sound_cheers_pi_start;
extern char _binary_sound_cheers_pi_end;
extern char _binary_sound_boom_pi_start;
extern char _binary_sound_boom_pi_end;
extern char _binary_sound_shot_pi_start;
extern char _binary_sound_shot_pi_end;

#define LEV(x) extern char _binary_data_lev ## x ## _pi_start;
LEV(0 )LEV(1 )LEV(2 )LEV(3 )LEV(4 )LEV(5 )LEV(6 )LEV(7 )LEV(8 )LEV(9 )LEV(10)LEV(11)LEV(12)
LEV(13)LEV(14)LEV(15)LEV(16)LEV(17)LEV(18)LEV(19)LEV(20)LEV(21)LEV(22)LEV(23)LEV(24)LEV(25)
LEV(26)LEV(27)LEV(28)LEV(29)LEV(30)LEV(31)LEV(32)LEV(33)LEV(34)LEV(35)LEV(36)LEV(37)LEV(38)
LEV(39)LEV(40)LEV(41)LEV(42)LEV(43)LEV(44)LEV(45)LEV(46)LEV(47)LEV(48)LEV(49)
#undef LEV

#define LEV(x) &_binary_data_lev ## x ## _pi_start,

char *level_data[]={
LEV(0 )LEV(1 )LEV(2 )LEV(3 )LEV(4 )LEV(5 )LEV(6 )LEV(7 )LEV(8 )LEV(9 )LEV(10)LEV(11)LEV(12)
LEV(13)LEV(14)LEV(15)LEV(16)LEV(17)LEV(18)LEV(19)LEV(20)LEV(21)LEV(22)LEV(23)LEV(24)LEV(25)
LEV(26)LEV(27)LEV(28)LEV(29)LEV(30)LEV(31)LEV(32)LEV(33)LEV(34)LEV(35)LEV(36)LEV(37)LEV(38)
LEV(39)LEV(40)LEV(41)LEV(42)LEV(43)LEV(44)LEV(45)LEV(46)LEV(47)LEV(48)LEV(49)
};

#define PlaySound(x,...)
#define play_sound(snd) audio_play(&_binary_sound_ ## snd ## _pi_start,&_binary_sound_ ## snd ## _pi_end);
#define alert(x)

static void level_groundpt(LEVEL_T *l, float pt[3], PENGUIN_T *p);
static float level_stack_height(LEVEL_T *l, PENGUIN_T *p);
static int level_getsquare(LEVEL_T *l, float x,float y);
static void level_switchpengs(LEVEL_T *l);
static void level_remove(LEVEL_T *l, PENGUIN_T *p);
static void level_movepenguin(LEVEL_T *l, PENGUIN_T *p, int checkice);
static BLOCKTYPE_T level_crackice(LEVEL_T *l,int sq);
static void level_crackice_r(LEVEL_T *l,int sq);
static PENGUIN_T *level_newpenguin(LEVEL_T *l, int sq, OBJECT_T t, int skip_place);

/* Return distance between two objects */
static float peng_dist(PENGUIN_T *a, PENGUIN_T *b)
{
	float dx = a->x-b->x;
	float dy = a->y-b->y;
	return sqrt(dx*dx+dy*dy);
}

static int is_baby(PENGUIN_T *p)
{
	return p->size<4;
}

//Turns number into +-1 or 0 depending on sign
static int mysign(float x) 
{
	if (x>0.1f) return 1;
	if (x<-0.1f) return -1;
	return 0;
}

/*
This should store the fact that we have completed level <levnum>.
*/
static void complete_level(int levnum) 
{
	// TODO
}

/*
Prepare the main level state object.
Pass in the texture to be used to draw the level.
*/
LEVEL_T *level_new(int t) 
{
	LEVEL_T *l;
	NEW(l);
	l->t = t;
	l->mode = 0;
	l->grid = 64;
	l->sz = 40.0;
	l->levnum = 0;
	l->backup=0;
	l->L=NULL;
	l->view=view_new();
		
	moveable[PENG]=1;
	moveable[BABYPENG]=1;
	moveable[CHEST]=1;
	moveable[BOMB]=1;
	moveable[FISHPAIL]=1;
	fixed[TREE]=1;
	fixed[OGUN]=1;
	fixed[OLASER]=1;
	fakeObjects[CAMERA] = 1;
	fakeObjects[NONE] = 1;
	objectHeights[PENG]=1.0;
	objectHeights[BABYPENG]=0.5;
	objectHeights[FISHPAIL]=0.5;
	objectHeights[CHEST]=1.0;
	objectHeights[TREE]=2.0;

	return l;
}

// If there are contents to the square then our height is determined based on what we are standing on.
// Fills in a vec3 describing the location of this point.  Coordinates specified as integers plus floating point offset 0.0 to 1.0 to move to next square
// If <p> is not NULL then we are trying to find the height of the specified penguin.
static void level_pt(LEVEL_T *l, float pt[3], int x, int y, float dx, float dy, PENGUIN_T *p)
{
	SQUARE_T cA={BSEA,NULL,0,0,0};
	SQUARE_T *A=&cA;

	if (0<=x && x<l->w && 0<=y && y<l->h)
	{
		A=&(l->L[x*l->grid+y]);
	}
	float H=l->sz*(A->h+dx*A->dx+dy*A->dy);
	if (A->block==BSEA)
		H = -l->sz;
	pt[0] = l->sz*(x+dx);
	pt[1] = l->sz*(y+dy);
	pt[2] = H;
	if (A->contents && p && p!=A->contents)
	{
		float g[3];
		// This square has a stack of objects
		// Determine height based on height of guy on the bottom
		level_groundpt(l,g,A->contents);
		pt[2]=g[2];
		for(PENGUIN_T *hit=A->contents;hit && hit!=p;hit=hit->contents)
		{
			if (peng_dist(hit,p)<l->sz*0.8)  // If the objects are too far apart (e.g. multiple sliders) then don't draw on top
				pt[2]+=objectHeights[hit->type]*l->sz;
		}
	}
}
// compute vec3 position of a penguin standing on the ground (or in a stack)
void level_groundpt(LEVEL_T *l, float pt[3], PENGUIN_T *p)
{
	float x = p->x;
	float y = p->y;
	x /= l->sz;
	y /= l->sz;
	int ix=floor(x);
	int iy=floor(y);
	level_pt(l,pt,ix,iy,x-ix,y-iy,p);
}

/* Given integer coordinates return blocktype of the square */
static BLOCKTYPE_T level_block(LEVEL_T *l, int x, int y)
{
	return l->L[x*l->grid+y].block;
}


// Compute the height for the float coordinates x,y w.r.t. square sq
float level_square_height(LEVEL_T *l, int sq, float dx, float dy)
{
	SQUARE_T *A=&l->L[sq];
	float H=l->sz*(A->h+dx*A->dx+dy*A->dy);
	if (A->block==BSEA)
		H = -l->sz;
	return H;
}

// Returns the height of this penguin
static float level_calc_potential(LEVEL_T *l, PENGUIN_T *p)
{
	return level_square_height(l,p->square,0.5f,0.5f)+level_stack_height(l,p);
}

// Returns the height of all the things the penguin is standing on
float level_stack_height(LEVEL_T *l, PENGUIN_T *p)
{
	float s=0;
	for(PENGUIN_T *hit=p->standingon;hit;hit=hit->standingon)
		s+=objectHeights[hit->type]*l->sz;
	return s;
}

static void face_init(short face[3],short a, short b, short c)
{
	face[0]=a;
	face[1]=b;
	face[2]=c;
}

/*
When the contents of the level change call this to rebuild the model
*/	
void level_make_model(LEVEL_T *l)
{
	#define MAXPTS 10000
	#define MAXFACES 10000
	// The level is an array of square,contents,height,dx,dy
	//# Note that arrays are stored in [x*sz+y] order to match C structures
	float pts[MAXPTS][3];
	float tex[MAXPTS][3];
	short faces[MAXFACES][3];
	int numpts=0;
	int numfaces=0;
	int w=l->w;
	int h=l->h; 
	float deltas[4][2] = {{0,0},{1,0},{1,1},{0,1}};
	int TC[]={0,4,1,2,6,7,7,8,9,9,15,7};
		
	for(int y=0;y<h;y++)
	{
		for(int x=0;x<w;x++)
		{ 
			int N=numpts;
			BLOCKTYPE_T c=level_block(l,x,y);
			if (c==BSEA) continue;
			int si=TC[c];
			float sx=(si&3)*0.25f;
			float sy=(si>>2)*0.25f;
			for (int i=0;i<4;i++)
			{
				float dx=deltas[i][0];
				float dy=deltas[i][1];
				level_pt(l,pts[numpts],x,y,dx,dy,NULL);
				vec3_init(tex[numpts++],sx+0.25*dx,sy+0.25*dy,1.0);
			}
			face_init(faces[numfaces++],N,N+1,N+2);
			face_init(faces[numfaces++],N,N+2,N+3);
			for(int i=0;i<4;i++)
			{
				float dx=deltas[i][0];
				float dy=deltas[i][1];
				float dx2=deltas[(i+1)&3][0];
				float dy2=deltas[(i+1)&3][1];
				float ex=dx+dx2-1;
				float ey=dy+dy2-1;
				float a[3];
				level_pt(l,a,x,y,dx,dy,NULL);
				float b[3];
				level_pt(l,b,x,y,dx2,dy2,NULL);
				float a2[3];
				level_pt(l,a2,x+ex,y+ey,dx-ex,dy-ey,NULL);
				float b2[3];
				level_pt(l,b2,x+ex,y+ey,dx2-ex,dy2-ey,NULL);
				if ((a[2]>a2[2]+0.1f) || (b[2]>b2[2]+0.1f))
				{
					N=numpts;
					vec3_copy(pts[numpts],a);
					vec3_copy(pts[numpts+1],b);
					vec3_copy(pts[numpts+2],b2);
					vec3_copy(pts[numpts+3],a2);
					for(int j=0;j<4;j++)
					{
						float dx3=deltas[j][0];
						float dy3=deltas[j][1];
						vec3_init(tex[numpts++],0.25f+0.25f*dx3,0.75f+0.25f*dy3,0.6f);
					}
					face_init(faces[numfaces++],N,N+2,N+1);
					face_init(faces[numfaces++],N,N+3,N+2);
				}
			}
		}
	}
	if (l->buf) buffer_free(l->buf);
	l->buf = buffer_new(pts,tex,faces,numpts,numfaces);
}

/*
Draw the underlying grid of tiles
*/
void level_draw_tiles(LEVEL_T *l, SHADER_T *s) 
{
	texture_select(l->t);
	buffer_draw(l->buf,s);
}

/*
Draw all objects in the level.
If <transparent> then draw objects requiring transparency.

TODO sort these in back to front order.
*/
void level_draw_objects(LEVEL_T *l, SHADER_T *s, int transparent) 
{
	float world[4][4];
	for(int i=0;i<l->numpengs;i++)
	{
		PENGUIN_T *p = &l->P[i];
		MODEL_T *m = models[p->type];
		if (!m) continue;
		
		if (!transparent && p->type==CHEST) continue;
		if (transparent && p->type!=CHEST) continue;
		mat4_identity(world);
		
		float C[3];
		level_groundpt(l,C,p);
		mat4_translate(world,C);

		if (abs(p->thetaf)>1)
		{
			mat4_rotatez(world,p->thetaf);
		}
		if (p->bounce>0)
		{
			mat4_rotatex(world,p->bounce*2);
		}

		shader_world(s,world);
		model_draw(m,s);
		if (p->type==PENG && p->numfish>0)
		{
			vec3_init(C,0,0,40);
			mat4_translate(world,C);
			shader_world(s,world);
			model_draw(models[FISHPAIL],s);
		}
	}
}

/* Now draw all fragment objects (using a billboard shader) */
void level_draw_fragments(LEVEL_T *l, SHADER_T *s, MODEL_T *expl) 
{
	float world[4][4];
	for(int i=0;i<l->numpengs;i++)
	{
		PENGUIN_T *p = &l->P[i];
		if (p->type == FRAGMENT)
		{
			mat4_identity(world);
			float A[3]={p->x,p->y,p->d};
			mat4_translate(world,A);
			shader_world(s,world);
			shader_blend(s,p->steps,0,0,0);
			model_draw(expl,s);
		}
	}
}

/*
Stores into the backup slot and moves forward.
numbackups increased if storing beyond range
*/
void level_snapshot(LEVEL_T *l)
{
	BACKUP_T *b = &l->snapshots[l->backup];
	b->numpengs = l->numpengs;
	memcpy(b->P,l->P,sizeof(PENGUIN_T)*l->numpengs);
	memcpy(b->L,l->L,sizeof(SQUARE_T)*l->w*l->h);
	l->backup = (l->backup+1)%MAXBACKUPS;
	l->numbackups=max(l->numbackups,l->backup);
}

void level_clear_snapshots(LEVEL_T *l)
{
	l->numbackups=0;
	l->backup=0;
}

void level_undo(LEVEL_T *l)
{
	if (!l->numbackups) return;
	l->backup=(l->backup-1+l->numbackups)%l->numbackups;
	
	//printf("Undo %d:%d\n",l->backup,l->numbackups);
	BACKUP_T *b = &l->snapshots[l->backup];
	l->numpengs = b->numpengs;
	memcpy(l->P,b->P,sizeof(PENGUIN_T)*l->numpengs);
	memcpy(l->L,b->L,sizeof(SQUARE_T)*l->w*l->h);
	
	level_make_model(l);
}

/*
Load binary data from a file and store it in the level object 
*/
void level_load(LEVEL_T *l)
{
	//char fname[256];
	//sprintf(fname,"data/lev%d.pi",l->levnum%50);
	//EMBED_FILE *fid=embed_fopen(fname,"rb");
	EMBED_FILE *fid=embed_fopen(level_data[l->levnum%50],"rb");
	assert(fid);
	int w,h,numpengs;
	embed_fread(&w,sizeof(int),1,fid);
	embed_fread(&h,sizeof(int),1,fid);
	embed_fread(&numpengs,sizeof(int),1,fid);
	l->w = w;
	l->h = h;
	l->numpengs = numpengs;
	if (l->L) free(l->L);
	l->L = calloc(sizeof(SQUARE_T),l->grid*l->grid);
	
	l->mode=0;
	l->delay=20; // Don't allow any keypresses for a while
	
	// Offset out by 32 to give us extra drawing space
	int off=8;
	for(int y=0;y<l->grid;y++)
	{
		//if (y>=off && y<this.h+off) continue;
		for(int x=0;x<l->grid;x++)
		{
			//if (x>=off && x<this.w+off) continue;
			SQUARE_T *s=&(l->L[x*l->grid+y]);
			CLEAR(s);
			s->block=BSEA;
		}
	}
	
	for(int x=0;x<l->w;x++)
	{
		embed_fread(&(l->L[(x+off)*l->grid+off]),sizeof(SQUARE_T),l->h,fid);
	}
	
	l->h=min(l->grid,l->w+off*2);
	l->w=min(l->grid,l->h+off*2);
	level_make_model(l);
	
	embed_fread(&(l->P),sizeof(PENGUIN_T),l->numpengs,fid);
	for(int i=0;i<l->numpengs;i++)
	{
		PENGUIN_T *p = &(l->P[i]);
		if (p->state==PUZZLER)
			heropeng=p;
		if (p->type==PENG && is_baby(p))
			p->type=BABYPENG;
		if (p->type==OGUN || p->type==OLASER)
			p->thetaf += 180;
		p->speed=ICESPEED;
		p->x+=off*l->sz;
		p->y+=off*l->sz;
		if (p->type==CAMERA)
		{
			p->dx+=off*l->sz;
			p->vx+=off*l->sz;
			p->dy+=off*l->sz;
			p->vy+=off*l->sz;
		}
		p->contents=NULL;
		p->standingon=NULL;
		if (!(fakeObjects[p->type]))
		{
			int ns = level_getsquare(l,p->x,p->y);
			if (ns>=0)
			{
				PENGUIN_T *on = l->L[ns].contents;
				p->contents = on;
				if (on)
					on->standingon=p;
				l->L[ns].contents=p;
			}
			p->square=ns;
		}
		p->numfish=0;
		p->bounce=0;
	}
	// Capture the initial snapshot
	l->backup=0;
	level_snapshot(l);
	embed_fclose(fid);
}
/*
Level.prototype.save = function()
{
	// First go through and compute extent of the level
	var maxx=0;
	var maxy=0;
	var minx=1000;
	var miny=1000;
	for(var y=0;y<this.h;y++)
	{
		for(var x=0;x<this.w;x++)
		{
			if (l->L[x*l->grid+y][0]!=BSEA)
			{
				maxy=Math.max(maxy,y);
				maxx=Math.max(maxx,x);
				miny=Math.min(miny,y);
				minx=Math.min(minx,x);
			}
		}
	}
	var X = [];
	for(var x=minx;x<=maxx;x++)	
	{
		var A=[];
		for(var y=miny;y<=maxy;y++)
		{
			var s = l->L[x*l->grid+y];
			A.push([s[0],-1,s[2],s[3],s[4]]);
		}
		X.push(A);
	}
	var P = [];
	for(var i=0;i<this.p->length;i++)
	{
		var p = this.P[i];
		var p2={};
		if (p->type==NONE) continue;
		p2.x=p->x-l->sz*minx;
		p2.y=p->y-l->sz*miny;
		p2.dx=p->dx;
		p2.dy=p->dy;
		p2.vy=p->vy;
		p2.vx=p->vx;
		if (p->type==CAMERA)
		{
			p2.dx-=minx*l->sz;
			p2.vx-=minx*l->sz;
			p2.dy-=miny*l->sz;
			p2.vy-=miny*l->sz;
		}
		p2.thetaf=p->thetaf;
		p2.speed=p->speed;
		p2.type=p->type;
		p2.state=p->state;
		p2.steps=p->steps;
		p2.numfish=p->numfish;
		p2.d=p->d;
		p2.size=p->size;
		p->push(p2);
	}
	document.getElementById("output").innerHTML="Level="+this.levnum+"<br>"+JSON.stringify([X,P]);		
}*/

/*
Return the index for the square corresponding to coordinates x,y
*/
int level_getsquare(LEVEL_T *l, float x,float y) 
{
	int nx = floor(x/l->sz);
	int ny = floor(y/l->sz);
	if (0<=nx && nx<l->w && 0<=ny && ny<l->h)
		return nx*l->grid+ny;
	return -1;
}

/*
Stop the given object from being displayed.
This is called if an object is destroyed for some reason so can trigger end of level.
*/
void level_delete_object(LEVEL_T *l, PENGUIN_T *p) 
{
	if (!p) return;
	if (p->state==PUZZLER)
		level_switchpengs(l);
	level_remove(l,p);
	if (p==heropeng)
	{
		l->mode=2;
		l->delay=10;
		p->bounce=0;
		key_pressed=0;
		return; // don't actually delete the penguin so we can still see it in the water...
	}
	p->type=NONE;  
}

// Take an object away from its square
static void level_remove(LEVEL_T *l, PENGUIN_T *p) 
{	
	if (p->standingon)
	{
		p->standingon->contents=p->contents; // Drop anything above us down one
	}
	else if (p->square>=0)
	{
		l->L[p->square].contents=p->contents;  // Remove pointer if we are at the base of the list
	}
	if (p->contents)
	{
		p->contents->standingon=NULL;
	}
	p->contents=NULL;
	p->standingon=NULL;
}

#define BOUNCETIME 10

/*
If push is 1 then it means there is a penguin pushing this (rather than just a collision)
Try to push an object in its currently set up vx,vy direction.
If it cannot move p->steps will be cleared and we return 0
otherwise changes square to next location
Also sets bounce so that we can show some movement (probably by forward rotation)
*/
int level_trymove(LEVEL_T *l,PENGUIN_T *p,int push)
{
	int ns = level_getsquare(l,p->x+p->vx*p->steps,p->y+p->vy*p->steps);
	PENGUIN_T *lastobj=NULL;
	// We transfer our momentum if we have a moving object
	if (ns>=0)
	{
		// First lets look to see if the wall is too high to slide over
		// Or if we have insufficient potential to get to our edge
		float newh = level_square_height(l, ns,      0.5f+p->vx*p->steps*-0.5f/l->sz,0.5f+p->vy*p->steps*-0.5f/l->sz);
		float oldh = level_square_height(l,p->square,0.5f+p->vx*p->steps*0.5f/l->sz ,0.5f+p->vy*p->steps*0.5f/l->sz)+level_stack_height(l,p);
		if (newh>oldh+l->sz*0.3f || oldh>p->potential+5)
		{
			p->thetaf+=p->vturn*p->steps;
			p->potential=level_calc_potential(l,p);
			p->steps=0;
			p->bounce=BOUNCETIME;
			return 0;
		}
		// Now lets look to see if we need to push a penguin
		
		// In the new scheme we never run out of momentum until we reach half height
		// Check for this when looking to see if we continue moving!
		float sqh = level_square_height(l,ns,      0.5f,0.5f);
		
		for(PENGUIN_T *hit = l->L[ns].contents;hit;hit=hit->contents)
		{
			float objh = objectHeights[hit->type]*l->sz;
			sqh+=objh;
			if (oldh>(sqh-5))
			{
				// We are going to move above this object, so don't collide
				lastobj=hit;
				continue;
			}
			if (fixed[hit->type])
			{
				p->thetaf+=p->vturn*p->steps;
				p->potential=level_calc_potential(l,p);
				p->steps=0;
				p->bounce=BOUNCETIME;
				return 0;
			}
			else if (p->type==PENG && hit->type == FISHPAIL)
			{
				p->numfish++;
				PlaySound('woohoo');
				play_sound(woohoo);
				level_delete_object(l,hit);
				break;
			}
			else if (p->type==BABYPENG && hit->type == FISHPAIL)
			{
				PlaySound('woohoo');
				play_sound(woohoo);
				level_delete_object(l,hit);
				if (p->state==BOPPING)
				{
					p->state = FED;
					while(p->state==FED)
						level_switchpengs(l);
				}
				else
					p->numfish++;
			}
			else if (p->type==PENG && p->numfish>0 && hit->type == BABYPENG && hit->state==BOPPING)
			{
				p->numfish--;
				PlaySound('woohoo');
				play_sound(woohoo);
				p->thetaf+=p->vturn*p->steps;
				p->steps=0;
				p->state=FED;
				p->potential=level_calc_potential(l,p);
				hit->state=PUZZLER;
				hit->bounce=BOUNCETIME; // Prevents an update on this cycle
				hit->steps=0;
				return 0;
			}
			// TODO feed babypeng
			else if (moveable[hit->type])
			{
				// Warning, this may not check for certain actions!
				hit->x+=hit->vx*hit->steps;
				hit->y+=hit->vy*hit->steps;
				hit->thetaf+=hit->vturn*hit->steps;
				hit->vx=p->vx;
				hit->vy=p->vy;
				hit->steps=p->steps;
				hit->speed=p->speed;
				hit->vturn=0;
				hit->thetaf=p->thetaf+p->vturn*p->steps;
				// When transferring momentum take account of the difference in height (allows us to push uphill)
				hit->potential=level_calc_potential(l,hit)+p->potential-level_calc_potential(l,p);
				if (!level_trymove(l,hit,push)) // Blocked
				{
					p->thetaf+=p->vturn*p->steps;
					p->bounce=BOUNCETIME;
					p->steps=0;
					p->potential=level_calc_potential(l,p);
					return 0;
				}
				level_movepenguin(l,hit,1);
				if (!push) // If sliding then we transfer our momentum instead.
				{
					p->thetaf+=p->vturn*p->steps;
					p->bounce=BOUNCETIME;
					p->steps=0;
					p->potential=level_calc_potential(l,p);
					return 0;
				}
			}
			else
			{
				alert("Unknown type "+hit->type);
			}
		}
	}
	// May want to check here what we have landed on!
	
	// This is the only place that squares change
	// May be easier to maintain a doubly linked list.
	level_remove(l,p);
	// Place us into the new square at appropriate location
	// Should not be possible for there to be something above lastobj or we would collide with it!
	p->square=ns;
	if (lastobj)
	{
		lastobj->contents=p;
		p->standingon=lastobj;
	}
	else
	{
		l->L[p->square].contents=p;
		p->standingon=NULL;
	}
	return 1;
}
   
static void level_movepenguin(LEVEL_T *l, PENGUIN_T *p, int checkice) 
{
	if (!checkice)
	{
		checkice = 1;
	}
	float sz = l->sz;
	SQUARE_T res_s={BSEA,NULL,0,0,0};
	SQUARE_T *res=&res_s;
	if (p->square>=0) res = &(l->L[p->square]);
	BLOCKTYPE_T b=res->block;
	float vx=res->dx;
	float vy=res->dy; 
	if (p->steps<=0)
	{
		// Just check for ice below us in case we need to be made to slide off in a new direction
		if (p->standingon)
		{
			return;
		}
		if (b!=BICE)
		{
			return;
		}
		vx=mysign(vx);
		vy=mysign(vy);
		if (vx==0 && vy==0) return;
		p->speed = ICESPEED;
		p->vx = -p->speed*vx;
		p->vy = -p->speed*vy; 
		p->vturn=0;
		p->potential=level_calc_potential(l,p);
	}
	else
	{
		// Take a step
		p->x += p->vx;
		p->y += p->vy;
		p->thetaf += p->vturn;
		p->steps -= 1;
		// TODO Ski jumping...
	}
	if  (p->steps>0) return;
	// Place into rounded location
	p->x = sz*floor(p->x/sz)+sz*0.5f;
	p->y = sz*floor(p->y/sz)+sz*0.5f;
	// Round angle to 90 degrees
	//printf("Before = %g\n",(double)p->thetaf);
	p->thetaf = (((360+45+p->thetaf)/90)&3)*90;  
	//printf("After = %g\n",(double)p->thetaf);
	// Now should look at what we have landed on
	if (p->type==PENG && b==BCRACKED)  // This applies even if the PENG is on top!
	{
		b=level_crackice(l,p->square);
		//l->mode=2;
		//l->delay=10;
		//keyPressed=0;
		PlaySound('boom');
		play_sound(boom);
	}
	int onice=0;
	if (p->standingon)
	{
		if (p->standingon->type==CHEST)
			onice = 1;
	}
	else
	{
		if (b==BICE)
			onice = 1;
	}
	if (onice && (p->vx || p->vy))
	{
		// Look to see if the slope is too steep for us (this may want to give us a chance to step off!)
		// Giving a little margin to prevent early stops should ensure we can get back up to our starting height
		if ((level_square_height(l,p->square,mysign(p->vx)*0.5f+0.5f,mysign(p->vy)*0.5f+0.5f)+level_stack_height(l,p))>(p->potential+5))
		{
			p->vx=p->vy=p->vturn=0;
		}
		else
		{
			p->steps = floor(sz/p->speed);
			if (level_trymove(l,p,0) && sounds_allowed)
			{
				if (p->type==BABYPENG)
				{
					PlaySound('wheee');
					play_sound(wheee);
					sounds_allowed=0;
				}
				else if (p->vturn)
				{
					PlaySound('whooo');
					play_sound(whooo);
					sounds_allowed=0;
				}
			}
		}
	}
	else if (p==heropeng && b==BEXIT)
	{
		l->mode=1;
		l->delay=10;
		key_pressed=0;
		complete_level(l->levnum);
		l->levnum++;
		PlaySound('audio1',1);
		play_sound(cheers);
		level_clear_snapshots(l);
	}
	else if ((p->type==PENG || p->type==BABYPENG) && b==BSEA)
	{
		PlaySound('splosh',1);
		play_sound(splosh);
		level_delete_object(l,p);
	}
	else if (p->type==CHEST && b==BSEA)
	{
		PlaySound('splosh',1);
		play_sound(splosh);
	}
	else if (p->type==CHEST && b==BCRACKED)
	{
		level_crackice(l,p->square);
		PlaySound('boom',1);
		play_sound(boom);
	}
	else if (b==BFRAGILE)
	{
		l->L[p->square].block=BCRACKED;
		level_make_model(l);
		PlaySound('shot',1);
		play_sound(shot);
	}
}

// Make this square have the height of the smallest corner
void level_flatten(LEVEL_T *l, int sq)
{
	// 3,4 are dx,dy
	SQUARE_T *a=&l->L[sq];
	a->h+=min(0,a->dx);
	a->h+=min(0,a->dy);
	a->dx=0;
	a->dy=0;
}

// Crack the ice and return the new block type
static BLOCKTYPE_T level_crackice(LEVEL_T *l,int sq)
{
	level_crackice_r(l,sq);
	level_make_model(l);
	return l->L[sq].block;
}

static void level_crackice_r(LEVEL_T *l,int sq)
{
	if (sq<=0) return;
	//if (!(sq in l->L)) return;
	BLOCKTYPE_T b = l->L[sq].block;
	if (b!=BCRACKED && b!=BFRAGILE) return;
	level_flatten(l,sq);
	l->L[sq].h-=1.0;
	l->L[sq].block=BICE;
	PENGUIN_T *p = level_newpenguin(l,sq,FRAGMENT,1);
	p->d=l->L[sq].h*l->sz;
	if (l->L[sq].h<-0.1)
	{
		l->L[sq].block=BSEA;
		while(1)
		{
			PENGUIN_T *p = l->L[sq].contents;
			if (!p) break;
			if (p->type==CHEST) break; // This floats!
			PlaySound("splosh",1);
			level_delete_object(l,p);
		}
	}
	level_crackice_r(l,sq-l->grid);
	level_crackice_r(l,sq+l->grid);
	level_crackice_r(l,sq+1);
	level_crackice_r(l,sq-1);
}

static void level_switchpengs(LEVEL_T *l)
{
	int found=0;
	for(int r=0;r<2;r++)
	for(int i=0;i<l->numpengs;i++)
	{
		PENGUIN_T *p = &l->P[i];
		if (p->state==PUZZLER)
		{
			found=1;
			p->state=FED;
		}
		else if (p->state==FED && found)
		{
			p->state=PUZZLER;
			p->bounce=10;
			return;
		}
	}
}

static int sounddelay=0;
void level_update(LEVEL_T *l)
{
	//var takeSnapshot=0;
	int left = currentlyPressedKeys[KEY_LEFT];
	int right = currentlyPressedKeys[KEY_RIGHT];
	int up = currentlyPressedKeys[KEY_UP];
	int down = currentlyPressedKeys[KEY_DOWN];
	int space = currentlyPressedKeys[KEY_SPACE];
	int backspace = currentlyPressedKeys[KEY_U] || currentlyPressedKeys[KEY_BACKSPACE];
	if (l->delay>0)
	{
		l->delay--;
		left = up = right = down = 0;
	}
	if (sounddelay>0)
	{
		sounddelay--;
	}
	if (currentlyPressedKeys[KEY_ESCAPE])
	{
		//window.location="http://penguinspuzzle.appspot.com";
		return;
	}
	
	if (currentlyPressedKeys[KEY_R])
	{
		currentlyPressedKeys[KEY_R]=0;
		level_load(l);
		return;
	}				
	if (l->delay==0 && l->mode>0)
	{
		// Just spin around heropeng!
		if (!key_pressed)
			return;
		if (l->mode==1)
		{
			level_load(l);
			l->delay=10;
			clear_keys();
		}
		else if (l->mode==2)
		{
			// Go into rewind mode if press left or right, or start from beginning if press up/down
			/*if (up || down)
			{
				var b=this.backup;
				this.backup=-1;
				this.undo();
				clear_keys();
				this.backup=b; // Restore backup so even if we restart we can still undo
				l->mode=0;
				l->delay=10;
			}
			else if (left || right)
			{
				this.retreatSnapshot();
				this.undo();
				l->mode=3;
				l->delay=10;
			}*/
			if (backspace)
			{
				currentlyPressedKeys[KEY_BACKSPACE]=0;
				currentlyPressedKeys[KEY_U]=0;
				level_undo(l);
				l->mode=0;
			}
			if (up || down || left || right || currentlyPressedKeys[KEY_R])
			{
				currentlyPressedKeys[KEY_R]=0;
				level_load(l);
				l->mode=0;
			}
		}
		/*else if (l->mode==3)
		{
			// Go into rewind mode if press left or right, or start if press up/down
			if (up || down || space)
			{
				l->mode=0;
				l->delay=10;
				clear_keys();
			}
			else if (left || right || backspace)
			{
				if (right)
				{
					if (!this.advanceSnapshot())
						this.retreatSnapshot()
				}
				else
				{
					if (!this.retreatSnapshot())
						this.advanceSnapshot();
				}
				this.undo();
				l->delay=10;
			}
		}*/
		return;
	}
	
	if (currentlyPressedKeys[KEY_N])
	{
		currentlyPressedKeys[KEY_N]=0;
		l->levnum++;
		level_load(l);
		level_clear_snapshots(l);
		return;
	}
	if (currentlyPressedKeys[KEY_P])
	{
		currentlyPressedKeys[KEY_P]=0;
		l->levnum--;
		level_load(l);
		level_clear_snapshots(l);
		return;
	}
	
	if (backspace)
	{
		currentlyPressedKeys[KEY_BACKSPACE]=0;
		currentlyPressedKeys[KEY_U]=0;
		level_undo(l);
		return;
	}
	if (l->mode>0)
		return;
	// Take a copy of the level and the penguins
	for(int i=0;i<l->numpengs;i++)
	{	
		PENGUIN_T *p = &l->P[i];
		if (p->bounce>0)
			p->bounce--;
		if ((p->type==PENG || p->type==BABYPENG) && p->state==PUZZLER && p->steps==0 && !(p->bounce>0))
		{
			// Can give some more keys, so allow sounds again
			sounds_allowed=1;
			// To prevent autorepeat, eat directional keypresses at this stage
			clear_keys();
			if (space)
			{
				level_switchpengs(l);
			}
			if ( (up || down || right || left))
			{
				level_snapshot(l);
				// May have slight problem with blocks not rewinding eactly unless heropeng is first in list?
				
				int target=0; // Target angle
				int allowReverse = 1;
				if (up)
					target=0;
				else if (down)
					target=180;
				else if (left)
					target=270;
				else if (right)
					target=90;
				int up_target=view_find_up(l->view);
				target=(target+up_target)%360;
				left = up = right = down = 0;
				int d=target-p->thetaf;
				if (d<=-179) d+=360;
				if (d>=179) d-=360;
				if (abs(d)<1)
				{
					up=1;
				}
				else if (allowReverse && abs(d+180)<1)
				{
					down=1;
				}
				else if (d<0)
				{
					left=1;
				}
				else
				{
					right=1;
				}
				
				{
					p->vx=p->speed*sin(target*3.1415f/180.0f);
					p->vy=-p->speed*cos(target*3.1415f/180.0f);
					p->vturn=0;
					p->steps=floor(l->sz/p->speed);
					if (down)
						p->vturn = 180;
					if (left)
						p->vturn = -90;
					if (right) 
						p->vturn = 90;
					p->vturn /= p->steps;
					p->potential = level_calc_potential(l,p); 
					SQUARE_T *x=&l->L[p->square];
					if (!(x->block==BICE && (x->dx!=0 || x->dy!=0)))
					{
						p->potential+=10; // Enough to get up half slope, but don't give any extra if on a slopy ice
					}
					level_trymove(l,p,0);
				}
			}
			
			level_movepenguin(l,p,0);
		}
		else if (moveable[p->type])
		{
			level_movepenguin(l,p,0);
		}
		else if (p->type==FRAGMENT)
		{
			p->steps++;
			if (p->steps>202)
				level_delete_object(l,p);
		}
	}
}

static PENGUIN_T *level_newpenguin(LEVEL_T *l, int sq, OBJECT_T t, int skip_place)
{
	int y = sq%l->grid;
	int x = floor(sq/l->grid);
	PENGUIN_T *p=NULL;
	int i;
	for(i=0;i<l->numpengs;i++)
	{
		p=&l->P[i];
		if (p->state==NONE)
		{
			break;
		}
	}
	if (i==l->numpengs)
	{
		p=&l->P[l->numpengs];
		l->numpengs++;
	}
	p->x=(x+0.5f)*l->sz;
	p->y=(y+0.5f)*l->sz;
	p->thetaf=0;
	p->d=4.0f;  
	p->size=8.0f;
	p->state=BOPPING;
	p->type=t;
	p->speed=ICESPEED;
	p->steps=0;
	p->numfish=0;
	p->bounce=0;
	p->dx=0;
	p->dy=0;
	p->vx=0;
	p->vy=0;
	//p->tol=0;
	//p->thetah=0;
	//p->thetac=0;
	//p->lastz=0;
	p->vturn=0;
	if (sq>=0 && !skip_place)
	{
		p->square=sq;
		PENGUIN_T *on=l->L[p->square].contents;
		if (on)
		{
			while(on->contents)
				on=on->contents;
			p->standingon=on;
			on->contents=p;
		}
		else
		{
			l->L[p->square].contents=p;
		}
	}
	else
	{
		p->square=-1;
	}
	p->standingon=NULL;
	//p->iden=0;
	//p->team=0;
	//alert("Added to "+sq+"="+x+","+y);
	return p;
}

// Find the closest camera
void level_update_view(LEVEL_T *l)
{
	static float lastTime=0;  // time in milliseconds
	PENGUIN_T *cam=NULL;
	lastTime+=30.0f;
	if (lastTime>100000) lastTime=0;
	//view.lookAt([0,0,0],[lastMouseX,-200,lastMouseY]);
	//return;
	float bestdist = 100000000;
	if (l->mode>0 && l->mode<=2)
	{
		float H[3];
		level_groundpt(l,H,heropeng);
		float s = 0.003f;
		if (l->mode==2)
			s=0.0003f;
		float a = 100;
		float dx=a*cos(lastTime*s);
		float dy=a*sin(lastTime*s);
		float B[3];
		B[0]=H[0]+dx;
		B[1]=H[1]+dy;
		B[2]=H[2]+50;
		view_look_at(l->view,H,B,0,0);
		return;
	}
	for(int i=0;i<l->numpengs;i++)
	{
		PENGUIN_T *p=&l->P[i];
		if (p->type!=CAMERA) continue;
		float dx=p->x-heropeng->x;
		float dy=p->y-heropeng->y;
		float camdist=dx*dx+dy*dy;
		if (camdist>bestdist) continue;
		cam=p;
		bestdist=camdist;
	}
	if (!cam)
	{
		float A[3]={0,0,0};
		float B[3]={0,-200,100};
		view_look_at(l->view,A,B,0,0);
		return;
	}
	float target[3];
	float eye[3];
	target[0]=cam->dx;
	target[1]=cam->dy;
	target[2]=cam->d;
	eye[0]=cam->vx;
	eye[1]=cam->vy;
	eye[2]=cam->size;
	view_look_at(l->view,target,eye,0,1);
}
