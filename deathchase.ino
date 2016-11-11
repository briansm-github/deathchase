#include "Arduboy.h"
#include "trees.h"
#include "bikes.h"

#define DENSITY 253 // forsst density, lower=more dense (maybe 254, 253 or 252)

Arduboy arduboy;

int zmstart[128];
int zmscale[128];
byte zbuff[128];

int x;
unsigned int tmap[128];

//---------------------------------------------------------
void print(int x, int y, char *s, int inv)
{
  while(*s!=0) {arduboy.drawChar(x,y,*s,!inv,inv,1); x+=6; s++;}
}

//----------------------------------------------------------

int title()
{
  arduboy.fillScreen(1);
  print(24,8,"Deathchase Demo",1);
  print(0,24,"'A' = Pause",1);
  print(0,40,"'B' = Begin",1);
  
  arduboy.display();
  while(!arduboy.pressed(B_BUTTON));
}
//----------------------------------------------------------
int draw_tree(int depth, int ratio)
{
  int i,w;
  
  w=twidth[depth]; if (x+w>128) w=128-x;
  for (i=ratio*w/256; i<w; i++) {
    arduboy.drawBitmap(x,0,&trees[tfirst[depth]+i*8],1,64,1);
    zbuff[x]=depth;
    x++; if (x==128) return(0);
  }
}

//----------------------------------------------------------
// simple ray-caster for tree renders
int draw_trees()
{
   int z,zm,zs;
   unsigned int b;

   for (x=0; x<128; x++) zbuff[x]=-1;   
   x=0;
   while(x!=128) {
     z=16; zm=zmstart[x]; zs=zmscale[x]; b=32768;
     while(z!=0) {
       //printf("x=%i, z=%i, zm/256=%i\n",x,z,zm/256);
     if (tmap[zm/256] & b)  {
         if (zbuff[x-1]<z) {
           x-=(zm%256)*twidth[z]/256; if (x<0) x=0;
           draw_tree(z,0);
         } else draw_tree(z,zm%256);
       }
       z--; b/=2;
       if (z==15) zm+=zs/3; else if (zm==14) zm+=zs/2; else zm+=zs;
       if (z==0) draw_tree(z,255); // background only
     }
   }
}

//----------------------------------------------------------
int draw_bike(int dir)
{
  int i,n;
  
  for (i=0; i<32; i++)
   arduboy.drawBitmap(i+48,48,&bmask[dir*64+i*2],1,16,0);
  for (i=0; i<32; i++)
   arduboy.drawBitmap(i+48,48,&bikes[dir*64+i*2],1,16,1);
}
//-----------------------------------------------------
void gameloop() {
  int i,n,z;
  int dir=0;
  
  int frame=0,found;

  for (i=0; i<128; i++) tmap[i]=0;
  for (i=0; i<128; i++) {
    //zmstart[i]=61*256+i*5*256/128;
    zmstart[i]=61*256+i*5*2;
    //zmscale[i]=(i-64)*61*256/64/16;
    zmscale[i]=(i-64)*61/4;
  }
  
  while(1) {
    arduboy.fillScreen(0);
    for (i=0; i<128; i++) {tmap[i]<<=1; if (rand()%256>DENSITY) tmap[i]|=1;}
    draw_trees();
    draw_bike(dir);   
    arduboy.display();

    // detect a tree-crash.....
    if (tmap[63]&32768) delay(2000);
    if (tmap[64]&32768) delay(2000);
    
    dir=0;
    if (arduboy.pressed(LEFT_BUTTON)) {dir=1; for (i=127; i>1; i--) tmap[i]=tmap[i-2];}
    if (arduboy.pressed(RIGHT_BUTTON)) {dir=2; for (i=0; i<126; i++) tmap[i]=tmap[i+2];}
    if (arduboy.pressed(A_BUTTON)) { // pause
         print(40,0," PAUSED ",0); arduboy.display();
         while(arduboy.pressed(A_BUTTON));
         while(!arduboy.pressed(A_BUTTON)) delay(10);
         while(arduboy.pressed(A_BUTTON));      
    }
    frame++;
    delay(50);
  }
}

//-----------------------------------------------------
void setup() {
  arduboy.begin();
}

void loop() {
  title();
  gameloop();
}
