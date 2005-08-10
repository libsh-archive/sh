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


#define INTERNAL_FORMAT GL_RGBA16 /*GL_RGBA8*/

#define NUMMODES 1

#define SO1 0.2
#define SO2 0.2
#define SCALE 5
#define GRID_2D_RES 512
#define SQR_GRID_3D_RES 8
#define GRID_3D_RES SQR_GRID_3D_RES*SQR_GRID_3D_RES
#define NUM_GRID_CELLS GRID_3D_RES*GRID_3D_RES*GRID_3D_RES

#define RCPSQR2 0.707106781186

#define S2 3.0

#define A0 (4.0/9)
#define D0 -(S2*2.0/3)

#define A1 (1.0/9)
#define B1 (S2*1.0/3)
#define C1 (S2*S2*1.0/2)
#define D1 -(S2*1.0/6)

#define A2 (1.0/36)
#define B2 (S2*1.0/12)
#define C2 (S2*S2*1.0/8)
#define D2 -(S2*1.0/24)

#define ZEROVEL 1.0

#define TAU 5.0
#define INV_TAU (1.0f/TAU)

#define VELSTEP 0.005

#define SQR_NUM_PARTICLES 512
#define NUM_PARTICLES (SQR_NUM_PARTICLES*SQR_NUM_PARTICLES)
#define PCO 0.01

// forward declarations
void init_shaders(void);
void init_textures(void);
void init_FBO(void);
void initGL(void);

void init_streams(void);
void reset_streams(void);
void update_streams(void);

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
GLuint fb;
GLuint depth_rb;

// states
int fmode = 0;

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
ShProgram eq0_fsh;
ShProgram eq1_fsh;
ShProgram eq2_fsh;
ShProgram eq3_fsh;
ShProgram eq4_fsh;
ShProgram collision0_fsh;
ShProgram collision1_fsh;
ShProgram collision2_fsh;
ShProgram collision3_fsh;
ShProgram collision4_fsh;
ShProgram dv_fsh;
//test
ShProgram showvel_fsh;
ShProgram showeq0_fsh;
ShProgram redonly_fsh;
ShProgram greenonly_fsh;
ShProgram mouse0_fsh;
ShProgram mouse1_fsh;
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
ShProgramSet* eq0_shaders;
ShProgramSet* eq1_shaders;
ShProgramSet* eq2_shaders;
ShProgramSet* eq3_shaders;
ShProgramSet* eq4_shaders;
ShProgramSet* collision0_shaders;
ShProgramSet* collision1_shaders;
ShProgramSet* collision2_shaders;
ShProgramSet* collision3_shaders;
ShProgramSet* collision4_shaders;
ShProgramSet* dv_shaders;
//test
ShProgramSet* showvel_shaders;
ShProgramSet* showeq0_shaders;
ShProgramSet* redonly_shaders;
ShProgramSet* greenonly_shaders;
ShProgramSet* mouse0_shaders;
ShProgramSet* mouse1_shaders;
// particles
ShProgramSet* advect_shaders;
ShProgramSet* particle_shaders;

//test vars
ShAttrib3f dirs; 


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

int dbc = 0, idbc = 1; // double buffering counter



//distribution package texture nodes 
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
ShArray2D<ShColor4f> dvt; // density and velocity
ShArray2D<ShColor4f> eqdpt0; // equilibrium distribution packages
ShArray2D<ShColor4f> eqdpt1;
ShArray2D<ShColor4f> eqdpt2;
ShArray2D<ShColor4f> eqdpt3;
ShArray2D<ShColor4f> eqdpt4;
ShArray2D<ShColor4f> bt;
ShArray2D<ShColor4f> post; 

ShArray2D<ShColor4f> colort;
ShArray2D<ShColor3fub> gausst(32,32);
ShImage gauss_image;

Texture3D dptc3d0(&dptc0, SQR_GRID_3D_RES);
Texture3D dptc3d1(&dptc1, SQR_GRID_3D_RES);
Texture3D dptc3d2(&dptc2, SQR_GRID_3D_RES);
Texture3D dptc3d3(&dptc3, SQR_GRID_3D_RES);
Texture3D dptc3d4(&dptc4, SQR_GRID_3D_RES);

Texture3D colort3d(&colort, SQR_GRID_3D_RES);

int dir = 0;

int rposbuff = 0, wposbuff = 1;

// channels holding the particle data
ShChannel<ShPoint3f> posA;
ShChannel<ShPoint3f> posB;

// programs
ShProgram particle;  
ShProgram particle_updateA;
ShProgram particle_updateB;

//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y, old_x, old_y, ddx, ddy;

bool show_help = false;


void init_FBO(void){
printf("init_FBO()\n");

GLuint testid;

glGenFramebuffersEXT(1, &fb);
glGenRenderbuffersEXT(1, &depth_rb);

glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


// initialize depth renderbuffer
glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                                 GL_DEPTH_COMPONENT24, GRID_2D_RES, GRID_2D_RES);
glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_rb);


/*GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
       printf("FBO IS INCOMPLETE!\n");*/

glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void load_textures(void){

    gauss_image.loadPng("densitysprite2.png");
    gausst.memory(gauss_image.memory());

   /* hm_image.loadPng("hm2.png");
    nm_image.loadPng("nm2.png");
    em_imageA.loadPng("sky2/A.png");    
    em_imageB.loadPng("sky2/C.png"); 
    em_imageC.loadPng("sky2/E.png");
    em_imageD.loadPng("sky2/F.png");
    em_imageE.loadPng("sky2/D.png");
    em_imageF.loadPng("sky2/B.png");
    terr_image.loadPng("densitysprite2.png");*/
			
    /*
    em.memory(em_imageA.memory(), static_cast<ShCubeDirection>(0));
    em.memory(em_imageB.memory(), static_cast<ShCubeDirection>(1));
    em.memory(em_imageC.memory(), static_cast<ShCubeDirection>(2));
    em.memory(em_imageD.memory(), static_cast<ShCubeDirection>(3));
    em.memory(em_imageE.memory(), static_cast<ShCubeDirection>(4));
    em.memory(em_imageF.memory(), static_cast<ShCubeDirection>(5));
*/

    
   }

void init_textures(){
printf("init_textures()\n");

  //float* expl = new float[] 


 float** testdata = new float*[5];	  
  for(int i=0;i<5;i++){ 
    
    testdata[i] = new float[NUM_GRID_CELLS*4];
    memset(testdata[i], 0, NUM_GRID_CELLS*4*sizeof(float));
   /* for(int j=3000;j<4000;j++)
	  testdata[i][j] = 1.0;//((float)rand())/(RAND_MAX-1);*/
  /*testdata[NUM_GRID_CELLS/2] = 1.0;
  testdata[NUM_GRID_CELLS/3] = 1.0;
  testdata[NUM_GRID_CELLS/4] = 1.0;*/
  }//for

  glGenTextures(5, dpt_ids);

  for (int i=0;i<NUM_GRID_CELLS;i++){
    testdata[4][i*4+2] = ZEROVEL;
  }

  for(int i=0;i<5;i++){
		glBindTexture(GL_TEXTURE_2D, dpt_ids[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, testdata[i]);  
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
		glBindTexture(GL_TEXTURE_2D, dptc_ids[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  
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



// init eqdpt's  
  glGenTextures(5, eqdpt_ids);
  for(int i=0;i<5;i++){
		glBindTexture(GL_TEXTURE_2D, eqdpt_ids[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  
	  }//for

  sprintf(tid, "%d",eqdpt_ids[0]);
  eqdpt0.meta("opengl:texid", tid);
  sprintf(tid, "%d",eqdpt_ids[1]);
  eqdpt1.meta("opengl:texid", tid);
  sprintf(tid, "%d",eqdpt_ids[2]);
  eqdpt2.meta("opengl:texid", tid);
  sprintf(tid, "%d",eqdpt_ids[3]);
  eqdpt3.meta("opengl:texid", tid);
  sprintf(tid, "%d",eqdpt_ids[4]);
  eqdpt4.meta("opengl:texid", tid);

// init density-velocity texture

  glGenTextures(1, &dvt_id);
  glBindTexture(GL_TEXTURE_2D, dvt_id);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  

  sprintf(tid, "%d",dvt_id);
  dvt.meta("opengl:texid", tid);
 

// init boundary texture
 
  glGenTextures(1, &bt_id);
  glBindTexture(GL_TEXTURE_2D, bt_id);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  

  sprintf(tid, "%d",bt_id);
  bt.meta("opengl:texid", tid);


// init positions textures
  
  float* posdata = new float[NUM_PARTICLES*4];
    memset(posdata, 0, NUM_PARTICLES*4*sizeof(float));
    for(int i=0;i<NUM_PARTICLES;i++){
	  
      posdata[4*i+0] = (float)(i%SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES;
      posdata[4*i+1] = (float)(i/SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES;
      posdata[4*i+2] = -1.0;
      posdata[4*i+2] = 1.0;

    
    }
 
  glGenTextures(2, post_ids);
 for(int i=0;i<2;i++){
  glBindTexture(GL_TEXTURE_2D, post_ids[i]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, posdata);  
 }
  /*sprintf(tid, "%d",post_ids[0]);
  post.meta("opengl:texid", tid);
  sprintf(tid, "%d",post_ids[1]);
  postB.meta("opengl:texid", tid);*/

  
  
//  glBindTexture(GL_TEXTURE_2D, 0);
}


void setup_texouts(){

  dpt0.size(GRID_2D_RES, GRID_2D_RES);
  dpt1.size(GRID_2D_RES, GRID_2D_RES);
  dpt2.size(GRID_2D_RES, GRID_2D_RES);
  dpt3.size(GRID_2D_RES, GRID_2D_RES);
  dpt4.size(GRID_2D_RES, GRID_2D_RES);
  colort.size(GRID_2D_RES, GRID_2D_RES);

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



void initGL(){
		
}

void display()
  {
//printf("display\n");
  
  glDisable(GL_DEPTH_TEST); // don't need depth test in stream programs


/////////////////////////////////////////////////////////////////////////////////////////////////////////	  
// Distribution packages update steps here:

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();


///////////////////  
// Draw boundaries:
 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, bt_id, 0);

  shBind(*redonly_shaders);
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
  
  glEnd();
  shUnbind(*redonly_shaders);

  shBind(*greenonly_shaders);
  glBegin(GL_QUADS);
  
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
  shUnbind(*greenonly_shaders);

  
/////////////////////////////////
// Calculate density and velocity:

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dvt_id, 0);

  shBind(*dv_shaders);
   drawQuad();
  shUnbind(*dv_shaders);

///////////////////////////////  
// Calculate equilibrium state:  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, eqdpt_ids[0], 0);

  shBind(*eq0_shaders);
   drawQuad();
  shUnbind(*eq0_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, eqdpt_ids[1], 0);

  shBind(*eq1_shaders);
   drawQuad();
  shUnbind(*eq1_shaders);
 
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, eqdpt_ids[4], 0);

  shBind(*eq4_shaders);
   drawQuad();
  shUnbind(*eq4_shaders);

/////////////////////
// Perform collision:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dptc_ids[0], 0);
  
  shBind(*collision0_shaders);
   drawQuad();
  shUnbind(*collision0_shaders);
  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dptc_ids[1], 0);
  
  shBind(*collision1_shaders);
   drawQuad();
  shUnbind(*collision1_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dptc_ids[4], 0);
  
  shBind(*collision4_shaders);
   drawQuad();
  shUnbind(*collision4_shaders);

/////////////////////
// Perform streaming:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[0], 0);

 
  shBind(*streaming0_shaders);
   drawQuad();
  shUnbind(*streaming0_shaders);

 if(buttons[0] == GLUT_DOWN){ 
  dirs(0) = ((float)ddx)/3;
  dirs(1) = ((float)ddy)/3;
  dirs(2) = 0;
  shBind(*mouse0_shaders);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-0.5, 0, 0);
  
  glTexCoord2f(1,0);
  glVertex3f(-0.5+SO2, 0, 0);
  
  glTexCoord2f(1,1);
  glVertex3f(-0.5+SO2, SO1,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-0.5, SO1,  0);
  
  glEnd();
  shUnbind(*mouse0_shaders);
 }//if
  

 
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[1], 0);
  
  shBind(*streaming1_shaders);
   drawQuad();
  shUnbind(*streaming1_shaders);

 if(buttons[0] == GLUT_DOWN){ 
  shBind(*mouse1_shaders);
  glBegin(GL_QUADS);
  
  glTexCoord2f(0,0);
  glVertex3f(-0.5, 0, 0);
  
  glTexCoord2f(1,0);
  glVertex3f(-0.5+SO2, 0, 0);
  
  glTexCoord2f(1,1);
  glVertex3f(-0.5+SO2, SO1,  0);
  
  glTexCoord2f(0,1);
  glVertex3f(-0.5, SO1,  0);
  
  glEnd();
  shUnbind(*mouse1_shaders);
 }//if

 glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[4], 0);

 
  shBind(*streaming4_shaders);
   drawQuad();
  shUnbind(*streaming4_shaders);

  
/////////////////////////////////////
// Update the positions of particles:

  rposbuff++;
  rposbuff %= 2;
  wposbuff = (rposbuff+1)%2;
  sprintf(tid, "%d",post_ids[rposbuff]);
  post.meta("opengl:texid", tid);
 	  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, post_ids[wposbuff], 0);

 
  shBind(*advect_shaders);
   drawQuad();
  shUnbind(*advect_shaders);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


  
//test

glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  shBind(*showvel_shaders);
   drawQuad();
  shUnbind(*showvel_shaders);


  
  
///////////////////////
// Draw particles pass:
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	
/*  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  float* particle_positions = 0;
 if (dir == 0){
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
    if (posA_storage)
      {

        posA_storage->sync();
        particle_positions = (float*)posA_storage->data();
      }//if
    
 }else
    {
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr posB_storage = shref_dynamic_cast<ShHostStorage>(posB.memory()->findStorage("host"));
    if (posB_storage){
 
      posB_storage->sync();
      particle_positions = (float*)posB_storage->data();
    }
 }//else

  if (particle_positions) {
  shBind(*particle_shaders);
  glBegin(GL_QUADS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   // for(int k=0;k<3;k++) mm[3](k) = particle_positions[3*i+k];
 
  //glMultiTexCoord3fARB(GL_TEXTURE0, particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
  //glMultiTexCoord1fARB(GL_TEXTURE1, particle_times[i]);
  //glNormal3f(particle_velocities[3*i],particle_velocities[3*i+1],particle_velocities[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  //glNormal3f(0, 0, -1);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);

  //glNormal3f(1, 0, 0);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);

  //glNormal3f(0, 0, 1);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  //glNormal3f(-1, 0, 0);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  //glNormal3f(0, -1, 0);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
 
   }//for
  glEnd();
  shUnbind(*particle_shaders);
  }//if

*/  

  
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

  ddx = old_x - cur_x;
  ddy = old_y - cur_y;
  
  old_x = cur_x;
  old_y = cur_y;

  }

void mouse(int button, int state, int x, int y)
  {
  buttons[button] = state;
  cur_x = x;
  cur_y = y;

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

       reset_streams();

      break;
      }
    case 's':
    case 'S':
      {

       update_streams();

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
    init_streams();
 
  
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


void init_streams(void)
  {
 particle = SH_BEGIN_PROGRAM("gpu:stream") {
    ShInOutPoint3f pos;
 /*   ShInOutVector3f vel;
    ShInOutVector3f initvel;
    ShInOutAttrib1f time;
    ShInputVector3f acc;
    ShInputAttrib1f delta;*/

    pos = pos + 0.000000001;

       
 /*  ShAttrib1f mu(0.8);
    ShAttrib1f eps(0.6);

    // check if below ground level
   ShAttrib1f elev = (hm( (pos(0,2)+SCALE)/(2.0*SCALE)) )(0);
    ShAttrib1f under = pos(1) < elev;

    ShAttrib1f out = (pos(0) < -SCALE) || (pos(0) > SCALE) || (pos(2) < -SCALE) || (pos(2) > SCALE);
    time -= lifeStep*ShAttrib1f(LIFE_STEP_FACTOR);
    time = cond(out, ShAttrib1f(0.0), time);
    pos = cond(time, pos, ShPoint3f(0.0,1.0,0.0));
    vel = cond(time, vel, initvel);
    time = cond(time, time, ShAttrib1f(1.0));
    
    // clamp to ground level if necessary
   pos = cond(under,ShPoint3f(pos(0), elev, pos(2)), pos);


    // modify velocity in case of collision
    ShVector3f norm( (nm((pos(0,2)+SCALE)/(2.0*SCALE)))(0,1,2)  );
    norm = norm(0,2,1)*2-1.0;
    norm = normalize(norm);
    norm(0) = -norm(0); 
    ShVector3f veln = dot(vel,norm)*norm;
    ShVector3f velt = vel - veln;
    vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);

    ShAttrib1f onsurface = abs(pos(1)-elev) < 0.1;
    ShAttrib1f minspeed = length(vel)<ShAttrib1f(MIN_SPEED);
    
    // clamp acceleration to zero if particles at or below ground plane 
    acc = cond(onsurface, ShVector3f(0.0, 0.0, 0.0), acc);

    // integrate acceleration to get velocity
    vel = cond(onsurface&&minspeed, vel, vel+acc*delta);
    vel -= DAMP_FACTOR*vel*delta;

    // integrate velocity to update position
    pos = cond(onsurface&&minspeed,pos,pos + vel*delta);*/

    
 } SH_END;

  ShHostMemoryPtr host_posA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_posB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
/*  ShHostMemoryPtr host_velA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_velB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_initvelA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_initvelB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_timeA = new ShHostMemory(sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_timeB = new ShHostMemory(sizeof(float)*NUM_PARTICLES, SH_FLOAT);*/


 
  posA = ShChannel<ShPoint3f>(host_posA, NUM_PARTICLES);
  posB = ShChannel<ShPoint3f>(host_posB, NUM_PARTICLES);
 /* velA = ShChannel<ShVector3f>(host_velA, NUM_PARTICLES);
  velB = ShChannel<ShVector3f>(host_velB, NUM_PARTICLES);
  initvelA = ShChannel<ShVector3f>(host_initvelA, NUM_PARTICLES);
  initvelB = ShChannel<ShVector3f>(host_initvelB, NUM_PARTICLES);
  timeA = ShChannel<ShAttrib1f>(host_timeA, NUM_PARTICLES);
  timeB = ShChannel<ShAttrib1f>(host_timeB, NUM_PARTICLES);*/

  // Tie the postion and velocity data into an ShStream, one
  // for the 'A' stream and one for the 'B' stream.
  ShStream dataA = posA; //& velA & initvelA & timeA;
  ShStream dataB = posB; //& velB & initvelB & timeB;

  // Process the generic particle program into two versions, one that
  // uses the dataA stream and one that uses the dataB stream. Additionally,
  // we feed in two uniforms (gravity & delta) since these will not change
  // on a particle by particle basis (although they can be changed globally).

  
//  particle_updateA = shSwizzle(0, 1) << (particle << dataA << gravity << delta);
//  particle_updateB = shSwizzle(0, 1) << (particle << dataB << gravity << delta);

  particle_updateA = (particle << posA);
  particle_updateB = (particle << posB);

  // Everything has been setup for the particle system to operate, last
  // thing is to set the initial data.
  reset_streams();
  }

void reset_streams(void)
  {
  
  ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
  /*ShHostStoragePtr velA_storage = shref_dynamic_cast<ShHostStorage>(velA.memory()->findStorage("host"));
  ShHostStoragePtr initvelA_storage = shref_dynamic_cast<ShHostStorage>(initvelA.memory()->findStorage("host"));
  ShHostStoragePtr timeA_storage = shref_dynamic_cast<ShHostStorage>(timeA.memory()->findStorage("host"));*/


  posA_storage->dirty();
  /*velA_storage->dirty();
  initvelA_storage->dirty();
  timeA_storage->dirty();*/

  float* posA_data = (float*)posA_storage->data();
  /*float* velA_data = (float*)velA_storage->data();
  float* initvelA_data = (float*)initvelA_storage->data();
  float* timeA_data = (float*)timeA_storage->data();*/


  // Iterate over the particle and initialize their state
  for(int i = 0; i < NUM_PARTICLES; i++)
    {
    // All the particles will start at the same places
    posA_data[3*i+0] = (float)(i%SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES*2-1.0;
    posA_data[3*i+1] = (float)(i/SQR_NUM_PARTICLES)/SQR_NUM_PARTICLES*2-1.0;
    posA_data[3*i+2] = -1.0;

/*    // The velocity direction will vary over a small cone of angles.
    float theta = 2*M_PI*((float)rand()/(RAND_MAX-1));
    float phi = 0.7*(M_PI/2) + 0.2*(M_PI/2)*((float)rand()/(RAND_MAX-1));

    float cost = cos(theta);
    float sint = sin(theta);
    float cosp = cos(phi);
    float sinp = sin(phi);

    // Additionally, vary the speed (i.e. magnitude of the velocity vector)
    float power = 5 + 4*((float)rand()/(RAND_MAX-1));

    initvelA_data[3*i+0] = power*sint*cosp;
    initvelA_data[3*i+1] = power*sinp;
    initvelA_data[3*i+2] = power*cost*cosp;

    velA_data[3*i+0] = 0;
    velA_data[3*i+1] = 0;
    velA_data[3*i+2] = 0;*/

   
    }

  // reset the dir flag to that the simulation will start with the 'A' stream
  dir = 0;
  }

void update_streams(void)
  {
  try 
    {
    // The value of dir specifcies whether to run particle_updateA, which
    // will take the data from stream 'A' and run the particle program to
    // generate stream 'B', or the opposite using 'B' to generate 'A'. The
    // rendering code uses dir in the same way to decided which stream has
    // the latest state data for draw drawing the particle.
    if (dir == 0)
      {
      posB /*& velB & initvelB & timeB*/ = particle_updateA;
      }
    else
      {
      posA /*& velA & initvelA & timeA*/ = particle_updateB;
      }
    dir = !dir;
   }
  catch (const ShException& e)
    {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    }
  catch (...)
    {
    std::cerr << "Unknown exception caught." << std::endl;
    }
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
    color(3) = density;
    
  } SH_END;

  eq0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

  
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = ((dvt(tc))(0,1,2))*2-1.0;
    eu(0) = ShVector3f(1,0,0)  | u;
    eu(1) = ShVector3f(-1,0,0) | u;
    eu(2) = ShVector3f(0,1,0)  | u;
    eu(3) = ShVector3f(0,-1,0) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A1 + B1*eu + C1*eu2 + D1*uu)*(dvt(tc))(3);
    
  } SH_END;

 eq1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
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
     
    color = (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3);
    
  } SH_END;

 eq2_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
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
     
    color = (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3);
    
  } SH_END;

 eq3_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
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
     
    color = (A2 + B2*eu + C2*eu2 + D2*uu)*dvt(tc)(3);
    
  } SH_END;

 eq4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
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
     
    color = (A1 + B1*eu + C1*eu2 + D1*uu)*dvt(tc)(3);
    color(2) = (A0 + D0*uu)*dvt(tc)(3); 
    
  } SH_END;

 collision0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = dpt0(tc) - INV_TAU*(dpt0(tc) - eqdpt0(tc));
    
  } SH_END;
 
 collision1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = dpt1(tc) - INV_TAU*(dpt1(tc) - eqdpt1(tc));
    
  } SH_END;

 collision4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = dpt4(tc) - INV_TAU*(dpt4(tc) - eqdpt4(tc));
        
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

  streaming4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //tc(0) = tc(0)+0.01;   
    color(0) = 0;//dptc4(tc)(0);//(dptc3d4.find14(tc))(0);
    color(1) = 0;//dptc4(tc)(1);//(dptc3d4.find11(tc))(1);
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
      color(0,1,2) = ShAttrib3f(dp,dp,dp);
    //color(0,1,2) = post(tc)(0,1,2);
/*	color(1) += clamp(bt(tc)(0) + bt(tc)(1), 0, 1.0);
	color(0) += bt(tc)(0);
	color(2) += bt(tc)(1);*/
	color(3) = 1.0;
    //color(2) = clamp(abs(-dvt(tc)(0)) + abs(-dvt(tc)(1)),0,1.0);
    
  } SH_END;
  showeq0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color(0,1,2) = eqdpt0(tc)(0,1,2);
    color(3) = 1.0;
    
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

   mouse0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //color = ShConstColor4f(1,0,0,1);
    discard(gausst(tc)(0) - 0.9 );
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
   
    //ShAttrib1f ampl = 1.0 - 2*abs(tc(0)-0.5);
    discard(gausst(tc)(0) - 0.9 );
   // ShAttrib1f ampl = 1.0 - gausst(tc)(0);
    
    
    //color = ShConstColor4f(1,0,0,1);
    color(0) = RCPSQR2*(dirs|ShAttrib3f(1,1,0));
    color(1) = RCPSQR2*(dirs|ShAttrib3f(-1,-1,0));
    color(2) = RCPSQR2*(dirs|ShAttrib3f(1,-1,0));
    color(3) = RCPSQR2*(dirs|ShAttrib3f(-1,1,0));
   } SH_END;

/////////////////
//particle shaders
   
   advect_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    ShAttrib4f ppos = post(tc);
    
    color(0,1,2) = ppos(0,1,2) + VELSTEP*((dvt(ppos(0,1))(0,1,2))*2-1.0);
    color(3) = 1.0;
   } SH_END;

   particle_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    //ShInOutTexCoord2f tc;
            
   } SH_END;

   particle_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    //ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color = ShConstColor4f(1,1,1,1);
   } SH_END;
 

  dv_shaders = new ShProgramSet(vsh, dv_fsh);
  eq0_shaders = new ShProgramSet(vsh, eq0_fsh);
  eq1_shaders = new ShProgramSet(vsh, eq1_fsh);
  eq2_shaders = new ShProgramSet(vsh, eq2_fsh);
  eq3_shaders = new ShProgramSet(vsh, eq3_fsh);
  eq4_shaders = new ShProgramSet(vsh, eq4_fsh);
  collision0_shaders = new ShProgramSet(vsh, collision0_fsh);
  collision1_shaders = new ShProgramSet(vsh, collision1_fsh);
  collision4_shaders = new ShProgramSet(vsh, collision4_fsh);
  streaming0_shaders = new ShProgramSet(vsh, streaming0_fsh);
  streaming1_shaders = new ShProgramSet(vsh, streaming1_fsh);
  streaming4_shaders = new ShProgramSet(vsh, streaming4_fsh);
////test
  showvel_shaders = new ShProgramSet(vsh, showvel_fsh);
  showeq0_shaders = new ShProgramSet(vsh, showeq0_fsh);
  redonly_shaders = new ShProgramSet(vsh, redonly_fsh);
  greenonly_shaders = new ShProgramSet(vsh, greenonly_fsh);
  mouse0_shaders = new ShProgramSet(vsh, mouse0_fsh);
  mouse1_shaders = new ShProgramSet(vsh, mouse1_fsh);
//particle shaders
  advect_shaders = new ShProgramSet(vsh, advect_fsh);
  particle_shaders = new ShProgramSet(particle_vsh, particle_fsh);
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
