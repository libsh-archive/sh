// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
//a
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

#ifndef WIN32
#include <GL/glxew.h>
#endif


#include <sh/sh.hpp>
using namespace std;

#include "Camera.hpp"
#include "texture3d.hpp"

using namespace SH;

// defines


#define DPT_INTERNAL_FORMAT GL_RGBA8 
#define BT_INTERNAL_FORMAT GL_RGBA8 
#define DVT_INTERNAL_FORMAT /*GL_RGBA32F_ARB*/ GL_RGBA16 /*GL_FLOAT_RGBA32_NV */

//#define USE_HALF_FLOAT
#ifdef USE_HALF_FLOAT
 #define POSITIONS_INTERNAL_FORMAT INTERNAL_FORMAT 
 #define NUM_POS_BYTES 2
 #define POS_TYPE GL_HALF_FLOAT_NV
#else
 #define POST_INTERNAL_FORMAT GL_RGBA32F_ARB //GL_FLOAT_RGBA32_NV
 #define NUM_POS_BYTES 4 
 #define POS_TYPE GL_FLOAT
#endif

#define DVT_TAR /*GL_TEXTURE_RECTANGLE_NV*/ GL_TEXTURE_2D
#define POST_TAR /*GL_TEXTURE_RECTANGLE_NV*/ GL_TEXTURE_2D
#define DPT_TAR /*GL_TEXTURE_RECTANGLE_NV*/ GL_TEXTURE_2D
#define BT_TAR /*GL_TEXTURE_RECTANGLE_NV*/ GL_TEXTURE_2D

#define PBO

#define NUMMODES 1

#define SO1 0.05
#define SO2 0.05
#define SCALE 5
#define GRID_2D_RES 512
#define SQR_GRID_3D_RES 8
#define GRID_3D_RES 64 //(SQR_GRID_3D_RES*SQR_GRID_3D_RES)
#define NUM_GRID_CELLS 262144 //(GRID_3D_RES*GRID_3D_RES*GRID_3D_RES)

#define RCPSQR2 0.707106781186


/*#define A0 (4.0/9)
#define D0 -(2.0/3)

#define A1 (1.0/9)
#define B1 (1.0/3)
#define C1 (1.0/2)
#define D1 -(1.0/6)

#define A2 (1.0/36)
#define B2 (1.0/12)
#define C2 (1.0/8)
#define D2 -(1.0/24)*/

#define A0 (1.0/3)
#define D0 -(1.0/2)

#define A1 (1.0/18)
#define B1 (1.0/6)
#define C1 (1.0/4)
#define D1 -(1.0/12)

#define A2 (1.0/36)
#define B2 (1.0/12)
#define C2 (1.0/8)
#define D2 -(1.0/24)


#define ZEROVEL 1.0

#define TAU 2.1
#define INV_TAU (1.0f/TAU)

#define VELSTEP 0.1

#define SQR_NUM_PARTICLES 512 //1024 //GRID_2D_RES
#define NUM_PARTICLES /*1048576*/ 262144 //(SQR_NUM_PARTICLES*SQR_NUM_PARTICLES)
#define PCO 0.02

// forward declarations
void init_shaders(void);
void init_textures(void);
void init_FBO(void);
void initGL(void);

void init_streams(void);
void reset_streams(void);
void update_streams(void);

//vertexbuffers
void init_vertexbuffers(void);

void switchBuffers(void);
void setup_texouts();
void load_textures(void);
int gprintf(int x, int y, char* fmt, ...);
ShAttrib1f
fresnel (
   ShVector3f v, 
   ShNormal3f n, 
   ShAttrib1f eta
);


#ifndef WIN32
 Display *dpy;
 int screen;
 GLXDrawable win;
 GLXContext ctx;
#endif 

// FBOs
GLuint fb, fb1;
GLuint depth_rb[2];

// states
int fmode = 0;

int mc = 0;
int counter = 0;


GLenum glerror;

// uniforms 


//-------------------------------------------------------------------
// shader data
//-------------------------------------------------------------------

// viewing uniforms
Camera camera;
ShPoint3f lightPos;
ShMatrix4x4f mm; // model space matrix (for particles)
ShMatrix4x4f mv;
ShMatrix4x4f mvd;
ShMatrix4x4f v, vd; // orientation only, no translation
ShPoint3f wCamOrig; // camera origin in world space

// programs
ShProgram plane_vsh;
ShProgram plane_fsh;

ShProgram vsh;
ShProgram streaming0_fsh;
ShProgram streaming1_fsh;
ShProgram streaming2_fsh;
ShProgram streaming3_fsh;
ShProgram streaming4_fsh;
ShProgram collision0_fsh;
ShProgram collision1_fsh;
ShProgram collision2_fsh;
ShProgram collision3_fsh;
ShProgram collision4_fsh;
ShProgram dv_fsh;
//test
ShProgram showvel_fsh;
ShProgram redonly_fsh;
ShProgram greenonly_fsh;
ShProgram blueonly_fsh;
ShProgram redgreen_fsh;
ShProgram mouse0_fsh;
ShProgram mouse1_fsh;
ShProgram mouse4_fsh;
//particles
ShProgram advect_fsh;
ShProgram particle_vsh;
ShProgram particle_fsh;



ShProgramSet* plane_shaders;
ShProgramSet* streaming0_shaders;
ShProgramSet* streaming1_shaders;
ShProgramSet* streaming2_shaders;
ShProgramSet* streaming3_shaders;
ShProgramSet* streaming4_shaders;
ShProgramSet* collision0_shaders;
ShProgramSet* collision1_shaders;
ShProgramSet* collision2_shaders;
ShProgramSet* collision3_shaders;
ShProgramSet* collision4_shaders;
ShProgramSet* dv_shaders;
//test
ShProgramSet* showvel_shaders;
ShProgramSet* redonly_shaders;
ShProgramSet* greenonly_shaders;
ShProgramSet* blueonly_shaders;
ShProgramSet* redgreen_shaders;
ShProgramSet* mouse0_shaders;
ShProgramSet* mouse1_shaders;
ShProgramSet* mouse4_shaders;
// particles
ShProgramSet* advect_shaders;
ShProgramSet* particle_shaders;


float* particle_positions;
float* particle_texcoords;


//test vars
ShAttrib3f dirs; 


//camera
float modelview[16];
float projection[16];



//test
ShImage test_image;
ShArray1D<ShColor4f> testt;

//distribution package texture ids
char tid[5];
GLuint dpt_ids[5];
GLuint dptc_ids[5];
GLuint eqdpt_ids[5];
GLuint dvt_id;
GLuint bt_id;
GLuint post_ids[2];

GLuint vbuffer_id;

int dbc = 0, idbc = 1; // double buffering counter

/*ShArrayRect<ShColor4f> dpt0; // distribution packages
ShArrayRect<ShColor4f> dpt1;
ShArrayRect<ShColor4f> dpt2;
ShArrayRect<ShColor4f> dpt3;
ShArrayRect<ShColor4f> dpt4;
ShArrayRect<ShColor4f> dptc0; // distribution packages after collision step
ShArrayRect<ShColor4f> dptc1;
ShArrayRect<ShColor4f> dptc2;
ShArrayRect<ShColor4f> dptc3;
ShArrayRect<ShColor4f> dptc4;
ShArrayRect<ShColor4f> dvt; // density and velocity
ShArrayRect<ShColor4f> bt;
ShArrayRect<ShColor4f> post;*/ 

ShArray2D<ShColor4f> dpt0; // distribution packages
ShArray2D<ShColor4f> dpt1;
ShArray2D<ShColor4f> dpt2;
ShArray2D<ShColor4f> dpt3;
ShArray2D<ShColor4f> dpt4;
ShArray2D<ShColor4f> dptc0; // distribution packages after collision step
ShArray2D<ShColor4f> dptc1;
ShArray2D<ShColor4f> dptc2;
ShArray2D<ShColor4f> dptc3;
ShArray2D<ShColor4f> dptc4;
ShTexture2D<ShColor4f> dvt; // density and velocity
ShArray2D<ShColor4f> bt;
ShArray2D<ShColor4f> post; 

//ShArray2D<ShColor4f> colort;
ShArray2D<ShColor3fub> gausst(32,32);
ShImage gauss_image;

Texture3D dptc3d0(&dptc0, SQR_GRID_3D_RES);
Texture3D dptc3d1(&dptc1, SQR_GRID_3D_RES);
Texture3D dptc3d2(&dptc2, SQR_GRID_3D_RES);
Texture3D dptc3d3(&dptc3, SQR_GRID_3D_RES);
Texture3D dptc3d4(&dptc4, SQR_GRID_3D_RES);

Texture3D dvt3d(&dvt, SQR_GRID_3D_RES);


//Texture3D colort3d(&colort, SQR_GRID_3D_RES);

int dir = 0;

int rposbuff = 0, wposbuff = 1;

/*
// channels holding the particle data
ShChannel<ShPoint3f> posA;
ShChannel<ShPoint3f> posB;

// programs
ShProgram particle;  
ShProgram particle_updateA;
ShProgram particle_updateB;
*/

//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y, old_x, old_y;

bool show_help = false;


void init_FBO(void){
printf("init_FBO()\n");

GLuint testid;

glGenFramebuffersEXT(1, &fb);
glGenRenderbuffersEXT(2, depth_rb);

glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


// initialize depth renderbuffer
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb[0]);
glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                                 GL_DEPTH_COMPONENT24, GRID_2D_RES, GRID_2D_RES);
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb[1]);
glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                                 GL_DEPTH_COMPONENT24, SQR_NUM_PARTICLES, SQR_NUM_PARTICLES);
glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_rb[0]);


/*GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
       printf("FB IS INCOMPLETE!\n");*/
    

glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void init_vertexbuffers(){
    // create the buffer object
    glGenBuffersARB(1, &vbuffer_id);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, vbuffer_id);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_EXT, NUM_PARTICLES*4*NUM_POS_BYTES, 0, GL_STREAM_COPY); // undefined data
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);
    printf("finished initializing VBO\n");
    glerror = glGetError();
    if (glerror != GL_NO_ERROR) printf("Error in glBindBufferARB\n");
 
}

void load_textures(void){

    gauss_image.loadPng("densitysprite2.png");
    gausst.memory(gauss_image.memory());

 
   }

void init_textures(){
printf("init_textures()\n");


 float** testdata = new float*[5];	  
  for(int i=0;i<5;i++){ 
    
    testdata[i] = new float[NUM_GRID_CELLS*4];
    memset(testdata[i], 0, NUM_GRID_CELLS*4*sizeof(float));
  }//for

  glGenTextures(5, dpt_ids);

  for (int i=0;i<NUM_GRID_CELLS;i++){
    testdata[4][i*4+2] = ZEROVEL;
  }

  for(int i=0;i<5;i++){
		glBindTexture(DPT_TAR, dpt_ids[i]);
		glTexParameteri(DPT_TAR,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(DPT_TAR,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(DPT_TAR, 0, DPT_INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, testdata[i]);  
  }//for
  sprintf(tid, "%d",dpt_ids[0]);
  dpt0.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[1]);
  dpt1.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[2]);
  dpt2.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[3]);
  dpt3.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[4]);
  dpt4.meta("opengl:texid", tid);

  glGenTextures(5, dptc_ids);
  for(int i=0;i<5;i++){
		glBindTexture(DPT_TAR, dptc_ids[i]);
		glTexParameteri(DPT_TAR,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(DPT_TAR,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(DPT_TAR, 0, DPT_INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  
  }//for
  sprintf(tid, "%d",dptc_ids[0]);
  dptc0.meta("opengl:texid", tid);
  sprintf(tid, "%d",dptc_ids[1]);
  dptc1.meta("opengl:texid", tid);
  sprintf(tid, "%d",dptc_ids[2]);
  dptc2.meta("opengl:texid", tid);
  sprintf(tid, "%d",dptc_ids[3]);
  dptc3.meta("opengl:texid", tid);
  sprintf(tid, "%d",dptc_ids[4]);
  dptc4.meta("opengl:texid", tid);


// init density-velocity texture

  glGenTextures(1, &dvt_id);
  glBindTexture(DVT_TAR, dvt_id);
  glTexParameteri(DVT_TAR, GL_TEXTURE_MAG_FILTER,GL_LINEAR/*NEAREST*/);
  glTexParameteri(DVT_TAR, GL_TEXTURE_MIN_FILTER,GL_LINEAR/*NEAREST*/);
  glTexImage2D(DVT_TAR, 0, DVT_INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  

  sprintf(tid, "%d",dvt_id);
  dvt.meta("opengl:texid", tid);
 

// init boundary texture
 
  glGenTextures(1, &bt_id);
  glBindTexture(BT_TAR, bt_id);
  glTexParameteri(BT_TAR,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(BT_TAR,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(BT_TAR, 0, BT_INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  

  sprintf(tid, "%d",bt_id);
  bt.meta("opengl:texid", tid);

 glBindTexture(GL_TEXTURE_2D, 0);

// init positions textures
  
  float* posdata = new float[NUM_PARTICLES*4];
    memset(posdata, 0, NUM_PARTICLES*4*sizeof(float));
    for(int i=0;i<NUM_PARTICLES;i++){
	  
      posdata[4*i+0] = ((float)((i%4096)%64)) /64;
      posdata[4*i+1] = ((float)((i%4096)/64)) /64;
      posdata[4*i+2] = ((float)( i/4096)) /64;
      posdata[4*i+3] = 1.0;

     /* posdata[4*i+0] = (float)((i%(GRID_3D_RES*GRID_3D_RES))%GRID_3D_RES)/GRID_3D_RES;
      posdata[4*i+1] = (float)((i%(GRID_3D_RES*GRID_3D_RES))/GRID_3D_RES)/GRID_3D_RES;
      posdata[4*i+2] = (float)( i/(GRID_3D_RES*GRID_3D_RES))/GRID_3D_RES;
      posdata[4*i+3] = 1.0;*/
      
    /*posdata[4*i+0] = (float)(i%SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES;
    posdata[4*i+1] = (float)(i/SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES;
    posdata[4*i+2] = 0;
      posdata[4*i+3] = 1.0;*/
    }
 
  glGenTextures(2, post_ids);
 for(int i=0;i<2;i++){
  glBindTexture(POST_TAR, post_ids[i]);
  glTexParameteri(POST_TAR,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(POST_TAR,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(POST_TAR, 0, POST_INTERNAL_FORMAT, SQR_NUM_PARTICLES, SQR_NUM_PARTICLES, 0, GL_RGBA, GL_FLOAT, posdata);  
 }

 //glBindTexture(GL_TEXTURE_RECTANGLE_NV, 0);

  /*sprintf(tid, "%d",post_ids[0]);
  post.meta("opengl:texid", tid);
  sprintf(tid, "%d",post_ids[1]);
  postB.meta("opengl:texid", tid);*/

  glerror = glGetError();
  if (glerror != GL_NO_ERROR) printf("Error in init_textures\n");

  
//  glBindTexture(GL_TEXTURE_2D, 0);
}


void setup_texouts(){

  dpt0.size(GRID_2D_RES, GRID_2D_RES);
  dpt1.size(GRID_2D_RES, GRID_2D_RES);
  dpt2.size(GRID_2D_RES, GRID_2D_RES);
  dpt3.size(GRID_2D_RES, GRID_2D_RES);
  dpt4.size(GRID_2D_RES, GRID_2D_RES);
  dptc0.size(GRID_2D_RES, GRID_2D_RES);
  dptc1.size(GRID_2D_RES, GRID_2D_RES);
  dptc2.size(GRID_2D_RES, GRID_2D_RES);
  dptc3.size(GRID_2D_RES, GRID_2D_RES);
  dptc4.size(GRID_2D_RES, GRID_2D_RES);
  dvt.size(GRID_2D_RES, GRID_2D_RES);
  bt.size(GRID_2D_RES, GRID_2D_RES);
  post.size(SQR_NUM_PARTICLES, SQR_NUM_PARTICLES);
}

void setup_view()
  {
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());

  v = transpose(mv);
  v[3](0,1,2) = ShVector3f(0.0, 0.0, 0.0);
  v = transpose(v);

  vd = mvd | inverse(v);
  

 // wCamOrig = (inverse(mv) | ShPoint4f(.0,.0,.0, 1.0))(0,1,2);
 ShMatrix4x4f imv = inverse(mv);
  wCamOrig = (imv | ShPoint3f(.0,.0,.0));

  }

ShAttrib1f
fresnel (
   ShVector3f v, 
   ShNormal3f n, 
   ShAttrib1f eta
) {
   //v = normalize(v);   
   //n = normalize(n);   
   ShAttrib1f c = pos(n|v);
   ShAttrib1f s = (eta - 1.0f)/(eta + 1.0f);
   s = s*s;
   // return c*1.0f + (1.0f - c)*0.1f;
   return s + (1.0f - s)*pow((1.0f - c),5);
}


void drawQuad(){
//  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0, -1.0, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( 1.0, -1.0, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( 1.0, 1.0,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-1.0, 1.0,  0);
  
  glEnd();

}

void drawQuad(float qx, float qy, float qw, float qh){
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(qx, qy, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( qx+qw, qy, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( qx+qw, qy+qh,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(qx, qy+qh,  0);
  
  glEnd();

}

void initGL(){
		
}

void display()
  {
//printf("display\n");
  
  glDisable(GL_DEPTH_TEST); // don't need depth test in stream programs
  glViewport(0,0,GRID_2D_RES,GRID_2D_RES);

/////////////////////////////////////////////////////////////////////////////////////////////////////////	  
// Distribution packages update steps here:

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();


///////////////////  
// Draw boundaries:
 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  BT_TAR, bt_id, 0);
 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 shBind(*redonly_shaders);
   glBegin(GL_LINES);

  for(int i=0;i<SQR_GRID_3D_RES;i++){
    glVertex3f( ((float)i)/SQR_GRID_3D_RES*2-1.0, -1.0, 0);
    glVertex3f( ((float)i)/SQR_GRID_3D_RES*2-1.0, 1.0, 0);
   }
  
  glEnd();
  shUnbind(*redonly_shaders);

  shBind(*greenonly_shaders);
   glBegin(GL_LINES);

  for(int i=0;i<SQR_GRID_3D_RES;i++){
    glVertex3f( -1.0, ((float)i)/SQR_GRID_3D_RES*2-1.0, 0);
    glVertex3f(  1.0, ((float)i)/SQR_GRID_3D_RES*2-1.0, 0);
   }
  
  glEnd();
  shUnbind(*greenonly_shaders);

 shBind(*blueonly_shaders);
  drawQuad(-1.0, 1.0-(2.0/SQR_GRID_3D_RES), 2.0/SQR_GRID_3D_RES, 2.0/SQR_GRID_3D_RES);
 shUnbind(*blueonly_shaders);

/*  shBind(*redgreen_shaders);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(0.05, -1.0, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( 0.1, -1.0, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( 0.1, 1.0,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(0.05, 1.0,  0);
  
  glEnd();
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-0.95, -1.0, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( -0.9, -1.0, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( -0.9, 1.0,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-0.95, 1.0,  0);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0, -0.9, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( 1.0, -0.9, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( 1.0, -0.85,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-1.0, -0.85,  0);
  
  glEnd();
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-1.0, 0.9, 0);
  
  glTexCoord2f(1,0);
  glVertex3f( 1.0, 0.9, 0);
  
  glTexCoord2f(1,1);
  glVertex3f( 1.0, 0.85,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-1.0, 0.85,  0);
  
  glEnd();
  shUnbind(*redgreen_shaders);*/

  
/////////////////////////////////
// Calculate density and velocity:

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DVT_TAR, dvt_id, 0);

  shBind(*dv_shaders);
   drawQuad();
  shUnbind(*dv_shaders);

/////////////////////
// Perform collision:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dptc_ids[0], 0);
  
  shBind(*collision0_shaders);
   drawQuad();
  shUnbind(*collision0_shaders);
  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dptc_ids[1], 0);
  
  shBind(*collision1_shaders);
   drawQuad();
  shUnbind(*collision1_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dptc_ids[2], 0);
  
  shBind(*collision2_shaders);
   drawQuad();
  shUnbind(*collision2_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dptc_ids[3], 0);
  
  shBind(*collision3_shaders);
   drawQuad();
  shUnbind(*collision3_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dptc_ids[4], 0);
  
  shBind(*collision4_shaders);
   drawQuad();
  shUnbind(*collision4_shaders);

/////////////////////
// Perform streaming:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dpt_ids[0], 0);

 
  shBind(*streaming0_shaders);
   drawQuad();
  shUnbind(*streaming0_shaders);

  dirs(0) = 0;	 
  dirs(1) = 0;	 
  dirs(2) = 0.0003;
 
 
/*  shBind(*mouse0_shaders);
  drawQuad(0.1, 0.5, SO1, SO2);
  shUnbind(*mouse0_shaders);*/
 

 
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dpt_ids[1], 0);
  
  shBind(*streaming1_shaders);
   drawQuad();
  shUnbind(*streaming1_shaders);

 /* shBind(*mouse1_shaders);
  drawQuad(0.1, 0.5, SO1, SO2);
   shUnbind(*mouse1_shaders);*/

 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dpt_ids[2], 0);

 
  shBind(*streaming2_shaders);
   drawQuad();
  shUnbind(*streaming2_shaders);

 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dpt_ids[3], 0);

 
  shBind(*streaming3_shaders);
   drawQuad();
  shUnbind(*streaming3_shaders);

 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  DPT_TAR, dpt_ids[4], 0);

 
  shBind(*streaming4_shaders);
   drawQuad();
  shUnbind(*streaming4_shaders);

//  if(buttons[0] == GLUT_DOWN){ 
  shBind(*mouse4_shaders);
   drawQuad(0.1, 0.5, SO1, SO2);
  shUnbind(*mouse4_shaders);
//  }//if
  

  
/////////////////////////////////////
// Update the positions of particles:
//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, // bind particle depth buffer
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_rb[1]);

  glViewport(0,0,SQR_NUM_PARTICLES, SQR_NUM_PARTICLES);

  rposbuff++;
  rposbuff %= 2;
  wposbuff = (rposbuff+1)%2;
  sprintf(tid, "%d",post_ids[rposbuff]);
  post.meta("opengl:texid", tid);
 	  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  POST_TAR, post_ids[wposbuff], 0);

  shBind(*advect_shaders);
   drawQuad();
  shUnbind(*advect_shaders);

#ifndef PBO
  
    float outpos[NUM_PARTICLES*4]; 
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, SQR_NUM_PARTICLES, SQR_NUM_PARTICLES, GL_RGBA, POS_TYPE, outpos);
  
#else    
// read back into vertex buffer
    // bind buffer object to pixel pack buffer
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, vbuffer_id);
    // read from frame buffer to buffer object
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glerror = glGetError();
    if (glerror != GL_NO_ERROR) printf("Error in glReadBuffer\n");
 

    glReadPixels(0, 0, SQR_NUM_PARTICLES, SQR_NUM_PARTICLES, GL_RGBA, POS_TYPE, 0);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);
#endif    
  
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, // bind back to regular depth buffer
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_rb[0]);
  glViewport(0,0,GRID_2D_RES, GRID_2D_RES);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();


//setup_view();
  
//test

glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  shBind(*showvel_shaders);
   drawQuad();
  shUnbind(*showvel_shaders);


//printf("about to render particles\n");
 // glViewport(-GRID_2D_RES,-GRID_2D_RES, GRID_2D_RES, GRID_2D_RES);

shBind(*plane_shaders);

  // draw volume slabs
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glBegin(GL_QUADS);
  for(int i=0; i<GRID_3D_RES; i++){
   float pz = (((float)i)/GRID_3D_RES)*2-1.0;
   glTexCoord2f(0,0);
   glVertex3f(-1.0, -1.0, pz);
   glTexCoord2f(1,0);
   glVertex3f( 1.0, -1.0, pz);
   glTexCoord2f(1,1);
   glVertex3f( 1.0, 1.0,  pz);
   glTexCoord2f(0,1);
   glVertex3f(-1.0, 1.0,  pz);
  }
  glEnd();
  glDisable(GL_BLEND);
  //glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);

 
  shUnbind(*plane_shaders);

///////////////////////
// Draw particles pass:

  glPointSize(1.0);
 
#ifdef PBO
    // bind buffer object to vertex array 
  glBindBufferARB(GL_ARRAY_BUFFER, vbuffer_id);
  glVertexAttribPointerARB(0, 4, POS_TYPE, GL_FALSE, 0, 0);
  glBindBufferARB(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArrayARB(0);
  shBind(*particle_shaders);
   glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
  shUnbind(*particle_shaders);
  glDisableVertexAttribArrayARB(0);
 
#else

   
 shBind(*particle_shaders);
  glBegin(GL_POINTS);
  for(int i = 0; i < NUM_PARTICLES; i++){
	  //printf("%d: %f, %f, %f\n", i, outpos[4*i], outpos[4*i+1], outpos[4*i+2]);
 
     glVertex3f(outpos[4*i], outpos[4*i+1], outpos[4*i+2]);
  }//for
  glEnd();
  shUnbind(*particle_shaders);


    
#endif //PBO


glutSwapBuffers();
  
  }//display

void reshape(int width, int height)
  {
  glViewport(0, 0, width, height);
  setup_view();
  }

void motion(int x, int y)
  {
  const double factor = 20.0;
  bool changed = false;
  
  if (buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN) {
    camera.orbit(cur_x, cur_y, x, y,
                 glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT));
    changed = true;
  }
  if (buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN) {
    camera.move(0, 0, (y - cur_y)/factor);
    changed = true;
  }
  if (buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
    camera.move((x - cur_x)/factor, (cur_y - y)/factor, 0);
    changed = true;
  }

  if (changed) {
    setup_view();
    glutPostRedisplay();
  }

  cur_x = x;
  cur_y = y;

  /*ddx = old_x - cur_x;
  ddy = old_y - cur_y;
  
  old_x = cur_x;
  old_y = cur_y;
*/
  }

void mouse(int button, int state, int x, int y)
  {
  buttons[button] = state;
  cur_x = x;
  cur_y = y;

  old_x = cur_x;
  old_y = cur_y;

    }

void idle(void)
  {
//  update_streams();
  glutPostRedisplay();
  }

void keyboard(unsigned char k, int x, int y)
  {
  switch(k)
    {
    case 'q':
    case 'Q':
      exit(0);
      break;
    case ' ':
      {
      static bool idling = false;
      if (idling) glutIdleFunc(NULL);
      else glutIdleFunc(idle);
      idling = !idling;
      break;
      }
    case 'r':
    case 'R':
      {

      break;
      }
    case 's':
    case 'S':
      {

      break;
      }
    case 'h':
    case 'H':
      {
      show_help = !show_help;
      break;
      }
    case 'f': // fountain modes
    case 'F':
      {
      fmode++;
      fmode %= NUMMODES;
      break;
      }
    }
  glutPostRedisplay();
  }

int main(int argc, char** argv)
  {

  if(GRID_3D_RES*GRID_3D_RES*GRID_3D_RES != GRID_2D_RES*GRID_2D_RES){
	  printf("resolutions don't match!\n");
	  return 1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Lattice-Boltzmann Example");
  glewInit();
  initGL();
  init_textures();
  init_FBO(); 
  init_vertexbuffers();
 

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glPointSize(5);


  try
    {
    std::string backend_name("arb");
    if (argc > 1) {
      backend_name = argv[1];
    }//if
    shSetBackend(backend_name);

    init_shaders();
    //init_streams();
 
  
    // Place the camera at its initial position
    camera.move(0.0, 0.0, -15.0);
    camera.rotate(-45, 0.0, 1.0, 0.0);
    camera.rotate(30, 1.0, 0.0, 0.0);
    setup_view();

    // Set up the light position
    lightPos = ShPoint3f(5.0, 2.0, 5.0);    

#if 0
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  particle_fsh.node()->code()->print(cout);
  cout << "--" << endl;
  plane_fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif

  
 setup_texouts();
 load_textures();
 

  glutMainLoop();
      }
  catch (const ShException& e)
    {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return 1;
    }
  catch (...)
    {
    std::cerr << "Unknown exception caught." << std::endl;
    return 1;
    }

  return 0;
  }


void init_shaders(void)
  {
   
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord2f tc;
            
  } SH_END;

  dv_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib4f densityvec;
    densityvec = dpt0(tc) + dpt1(tc) + dpt2(tc) + dpt3(tc) + dpt4(tc);  // last element of dpt4 is unused and should not be added
    ShAttrib1f density = densityvec(0) + densityvec(1) + densityvec(2) + densityvec(3);
    
    ShAttrib3f velocity(0,0,0);
    velocity += ShAttrib3f( 1, 0, 0)*dpt0(tc)(0);
    velocity += ShAttrib3f(-1, 0, 0)*dpt0(tc)(1);
    velocity += ShAttrib3f( 0, 1, 0)*dpt0(tc)(2);
    velocity += ShAttrib3f( 0,-1, 0)*dpt0(tc)(3);

    velocity += ShAttrib3f( 1, 1, 0)*dpt1(tc)(0);
    velocity += ShAttrib3f(-1,-1, 0)*dpt1(tc)(1);
    velocity += ShAttrib3f( 1,-1, 0)*dpt1(tc)(2);
    velocity += ShAttrib3f(-1, 1, 0)*dpt1(tc)(3);

    velocity += ShAttrib3f( 1, 0, 1)*dpt2(tc)(0);
    velocity += ShAttrib3f(-1, 0,-1)*dpt2(tc)(1);
    velocity += ShAttrib3f( 1, 0,-1)*dpt2(tc)(2);
    velocity += ShAttrib3f(-1, 0, 1)*dpt2(tc)(3);
  
    velocity += ShAttrib3f( 0, 1, 1)*dpt3(tc)(0);
    velocity += ShAttrib3f( 0,-1,-1)*dpt3(tc)(1);
    velocity += ShAttrib3f( 0, 1,-1)*dpt3(tc)(2);
    velocity += ShAttrib3f( 0,-1, 1)*dpt3(tc)(3);

    velocity += ShAttrib3f( 0, 0, 1)*dpt4(tc)(0);
    velocity += ShAttrib3f( 0, 0,-1)*dpt4(tc)(1);
    //velocity += ShAttrib3f( 0, 0, 0)*dpt4(tc)(2);
    
    velocity /= density;
    
    color(0,1,2) = velocity*0.5+0.5;
    color(3) = density*0.2;
    
  } SH_END;

 collision0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
   
    ShAttrib4f eu;
    ShVector3f u = ((dvt(tc))(0,1,2))*2-1.0;
    eu(0) = ShVector3f(1,0,0)  | u;
    eu(1) = ShVector3f(-1,0,0) | u;
    eu(2) = ShVector3f(0,1,0)  | u;
    eu(3) = ShVector3f(0,-1,0) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = dpt0(tc) - INV_TAU*(dpt0(tc) - (A1 + B1*eu + C1*eu2 + D1*uu)*(dvt(tc))(3)*5);
    
  } SH_END;
 
 collision1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
 
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc)(0,1,2))*2-1.0;
    eu(0) = ShVector3f(1,1,0)  | u;
    eu(1) = ShVector3f(-1,-1,0) | u;
    eu(2) = ShVector3f(1,-1,0)  | u;
    eu(3) = ShVector3f(-1,1,0) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = dpt1(tc) - INV_TAU*(dpt1(tc) - (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3)*5);
    
  } SH_END;

 collision2_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
 
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc)(0,1,2))*2-1.0;
    eu(0) = ShVector3f(1,0,1)  | u;
    eu(1) = ShVector3f(-1,0,-1) | u;
    eu(2) = ShVector3f(1,0,-1)  | u;
    eu(3) = ShVector3f(-1,0,1) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = dpt2(tc) - INV_TAU*(dpt2(tc) - (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3)*5);
    
  } SH_END;

 collision3_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
 
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc)(0,1,2))*2-1.0;
    eu(0) = ShVector3f(0,1,1)  | u;
    eu(1) = ShVector3f(0,-1,-1) | u;
    eu(2) = ShVector3f(0,1,-1)  | u;
    eu(3) = ShVector3f(0,-1,1) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = dpt3(tc) - INV_TAU*(dpt3(tc) - (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3)*5);
    
  } SH_END;

 collision4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
  
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc)(0,1,2))*2-1.0;
    eu(0) = ShVector3f(0,0,1)  | u;
    eu(1) = ShVector3f(0,0,-1) | u;
    eu(2) = 0;
    eu(3) = 0;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
    
    ShAttrib4f eq;
    eq(0,1) = (A1 + B1*eu(0,1) + C1*eu2(0,1) + D1*uu)*dvt(tc)(3)*5;
    eq(2) = (A0 + D0*uu)*dvt(tc)(3)*5;
    
    color = dpt4(tc) - INV_TAU*(dpt4(tc) - eq);
        
  } SH_END;

 
  
  streaming0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //color = ShConstColor4f(0,0,0,0);

    ShAttrib1f bdx = (bt(tc))(0);
    ShAttrib1f bdy = (bt(tc))(1);
    ShAttrib1f bdz = (bt(tc))(2);
     
    color(0) = cond( bdx , (dptc3d0.find13(tc))(1) , (dptc3d0.find12(tc))(0) );
    color(1) = cond( bdx , (dptc3d0.find12(tc))(0) , (dptc3d0.find13(tc))(1) );
    color(2) = cond( bdy , (dptc3d0.find10(tc))(3) , (dptc3d0.find15(tc))(2) ); 
    color(3) = cond( bdy , (dptc3d0.find15(tc))(2) , (dptc3d0.find10(tc))(3) ); 
    
  } SH_END;

  streaming1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib1f bdx = (bt(tc))(0);
    ShAttrib1f bdy = (bt(tc))(1);
    ShAttrib1f bdz = (bt(tc))(2);

    color(0) = cond(bdx, (dptc3d1.find16(tc))(3),  (dptc3d1.find14(tc))(0) );
    color(0) = cond(bdy, (dptc3d1.find9(tc))(2),  color(0) );
    
    color(1) = cond(bdx, (dptc3d1.find9(tc))(2) , (dptc3d1.find11(tc))(1) );
    color(1) = cond(bdy, (dptc3d1.find16(tc))(3) , color(1) );
 
    color(2) = cond(bdx, (dptc3d1.find11(tc))(1) , (dptc3d1.find9(tc))(2));
    color(2) = cond(bdy, (dptc3d1.find14(tc))(0) , color(2));

    color(3) = cond(bdx, (dptc3d1.find14(tc))(0) , (dptc3d1.find16(tc))(3));
    color(3) = cond(bdy, (dptc3d1.find11(tc))(1) , color(3));

        
  } SH_END;

  streaming2_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib1f bdx = (bt(tc))(0);
    ShAttrib1f bdy = (bt(tc))(1);
    ShAttrib1f bdz = (bt(tc))(2);

    color(0) = cond(bdx, (dptc3d2.find5(tc))(3),  (dptc3d2.find3(tc))(0) );
    color(0) = cond(bdz, (dptc3d2.find20(tc))(2),  color(0) );
    
    color(1) = cond(bdx, (dptc3d2.find20(tc))(2) , (dptc3d2.find22(tc))(1) );
    color(1) = cond(bdz, (dptc3d2.find5(tc))(3) , color(1) );
 
    color(2) = cond(bdx, (dptc3d2.find22(tc))(1) , (dptc3d2.find20(tc))(2));
    color(2) = cond(bdz, (dptc3d2.find5(tc))(0) , color(2));

    color(3) = cond(bdx, (dptc3d2.find5(tc))(0) , (dptc3d2.find5(tc))(3));
    color(3) = cond(bdz, (dptc3d2.find22(tc))(1) , color(3));

 /*   color(0) = (dptc3d2.find3(tc))(0);
    color(1) = (dptc3d2.find22(tc))(1);
    color(2) = (dptc3d2.find20(tc))(2);
    color(3) = (dptc3d2.find5(tc))(3);*/
  } SH_END;

  streaming3_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib1f bdx = (bt(tc))(0);
    ShAttrib1f bdy = (bt(tc))(1);
    ShAttrib1f bdz = (bt(tc))(2);

    color(0) = cond(bdy, (dptc3d3.find1(tc))(3),  (dptc3d3.find7(tc))(0));
    color(0) = cond(bdz, (dptc3d3.find24(tc))(2),  color(0) );
    
    color(1) = cond(bdy, (dptc3d3.find24(tc))(2) , (dptc3d3.find18(tc))(1) );
    color(1) = cond(bdz, (dptc3d3.find1(tc))(3) , color(1) );
 
    color(2) = cond(bdy, (dptc3d3.find18(tc))(1) , (dptc3d3.find24(tc))(2));
    color(2) = cond(bdz, (dptc3d3.find7(tc))(0) , color(2));

    color(3) = cond(bdy, (dptc3d3.find7(tc))(0) , (dptc3d3.find1(tc))(3));
    color(3) = cond(bdz, (dptc3d3.find18(tc))(1) , color(3));


/*    color(0) = (dptc3d3.find7(tc))(0);
    color(1) = (dptc3d3.find18(tc))(1);
    color(2) = (dptc3d3.find24(tc))(2);
    color(3) = (dptc3d3.find1(tc))(3);*/
  } SH_END;

  streaming4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib1f bdz = (bt(tc))(2);

    color(0) = cond( bdz , (dptc3d4.find21(tc))(1) , (dptc3d4.find4(tc))(0) );
    color(1) = cond( bdz , (dptc3d4.find4(tc))(0) , (dptc3d4.find21(tc))(1) );
 
    /*color(0) = (dptc3d4.find21(tc))(0);
    color(1) = (dptc3d4.find4(tc))(1);*/
    color(2) = dptc4(tc)(2);
    color(3) = 0;

    
  } SH_END;

  showvel_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //color(0,2) = abs((dvt(tc)(0,1))*2-1.0);
      ShVector3f vel = (dvt(tc)(0,1,2))*2-1.0;
      ShAttrib1f dp = vel|vel;
      //color(0,1,2) = ShAttrib3f(dp,dp,dp);
      //color(2) = dvt(tc)(3)*5;//ShAttrib3f(dp,dp,dp);
     color(0,1,2) = abs(dvt(tc)(0,1,2)*2-1.0)*1.6;
/*	color(1) += clamp(bt(tc)(0) + bt(tc)(1), 0, 1.0);*/
	/*color(0) = bt(tc)(0);
	color(1) = bt(tc)(1);*/
        //color(0,1,2) = bt(tc)(0,1,2);
	color(3) = 1.0;
    //color(2) = clamp(abs(-dvt(tc)(0)) + abs(-dvt(tc)(1)),0,1.0);
    
  } SH_END;
   
  redonly_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = ShConstColor4f(1,0,0,1);
        } SH_END;

   greenonly_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = ShConstColor4f(0,1,0,1);
       
  } SH_END;
 
    blueonly_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = ShConstColor4f(0,0,1,1);
       
  } SH_END;
  redgreen_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = ShConstColor4f(1,1,0,1);
       
   } SH_END;

   mouse0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    discard(gausst(tc)(0) - 0.2);
    ShAttrib1f ampl = 1.0 - gausst(tc)(0);
    // ShAttrib1f ampl = 1.0 - 2*abs(tc(0)-0.5);
    color(0) = (dirs|ShAttrib3f(1,0,0));
    color(1) = (dirs|ShAttrib3f(-1,0,0));
    color(2) = (dirs|ShAttrib3f(0,1,0));
    color(3) = (dirs|ShAttrib3f(0,-1,0));
   } SH_END;

   mouse1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
   
   discard(gausst(tc)(0) - 0.2);
    
    color(0) = RCPSQR2*(dirs|ShAttrib3f(1,1,0));
    color(1) = RCPSQR2*(dirs|ShAttrib3f(-1,-1,0));
    color(2) = RCPSQR2*(dirs|ShAttrib3f(1,-1,0));
    color(3) = RCPSQR2*(dirs|ShAttrib3f(-1,1,0));
   } SH_END;

   mouse4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;
   
   discard(gausst(tc)(0) - 0.2);
    
    color(0) = (dirs|ShAttrib3f(0,0,1));
    color(1) = (dirs|ShAttrib3f(0,0,-1));
    color(2) = 0;
    color(3) = 0;
   } SH_END;
/////////////////
//particle shaders
   
   advect_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib4f ppos = post(tc);
    
    color(0,1,2) = ppos(0,1,2) + VELSTEP*((dvt3d( ppos(0,1,2) )(0,1,2))*2-1.0);
    //color(2) = 0;
    color(3) = 1.0;
   } SH_END;

   particle_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    //ShInOutTexCoord2f tc;
    
    pos(0,1,2) = (pos(0,1,2)*2-1.0);
    pos(3) = 1.0;
    pos = mvd|pos;
                
   } SH_END;

   particle_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    //ShInputTexCoord2f tc;
    ShOutputColor4f color;

    /*color(0,1,2) = dvt(tc)(0,1,2);
    color(3) = 1.0;*/

    color = ShConstColor4f(1,1,1,1);
   } SH_END;
 
   plane_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord2f tc;
    ShOutputPosition3f posp;
   
    posp = pos(0,1,2);
    pos = mvd | pos;
                   
   } SH_END;

   plane_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
 
   ShInputPosition4f pos;
   ShInputTexCoord2f tc;
   ShInputPosition4f posp;
   ShOutputColor4f color;

   color(0,1,2) = abs(dvt3d(posp(0,1,2)*0.5+0.5)(0,1,2)*2-1.0);
   color(3) = dvt3d(posp(0,1,2)*0.5+0.5)(3);
   //color = ShConstColor4f(1,1,1,0);

    
   } SH_END;

  dv_shaders = new ShProgramSet(vsh, dv_fsh);
  collision0_shaders = new ShProgramSet(vsh, collision0_fsh);
  collision1_shaders = new ShProgramSet(vsh, collision1_fsh);
  collision2_shaders = new ShProgramSet(vsh, collision2_fsh);
  collision3_shaders = new ShProgramSet(vsh, collision3_fsh);
  collision4_shaders = new ShProgramSet(vsh, collision4_fsh);
  streaming0_shaders = new ShProgramSet(vsh, streaming0_fsh); 
  streaming1_shaders = new ShProgramSet(vsh, streaming1_fsh);
  streaming2_shaders = new ShProgramSet(vsh, streaming2_fsh);
  streaming3_shaders = new ShProgramSet(vsh, streaming3_fsh);
  streaming4_shaders = new ShProgramSet(vsh, streaming4_fsh);
////test
  showvel_shaders = new ShProgramSet(vsh, showvel_fsh);
  redonly_shaders = new ShProgramSet(vsh, redonly_fsh);
  greenonly_shaders = new ShProgramSet(vsh, greenonly_fsh);
  blueonly_shaders = new ShProgramSet(vsh, blueonly_fsh);
  redgreen_shaders = new ShProgramSet(vsh, redgreen_fsh);
  mouse0_shaders = new ShProgramSet(vsh, mouse0_fsh);
  mouse1_shaders = new ShProgramSet(vsh, mouse1_fsh);
  mouse4_shaders = new ShProgramSet(vsh, mouse4_fsh);
///particle shaders
  advect_shaders = new ShProgramSet(vsh, advect_fsh);
  particle_shaders = new ShProgramSet(particle_vsh, particle_fsh);
  plane_shaders = new ShProgramSet(plane_vsh, plane_fsh);
        }


int gprintf(int x, int y, char* fmt, ...)
  {
  char temp[1024];
  va_list va;
  va_start(va, fmt);
  vsprintf(temp, fmt, va);
  va_end(va);

  // setup the matrices for a direct
  // screen coordinate transform when
  // using glRasterPos
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vp[2], 0, vp[3], -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // just in case, turn lighting off and disable depth testing
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  // render the character through glut
  char* p = temp;
  glRasterPos2f(x, y);
  while(*p)
    {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (*p));
    p++;
    }

  // reset OpenGL to what is was
  // before we started
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  return p-temp;
  }
