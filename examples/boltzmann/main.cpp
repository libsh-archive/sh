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
//#define USING_STREAMS

#define NUMMODES 1

#define SCALE 5
#define GRID_2D_RES 512
#define SQR_GRID_3D_RES 8
#define GRID_3D_RES SQR_GRID_3D_RES*SQR_GRID_3D_RES
#define NUM_GRID_CELLS GRID_3D_RES*GRID_3D_RES*GRID_3D_RES

#define TAU 20
#define INV_TAU (1.0f/TAU)

// forward declarations
void init_shaders(void);
void init_textures(void);
void init_FBO(void);
void initGL(void);
#ifdef USING_STREAMS
void init_streams(void);
void reset_streams(void);
void update_streams(void);
#endif
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


#ifdef USING_STREAMS
// channels holding the distribution packages
ShChannel<ShVector4f> dp0;
ShChannel<ShVector4f> dp1;
ShChannel<ShVector4f> dp2;
ShChannel<ShVector4f> dp3;
ShChannel<ShVector4f> dp4;
ShChannel<ShVector4f> color;

// programs
ShProgram streaming;  
ShProgram lbm_update;
#endif

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
ShVector3f gravity(0.0, -9.8, 0.0);
ShAttrib1f delta(0.02); 
ShAttrib1f simtime(0.0);
ShConstAttrib1f timedelta(0.005);



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
/*ShProgram vsh;
ShProgram particle_vsh;
ShProgram particle_fsh;
ShProgram particle_volume_vsh;
ShProgram particle_volume_fsh;
ShProgram terrain_vsh;
ShProgram terrain_fsh;
ShProgram skybox_vsh;
ShProgram skybox_fsh;*/
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


/*ShProgramSet* particle_shaders;
ShProgramSet* particle_volume_shaders;
ShProgramSet* terrain_shaders;
ShProgramSet* skybox_shaders;*/
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


#ifndef USING_STREAMS
//test
ShImage test_image;
ShArray1D<ShColor4f> testt;

//distribution package texture ids
GLuint dpt_ids[2][5];
GLuint eqdpt_ids[5];
GLuint dvt_id;

int dbc = 0, idbc = 1; // double buffering counter

#endif

//distribution package texture nodes 
ShArray2D<ShColor4f> dpt0; // distribution packages
ShArray2D<ShColor4f> dpt1;
ShArray2D<ShColor4f> dpt2;
ShArray2D<ShColor4f> dpt3;
ShArray2D<ShColor4f> dpt4;
ShArray2D<ShColor4f> dvt; // density and velocity
ShArray2D<ShColor4f> eqdpt0; // equilibrium distribution packages
ShArray2D<ShColor4f> eqdpt1;
ShArray2D<ShColor4f> eqdpt2;
ShArray2D<ShColor4f> eqdpt3;
ShArray2D<ShColor4f> eqdpt4;

ShArray2D<ShColor4f> colort;

Texture3D dpt3d0(&dpt0, SQR_GRID_3D_RES);
Texture3D dpt3d1(&dpt1, SQR_GRID_3D_RES);
Texture3D dpt3d2(&dpt2, SQR_GRID_3D_RES);
Texture3D dpt3d3(&dpt3, SQR_GRID_3D_RES);
Texture3D dpt3d4(&dpt4, SQR_GRID_3D_RES);

Texture3D colort3d(&colort, SQR_GRID_3D_RES);

//ShNoMIPFilter<ShTextureCube<ShColor3f> > em(EM_RES, EM_RES); //env map

/*ShTexture2D<ShColor4f> color_tex; 
Texture3D color_tex3d(&color_tex, SLICERES, SQRNUMSLICES);*/


/*ShImage hm_image;
ShImage nm_image;
ShImage em_imageA;
ShImage em_imageB;
ShImage em_imageC;
ShImage em_imageD;
ShImage em_imageE;
ShImage em_imageF;
ShImage terr_image;*/


//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

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

  float* expl = new float[] 


  float* testdata[5];	  
  for(int i=0;i<5;i++){ 
    testdata[i] = new float[NUM_GRID_CELLS*4];
    memset(testdata[i], 0, NUM_GRID_CELLS*4*sizeof(float));
    for(int j=0;j<10000;j++)
	  testdata[i][j] = ((float)rand())/(RAND_MAX-1);
  /*testdata[NUM_GRID_CELLS/2] = 1.0;
  testdata[NUM_GRID_CELLS/3] = 1.0;
  testdata[NUM_GRID_CELLS/4] = 1.0;*/
  }//for

  glGenTextures(5, dpt_ids[0]);
  glGenTextures(5, dpt_ids[1]);

  for(int i=0;i<2;i++){
  	  for(int j=0;j<5;j++){
		glBindTexture(GL_TEXTURE_2D, dpt_ids[i][j]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, i ? testdata : NULL);  
	  }//for
  }//for
 

// init eqdpt's  
  glGenTextures(5, eqdpt_ids);
  for(int i=0;i<5;i++){
		glBindTexture(GL_TEXTURE_2D, eqdpt_ids[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  
	  }//for

  char tid[5];
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, NULL);  

  sprintf(tid, "%d",dvt_id);
  dvt.meta("opengl:texid", tid);
 
  glBindTexture(GL_TEXTURE_2D, 0);

}


void setup_texouts(){

  dpt0.size(GRID_2D_RES, GRID_2D_RES);
  dpt1.size(GRID_2D_RES, GRID_2D_RES);
  dpt2.size(GRID_2D_RES, GRID_2D_RES);
  dpt3.size(GRID_2D_RES, GRID_2D_RES);
  dpt4.size(GRID_2D_RES, GRID_2D_RES);
  colort.size(GRID_2D_RES, GRID_2D_RES);

#ifndef USING_STREAMS

  /*ShHostMemoryPtr host_dp0 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp1 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp2 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp3 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp4 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_color = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);

  //ShHostMemoryPtr host_test = new ShHostMemory(3*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  test_image.loadPng("A.png");
  testt.memory(test_image.memory());


  ShHostStoragePtr dp0_storage = shref_dynamic_cast<ShHostStorage>(host_dp0->findStorage("host"));
  ShHostStoragePtr dp1_storage = shref_dynamic_cast<ShHostStorage>(host_dp1->findStorage("host"));
  ShHostStoragePtr dp2_storage = shref_dynamic_cast<ShHostStorage>(host_dp2->findStorage("host"));
  ShHostStoragePtr dp3_storage = shref_dynamic_cast<ShHostStorage>(host_dp3->findStorage("host"));
  ShHostStoragePtr dp4_storage = shref_dynamic_cast<ShHostStorage>(host_dp4->findStorage("host"));
  ShHostStoragePtr color_storage = shref_dynamic_cast<ShHostStorage>(host_color->findStorage("host"));
*/

  
  /*new GlTextureStorage(host_dp0->object(),
                                     GL_TEXTURE_2D,
                                     GL_RGBA,
                                     GL_RGBA_FLOAT16_ATI,
                                     host_dp0->value_type(),
                                     dpt0.width(), dpt0.height(), 
                                     dpt0.depth(), dpt0.size(),
                                     NUM_GRID_CELLS, dpt0.name(), 0);*/

 /* // these storages maybe cached on the graphics card, flag them
  // as dirty so that:
  // 1) the latest copies will be on the host 
  // 2) after reseting the stream, the graphics card copy will be updated
  dp0_storage->dirtyall();
  dp1_storage->dirtyall();
  dp2_storage->dirtyall();
  dp3_storage->dirtyall();
  dp4_storage->dirtyall();
  color_storage->dirtyall();

  for(int i=0;i<NUM_GRID_CELLS;i++){
    ((float*)(color_storage->data()) )[i*4] =   0.6;//static_cast<float>( (testt( ShAttrib1f((float)(i*3)/NUM_GRID_CELLS)  ))(0));
    ((float*)(color_storage->data()) )[i*4+1] = 0.3; //static_cast<float>( (testt(i*3)/NUM_GRID_CELLS )(1));
    ((float*)(color_storage->data()) )[i*4+2] = 0.1; //static_cast<float>( (testt(i*3)/NUM_GRID_CELLS )(2));
    ((float*)(color_storage->data()) )[i*4+3] = 0.5;
  
  }


  dpt0.memory(host_dp0);
  dpt1.memory(host_dp1);
  dpt2.memory(host_dp2);
  dpt3.memory(host_dp3);
  dpt4.memory(host_dp4);
  colort.memory(host_color);*/

#else

  dpt0.memory(dp0.memory());
  dpt1.memory(dp1.memory());
  dpt2.memory(dp2.memory());
  dpt3.memory(dp3.memory());
  dpt4.memory(dp4.memory());
  colort.memory(color.memory());

#endif

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

 // gravity = (imv | ShVector3f(.0,-9.8,.0));
    gravity = ShVector3f(.0,-9.8,.0);

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


void switchBuffers(){
//switch texture buffers:
  dbc++;
  dbc %= 2;
  idbc = (dbc+1)%2; //inverse dbc
  //printf("dbc: %d, idbc: %d\n", dbc, idbc);
  char tid[5];

  // set up texnodes to point to correct texids:
  sprintf(tid, "%d",dpt_ids[dbc][0]);
  dpt0.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[dbc][1]);
  dpt1.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[dbc][2]);
  dpt2.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[dbc][3]);
  dpt3.meta("opengl:texid", tid);
  sprintf(tid, "%d",dpt_ids[dbc][4]);
  dpt4.meta("opengl:texid", tid);

}

void initGL(){

		
}

void display()
  {
//printf("display\n");

  switchBuffers();	  
  glDisable(GL_DEPTH_TEST); // don't need depth test in stream programs


/////////////////////////////////////////////////////////////////////////////////////////////////////////	  
// Distribution packages update steps here:

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

/////////////////////////////////
// Calculate density and velocity:

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dvt_id, 0);

  shBind(*dv_shaders);
   drawQuad();
  shUnbind(*dv_shaders);

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
 
// Perform collision:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[idbc][0], 0);
  
  shBind(*collision0_shaders);
   drawQuad();
  shUnbind(*collision0_shaders);
  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[idbc][1], 0);
  
  shBind(*collision1_shaders);
   drawQuad();
  shUnbind(*collision1_shaders);

// Perform streaming:
  switchBuffers(); // need to consider the collision results and not overwrite them
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[idbc][0], 0);

  shBind(*streaming0_shaders);
   drawQuad();
  shUnbind(*streaming0_shaders);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[idbc][1], 0);

  shBind(*streaming1_shaders);
   drawQuad();
  shUnbind(*streaming1_shaders);


//glDisable(GL_DEPTH_TEST);
//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

/*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);*/

//camera.glProjection(1.0);


// cleanup
//glEnable(GL_DEPTH_TEST);
/*glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, 0, 0);*/


glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  
//test

glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

/*  shBind(*showeq0_shaders);
   drawQuad();
  shUnbind(*showeq0_shaders);*/

  shBind(*showvel_shaders);
   drawQuad();
  shUnbind(*showvel_shaders);

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Display pass here:
  
// normal rendering pass
  /*glActiveTexture(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, color_tex_id);

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);*/

  
  // Bind in the programs used to shade the particles
  // The updating of the stream is done using double buffering, if we just
  // ran particle_updateA then the latest data state is in stream 'B', otherwise
  // its in stream 'A' (because particle_updateB was the last to be run). Based
  // on the value of dir, render the appropriate stream
 /* float* particle_positions = 0;
  float* particle_velocities = 0;
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
    if (posA_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      posA_storage->sync();

      // fetch the raw data pointer and simply render the particles as points
      particle_positions = (float*)posA_storage->data();
      }
    ShHostStoragePtr velA_storage = shref_dynamic_cast<ShHostStorage>(velA.memory()->findStorage("host"));
    if (velA_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      velA_storage->sync();

      // fetch the raw data pointer and simply render the particles as points
      particle_velocities = (float*)velA_storage->data();
      }
  float* particle_times = 0;
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr timeA_storage = shref_dynamic_cast<ShHostStorage>(timeA.memory()->findStorage("host"));
    if (timeA_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      timeA_storage->sync();

      // fetch the raw data pointer and simply render the particles as points
      particle_times = (float*)timeA_storage->data();
      }
*/
  
 /* float* terrain_positions = 0;
  
    ShHostStoragePtr terrain_storage = shref_dynamic_cast<ShHostStorage>(terrain.memory()->findStorage("host"));
    if (terrain_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      terrain_storage->sync();

      // fetch the raw data pointer and simply render the particles as points
      terrain_positions = (float*)terrain_storage->data();
      }

 // skybox
  glDisable(GL_DEPTH_TEST);
  shBind(*skybox_shaders);
 // diffuse_color = ShColor3f(1.0, 1.0, 1.0);
 // glDisable(GL_LIGHTING);
 glBegin(GL_QUADS);

//  glTexCoord2f(0,1);
  glVertex3f(-1, 1, -1);
//  glTexCoord2f(1,1);
  glVertex3f( 1, 1, -1);
//  glTexCoord2f(1,0);
  glVertex3f( 1, 1,  1);
//  glTexCoord2f(0,0);
  glVertex3f(-1, 1,  1);

//  glTexCoord2f(0,1);
  glVertex3f(-1, -1, -1);
//  glTexCoord2f(1,1);
  glVertex3f( 1, -1, -1);
//  glTexCoord2f(1,0);
  glVertex3f( 1, 1, -1);
//  glTexCoord2f(0,0);
  glVertex3f(-1, 1, -1);

  glVertex3f( 1, -1, -1);
  glVertex3f( 1, -1,  1);
  glVertex3f( 1, 1,  1);
  glVertex3f( 1, 1, -1);

  glVertex3f( 1, -1,  1);
  glVertex3f(-1, -1,  1);
  glVertex3f(-1, 1,  1);
  glVertex3f( 1, 1,  1);

  glVertex3f(-1, -1,  1);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, 1, -1);
  glVertex3f(-1, 1,  1);

  glVertex3f(-1, -1, -1);
  glVertex3f(-1, -1,  1);
  glVertex3f( 1, -1,  1);
  glVertex3f( 1, -1, -1);

  glEnd();
  glEnable(GL_DEPTH_TEST);
*/

/*
#ifdef TERRAIN
 if (terrain_positions) {
    shBind(*terrain_shaders);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < (TM_RES-1)*(TM_RES-1)*6; i++)
      {
        glVertex3fv(&terrain_positions[3*i]);
      }
    glEnd();
  }
#endif // TERRAIN
 */
  
 /* 
// if(fmode > 0){
  if (particle_positions) {
  shBind(*particle_shaders);
  glBegin(GL_QUADS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   // for(int k=0;k<3;k++) mm[3](k) = particle_positions[3*i+k];
 
  glMultiTexCoord3fARB(GL_TEXTURE0, particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
  glMultiTexCoord1fARB(GL_TEXTURE1, particle_times[i]);
  glNormal3f(particle_velocities[3*i],particle_velocities[3*i+1],particle_velocities[3*i+2]);
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

  }//if
// }//if fmode
*/

 /*
  shBind(*particle_shaders);
  glBegin(GL_QUADS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   // for(int k=0;k<3;k++) mm[3](k) = particle_positions[3*i+k];
 
  //glMultiTexCoord3fARB(GL_TEXTURE0, ((float)(i%SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES, ((float)(i/SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES, 0);
  glTexCoord2f(((float)(i%SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES, ((float)(i/SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES);
  //glMultiTexCoord1fARB(GL_TEXTURE1, particle_times[i]);
  //glNormal3f(particle_velocities[3*i],particle_velocities[3*i+1],particle_velocities[3*i+2]);
  glVertex3f(-PCO, PCO, -PCO);
  glVertex3f(PCO, PCO, -PCO);
  glVertex3f(PCO, PCO, PCO);
  glVertex3f(-PCO, PCO, PCO);

  //glNormal3f(0, 0, -1);
  glVertex3f(-PCO, -PCO, -PCO);
  glVertex3f(PCO, -PCO, -PCO);
  glVertex3f(PCO, PCO, -PCO);
  glVertex3f(-PCO, PCO, -PCO);

  //glNormal3f(1, 0, 0);
  glVertex3f(PCO, -PCO, -PCO);
  glVertex3f(PCO, -PCO, PCO);
  glVertex3f(PCO, PCO, PCO);
  glVertex3f(PCO, PCO, -PCO);

  //glNormal3f(0, 0, 1);
  glVertex3f(PCO, -PCO, PCO);
  glVertex3f(-PCO, -PCO, PCO);
  glVertex3f(-PCO, PCO, PCO);
  glVertex3f(PCO, PCO, PCO);

  //glNormal3f(-1, 0, 0);
  glVertex3f(-PCO, -PCO, PCO);
  glVertex3f(-PCO, -PCO, -PCO);
  glVertex3f(-PCO, PCO, -PCO);
  glVertex3f(-PCO, PCO, PCO);

  //glNormal3f(0, -1, 0);
  glVertex3f(-PCO, -PCO, -PCO);
  glVertex3f(-PCO, -PCO, PCO);
  glVertex3f(PCO, -PCO, PCO);
  glVertex3f(PCO, -PCO, -PCO);
 
   }//for
  glEnd();
 */

 
 
 // Bind in the programs to shade the plane pillar
 
// glEnable(GL_TEXTURE_2D);
/* glBindTexture(GL_TEXTURE_2D, pb_texid);

// glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

 glBegin(GL_QUADS);

  glTexCoord2f(0,1);
  glVertex3f(-1, 1, -1);
  glTexCoord2f(1,1);
  glVertex3f( 1, 1, -1);
  glTexCoord2f(1,0);
  glVertex3f( 1, 1,  1);
  glTexCoord2f(0,0);
  glVertex3f(-1, 1,  1);

  glTexCoord2f(0,1);
  glVertex3f(-1, -1, -1);
  glTexCoord2f(1,1);
  glVertex3f( 1, -1, -1);
  glTexCoord2f(1,0);
  glVertex3f( 1, 1, -1);
  glTexCoord2f(0,0);
  glVertex3f(-1, 1, -1);

  glVertex3f( 1, -1, -1);
  glVertex3f( 1, -1,  1);
  glVertex3f( 1, 1,  1);
  glVertex3f( 1, 1, -1);

  glVertex3f( 1, -1,  1);
  glVertex3f(-1, -1,  1);
  glVertex3f(-1, 1,  1);
  glVertex3f( 1, 1,  1);

  glVertex3f(-1, -1,  1);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, 1, -1);
  glVertex3f(-1, 1,  1);

  glVertex3f(-1, -1, -1);
  glVertex3f(-1, -1,  1);
  glVertex3f( 1, -1,  1);
  glVertex3f( 1, -1, -1);

  glEnd();*/


  
  /*
  shBind(*plane_shaders);

  // draw volume slabs
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glBegin(GL_QUADS);
  for(int i=0; i<(GRID_3D_RES); i++){
   float pz = (i*2.0*SCALE)/GRID_3D_RES-SCALE;
   //glTexCoord2f(0,0);
   glVertex3f(-SCALE, -SCALE, pz);
   //glTexCoord2f(1,0);
   glVertex3f( SCALE, -SCALE, pz);
   //glTexCoord2f(1,1);
   glVertex3f( SCALE, SCALE,  pz);
   //glTexCoord2f(0,1);
   glVertex3f(-SCALE, SCALE,  pz);
  }
  glEnd();
  glDisable(GL_BLEND);
  //glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
 
  shUnbind(*plane_shaders);
 */
 
  /*
  // Help information
  if (show_help)
    {
    gprintf(30, 160, "Sh Stream Example Help");
    gprintf(30, 135, "  'R' - Reset simulation");
    gprintf(30, 120, "  'S' - Single step simulation");
    gprintf(30, 105, "Space - Start/stop simulation");
    gprintf(30, 90,  "  'H' - Show/hide help");
    gprintf(30, 75,  "  'Q' - Quit");
    }
  else
    {
    gprintf(10, 10, "'H' for help...");
    }*/

  
  glutSwapBuffers();

  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);


  /*
// off screen pass     
// unbind texture
  glActiveTexture(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);		  

// render particle positions into texture
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb1);
  //setup_view();

  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  // Set states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);


  shBind(*particle_volume_shaders);
  glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
  glEnable( GL_POINT_SPRITE_ARB );
  glPointSize(5.0f);
  glBegin(GL_POINTS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   //glVertex3f(particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
    //glMultiTexCoord3fARB(GL_TEXTURE1, particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
   //glMultiTexCoord1fARB(GL_TEXTURE2, particle_times[i]);
   //glNormal3f(particle_velocities[3*i],particle_velocities[3*i+1],particle_velocities[3*i+2]);
   glVertex3f( ((float)(i%SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES, ((float)(i/SQR_NUM_PARTICLES))/SQR_NUM_PARTICLES, 0);
  }//for
  glEnd();
  glDisable( GL_POINT_SPRITE_ARB );
   

  glDisable(GL_BLEND);
  
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
*/
  
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
  }

void mouse(int button, int state, int x, int y)
  {
  buttons[button] = state;
  cur_x = x;
  cur_y = y;
  }

void idle(void)
  {
  #ifdef USING_STREAMS
   update_streams();
  #endif
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
      #ifdef USING_STREAMS
       reset_streams();
      #endif
      break;
      }
    case 's':
    case 'S':
      {
	  #ifdef USING_STREAMS
       update_streams();
      #endif
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
    #ifdef USING_STREAMS 
	 init_streams();
    #endif
  
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

#ifdef USING_STREAMS
void init_streams(void)
  {
  // Specifiy the generic particle update program, later it will
  // be specialized for the the actual particle update.
  streaming = SH_BEGIN_PROGRAM("gpu:stream") {
    ShOutputColor4f dp0;
    ShOutputColor4f dp1;
    ShOutputColor4f dp2;
    ShOutputColor4f dp3;
    ShOutputColor4f dp4;
    ShOutputColor4f color;
    ShInputTexCoord2f currtc;

    dp0(0) = (dpt3d0.find12(currtc))(0);

    
/*    // parameters controlling the amount of momentum
    // tranfer on collision
    ShAttrib1f mu(0.8);
    ShAttrib1f eps(0.6);

    // check if below ground level
    ShAttrib1f elev = (hm( (pos(0,2)+SCALE)/(2.0*SCALE)) )(0);
    ShAttrib1f under = pos(1) < elev;

    ShAttrib1f out = (pos(0) < -SCALE) || (pos(0) > SCALE) || (pos(2) < -SCALE) || (pos(2) > SCALE);
    time(0) -= lifeStep*ShAttrib1f(LIFE_STEP_FACTOR);
    time(0) = cond(out, ShAttrib1f(0.0), time(0));
    pos = cond(time(0), pos, ShPoint3f(0.0,1.0,0.0));
    vel = cond(time(0), vel, initvel);
    time(0) = cond(time(0), time(0), ShAttrib1f(1.0));
 
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
    pos = cond(onsurface&&minspeed,pos,pos + vel*delta);
*/
    
      } SH_END;

 
  ShHostMemoryPtr host_dp0 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp1 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp2 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp3 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_dp4 = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);
  ShHostMemoryPtr host_color = new ShHostMemory(4*sizeof(float)*NUM_GRID_CELLS, SH_FLOAT);



/*host_posA->setTag("host_posA");
host_velA->setTag("host_velA");
host_initvelA->setTag("host_initvelA");
host_timeA->setTag("host_timeA");*/


  dp0 = ShChannel<ShVector4f>(host_dp0, NUM_GRID_CELLS);
  dp1 = ShChannel<ShVector4f>(host_dp1, NUM_GRID_CELLS);
  dp2 = ShChannel<ShVector4f>(host_dp2, NUM_GRID_CELLS);
  dp3 = ShChannel<ShVector4f>(host_dp3, NUM_GRID_CELLS);
  dp4 = ShChannel<ShVector4f>(host_dp4, NUM_GRID_CELLS);
  color = ShChannel<ShVector4f>(host_color, NUM_GRID_CELLS);
    
  //ShStream data = posA & velA & initvelA & timeA;
  
  lbm_update = (streaming /*<< dataA << gravity << delta*/);

  // Everything has been setup for the particle system to operate, last
  // thing is to set the initial data.
  reset_streams();
  }

void reset_streams(void)
  {
  // Set our time counter to zero
  simtime = 0.0;
    
  // Use the ShChannels objects to find the associated ShHostStorage and
  // then fetch the raw data and cast to a float pointer.
  ShHostStoragePtr dp0_storage = shref_dynamic_cast<ShHostStorage>(dp0.memory()->findStorage("host"));
  ShHostStoragePtr dp1_storage = shref_dynamic_cast<ShHostStorage>(dp1.memory()->findStorage("host"));
  ShHostStoragePtr dp2_storage = shref_dynamic_cast<ShHostStorage>(dp2.memory()->findStorage("host"));
  ShHostStoragePtr dp3_storage = shref_dynamic_cast<ShHostStorage>(dp3.memory()->findStorage("host"));
  ShHostStoragePtr dp4_storage = shref_dynamic_cast<ShHostStorage>(dp4.memory()->findStorage("host"));


  // these storages maybe cached on the graphics card, flag them
  // as dirty so that:
  // 1) the latest copies will be on the host 
  // 2) after reseting the stream, the graphics card copy will be updated
  dp0_storage->dirtyall();
  dp1_storage->dirtyall();
  dp2_storage->dirtyall();
  dp3_storage->dirtyall();
  dp4_storage->dirtyall();

  float* dp0_data = (float*)dp0_storage->data()

  for(int i=0;i<NUM_GRID_CELLS;i++){
  	dp0_data[4*i]   = 0.5*((float)rand())/(RAND_MAX-1);
        dp0_data[4*i+1] = 0;
        dp0_data[4*i+2] = 0;
        dp0_data[4*i+3] = 0;
   } 	  

 /* float* posA_data = (float*)posA_storage->data();
  float* velA_data = (float*)velA_storage->data();
  float* initvelA_data = (float*)initvelA_storage->data();
  float* timeA_data = (float*)timeA_storage->data();


  // Iterate over the particle and initialize their state
  for(int i = 0; i < NUM_PARTICLES; i++)
    {
    // All the particles will start at the same places
    posA_data[3*i+0] = 0;
    posA_data[3*i+1] = 1;
    posA_data[3*i+2] = 0;

    // The velocity direction will vary over a small cone of angles.
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
    velA_data[3*i+2] = 0;

    timeA_data[3*i] = i*(1.0/NUM_PARTICLES);

    lifeStep = ShAttrib1f(1.0/NUM_PARTICLES);
    
    }
*/
  }

void update_streams(void)
  {

  try 
    {
      dp0 & dp1 & dp2 & dp3 & dp4 & color = lbm_update;

	    
      simtime += timedelta;
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
#endif //USING_STREAMS

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
    velocity += ShAttrib3f( 0, 0, 0)*dpt4(tc)(2);
    
    velocity /= density;
    
    color(0,1,2) = velocity;
    color(3) = density;
    
  } SH_END;

  eq0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //constants:
    ShAttrib1f A, B, C, D;
    A = 1.0/18;
    B = 3.0/18;
    C = 1.0/4;
    D = -1.0/12;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc))(0,1,2);
    eu(0) = ShVector3f(1,0,0)  | u;
    eu(1) = ShVector3f(-1,0,0) | u;
    eu(2) = ShVector3f(0,1,0)  | u;
    eu(3) = ShVector3f(0,-1,0) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A + B*eu + C*eu2 + D*uu)*(dvt(tc))(3);
    
  } SH_END;

 eq1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //constants:
    ShAttrib1f A, B, C, D;
    A = 1.0/18;
    B = 3.0/18;
    C = 1.0/4;
    D = -1.0/12;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = dvt(tc)(0,1,2);
    eu(0) = ShVector3f(1,1,0)  | u;
    eu(1) = ShVector3f(-1,-1,0) | u;
    eu(2) = ShVector3f(1,-1,0)  | u;
    eu(3) = ShVector3f(-1,1,0) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A + B*eu + C*eu2 + D*uu)*dvt(tc)(3);
    
  } SH_END;

 eq2_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //constants:
    ShAttrib1f A, B, C, D;
    A = 1.0/18;
    B = 3.0/18;
    C = 1.0/4;
    D = -1.0/12;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = dvt(tc)(0,1,2);
    eu(0) = ShVector3f(1,0,1)  | u;
    eu(1) = ShVector3f(-1,0,-1) | u;
    eu(2) = ShVector3f(1,0,-1)  | u;
    eu(3) = ShVector3f(-1,0,1) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A + B*eu + C*eu2 + D*uu)*dvt(tc)(3);
    
  } SH_END;

 eq3_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //constants:
    ShAttrib1f A, B, C, D;
    A = 1.0/18;
    B = 3.0/18;
    C = 1.0/4;
    D = -1.0/12;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = dvt(tc)(0,1,2);
    eu(0) = ShVector3f(0,1,1)  | u;
    eu(1) = ShVector3f(0,-1,-1) | u;
    eu(2) = ShVector3f(0,1,-1)  | u;
    eu(3) = ShVector3f(0,-1,1) | u;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A + B*eu + C*eu2 + D*uu)*dvt(tc)(3);
    
  } SH_END;

 eq4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //constants:
    ShAttrib1f A, B, C, D;
    A = 1.0/18;
    B = 3.0/18;
    C = 1.0/4;
    D = -1.0/12;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = dvt(tc)(0,1,2);
    eu(0) = ShVector3f(0,0,1)  | u;
    eu(1) = ShVector3f(0,0,-1) | u;
    eu(2) = 0;
    eu(3) = 0;
    
    ShAttrib4f eu2 = eu * eu;
    ShAttrib1f uu = u|u;
     
    color = (A + B*eu + C*eu2 + D*uu)*dvt(tc)(3);
    
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

 /* streaming0_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord2f tc;
    //ShOutputPosition3f posp;
      
    //posp = pos(0,1,2);
    //tc = color_tex3d.get2DTexCoord(pos(0,1,2)/5);
    //pos = mvd | pos;
        
    } SH_END;*/

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  streaming0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    //ShInputPosition4f posp;
    ShOutputColor4f color;

    //tc(0) = tc(0)+0.01;   
    color(0) = (dpt3d0.find12(tc))(0);
    color(1) = (dpt3d0.find13(tc))(1);
    color(2) = (dpt3d0.find15(tc))(2);
    color(3) = (dpt3d0.find10(tc))(3);
///color = ShColor4f(0.2, 0.4, 0.1, 0.5);
    
  } SH_END;

  streaming1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    //ShInputPosition4f posp;
    ShOutputColor4f color;

    //tc(0) = tc(0)+0.01;   
    color(0) = (dpt3d0.find14(tc))(0);
    color(1) = (dpt3d0.find11(tc))(1);
    color(2) = (dpt3d0.find9(tc))(2);
    color(3) = (dpt3d0.find16(tc))(3);
///color = ShColor4f(0.2, 0.4, 0.1, 0.5);
    
  } SH_END;

  showvel_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color(0,1,2) = dvt(tc)(0,1,2);
    color(3) = 1.0;
    
  } SH_END;
  showeq0_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color(0,1,2) = eqdpt0(tc)(0,1,2);
    color(3) = 1.0;
    
  } SH_END;
  
 /*  plane_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    //ShInOutTexCoord2f tc;
    ShOutputPosition3f posp;
      
    posp = pos(0,1,2);
    //tc = color_tex3d.get2DTexCoord(pos(0,1,2)/5);
    pos = mvd | pos;
        
    } SH_END;

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  plane_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    //ShInputTexCoord2f tc;
    ShInputPosition4f posp;
    ShOutputColor4f color;

       
    color = dpt3d0(posp(0,1,2)/SCALE);
    
  } SH_END;*/


 /*   skybox_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShOutputPosition3f posp;
    ShInOutTexCoord2f tc;

    posp = pos(0,1,2);
    pos(0,1,2) += wCamOrig;
    
    pos = mvd | pos;

  } SH_END;

    skybox_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputPosition3f posp;
    ShInputTexCoord2f tc;
    ShOutputColor3f color;

       color = em(posp); 
  } SH_END;

    
    terrain_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShOutputTexCoord2f tc;
    ShOutputPoint4f posp;
    //ShInOutNormal3f normal;
    ShOutputVector3f lightv;
   
    // Get the Y value from heightmap
    //pos(1) = pos(1) + 5.0*(terrt(pos(0,2)))(1); 
    posp = pos;
    tc = pos(0,2)/(2.0*SCALE)+.5;
       
    // Compute viewspace position
    ShPoint3f posv = (mv | posp)(0,1,2);

    // Compute light direction
    lightv = lightPos - posv;
    
    // Project position
    pos = mvd | posp;

    // Project normal
    //normal = mv | normal;

  } SH_END;

    terrain_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShInputPosition4f posp;
//ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShOutputColor3f color;

    //normal = normalize(normal);
    lightv = normalize(lightv);
    ShVector3f norm( (nm((posp(0,2)+SCALE)/(2.0*SCALE)))(0,1,2)*2-1.0  );
    
    norm = normalize(norm);
    norm(0)=-norm(0);
    lightv = normalize(lightv);
    
    
    //color = (norm|lightv)*terrt(tc)*ShColor3f(0.9,0.7,0.5); //terrain color;
    //color = ShConstColor3f(0.5, 0.5, 0.5);
    color = (norm|lightv)*((texpos(tc))(0,1,2))*ShColor3f(0.9,0.7,0.5); //terrain color;
  } SH_END;

     
  // This vertex program will be used for all our shading, it
  // simply transforms the incoming position/normal and generates
  // a light vector. 
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
   // ShInOutAttrib1f lifetime;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    // Compute viewspace position
    ShPoint3f posv = (mv | pos)(0,1,2);

    // Compute light direction
    lightv = lightPos - posv;
    
    // Project position
    pos = mvd | pos;

    // Project normal
    //normal = mv | normal;

  } SH_END;

    particle_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord3f partpos;
    ShInOutTexCoord1f parttime;
    ShInputNormal3f vel; // not real normal
    //ShOutputPoint3f posv;
    ShOutputVector3f camray;
    ShOutputPoint3f posm; // position in MS
    ShOutputVector3f lightv;
    ShOutputMatrix4x4f omtm;

    // Compute camera ray
    //WS
    camray = ShPoint3f(pos(0,1,2)) - wCamOrig;
    //->MS
    ShMatrix4x4f mtm, imtm; // inverse model transformation matrix
    mtm[3](0,1,2) = ShVector3f(partpos); //absolute position of the particle
    mtm[1](0,1,2) = vel/3 + normalize(vel);
    mtm[0](0,1,2) = normalize(-partpos);
    mtm[2](0,1,2) = normalize(mtm[0](0,1,2) ^ mtm[1](0,1,2));
    mtm[0](0,1,2) = normalize(mtm[2](0,1,2) ^ mtm[1](0,1,2));
    mtm = transpose(mtm); 
    omtm = mtm;
    imtm = inverse(mtm);
    camray = imtm(0,1,2)(0,1,2) | camray;
    posm = (imtm | pos)(0,1,2);
    lightv = -(imtm|lightPos);
        
    // Compute viewspace position
   // posv = (mv | pos)(0,1,2);

    // Compute light direction
    //lightv = lightPos - posv;
    
    // Project position
    pos = mvd | pos;

    // Project normal
    //normal = mv | normal;

  } SH_END;

 // This fragment shader will be used to shade our particle. Its
  // just assigns a uniform color (point_color). The above vertex
  // program will be used in conjunction with this fragment program
  // the transformed normal and generated light vector are simply
  // ignored.
  particle_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
   ShInputTexCoord3f partpos;
   ShInputTexCoord1f parttime;
   //ShInputPoint3f posv;
   ShInputVector3f camray;
   ShInputPoint3f posm;
   ShInputVector3f lightv;
   ShInputMatrix4x4f mtm;
   ShOutputColor3f color;

   camray = normalize(camray); 	  
   // now posv and camray gives us the full camera ray data
   
     
   // solve for intersection
   ShAttrib1f r1 = .02;
   ShAttrib1f s1 = (-posm)|camray;
   ShAttrib1f l2 = (-posm)|(-posm);
   discard(l2-s1*s1-r1*r1);

   // find the solution
   ShAttrib1f t1 = s1 - sqrt(r1*r1-l2+s1*s1);
   ShVector3f norm = normalize(mtm(0,1,2)(0,1,2) | (t1*camray+posm));
   //ShAttrib1f in = normalize(lightv) | norm;
//   color = em(mtm(0,1,2)(0,1,2) | (t1*camray+posm));
   
   ShVector3f reflv = reflect(camray,norm);
   ShVector3f refrv = refract(-camray,norm,ShAttrib1f(1.0/1.5));
   ShVector1f fres = fresnel(camray,norm,ShAttrib1f(1.5));
   //fres = 1.0;
   color = fres*em(reflv)(0,1,2) + (1.0f-fres)*em(refrv)(0,1,2); 
   //ShAttrib1f lifetime(1.0-parttime);
   color = lerp(parttime*parttime*parttime*parttime-.2, ShConstColor3f(1.0, 1.0, 1.0), color); 

   } SH_END;

  particle_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInputTexCoord2f partpos2d;
    //ShInOutTexCoord3f partpos;
    //ShInOutTexCoord1f parttime;
    //ShInputNormal3f vel; // not real normal
    //ShOutputPoint3f posv;
    ShOutputVector3f camray;
    ShOutputPoint3f posm; // position in MS
    ShOutputVector3f lightv;
    ShOutputMatrix4x4f omtm;

    ShVector3f vel(0,1,0);
    ShVector3f partpos(0,0,0);
    
    //partpos = (texpos(partpos2d(0,1)))(0,1,2);
    partpos(0,1) = partpos2d(0,1);
    pos(0,1,2) = pos(0,1,2) + partpos(0,1,2); 
    //pos(0,1,2) = pos(0,1,2) + partpos(0,1,2); 

    // Compute camera ray
    //WS
    camray = ShPoint3f(pos(0,1,2)) - wCamOrig;
    //->MS
    ShMatrix4x4f mtm, imtm; // inverse model transformation matrix
    mtm[3](0,1,2) = ShVector3f(partpos); //absolute position of the particle
    mtm[1](0,1,2) = vel/3 + normalize(vel);
    mtm[0](0,1,2) = normalize(-partpos);
    mtm[2](0,1,2) = normalize(mtm[0](0,1,2) ^ mtm[1](0,1,2));
    mtm[0](0,1,2) = normalize(mtm[2](0,1,2) ^ mtm[1](0,1,2));
    mtm = transpose(mtm); 
    omtm = mtm;
    imtm = inverse(mtm);
    camray = imtm(0,1,2)(0,1,2) | camray;
    posm = (imtm | pos)(0,1,2);
    lightv = -(imtm|lightPos);
        
    // Compute viewspace position
   // posv = (mv | pos)(0,1,2);

    // Compute light direction
    //lightv = lightPos - posv;
    
    // Project position
    pos = mvd | pos;

    // Project normal
    //normal = mv | normal;

  } SH_END;

 // This fragment shader will be used to shade our particle. Its
  // just assigns a uniform color (point_color). The above vertex
  // program will be used in conjunction with this fragment program
  // the transformed normal and generated light vector are simply
  // ignored.
  particle_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
   //ShInputTexCoord3f partpos;
   //ShInputTexCoord1f parttime;
   //ShInputPoint3f posv;
   ShInputVector3f camray;
   ShInputPoint3f posm;
   ShInputVector3f lightv;
   ShInputMatrix4x4f mtm;
   ShOutputColor3f color;

   camray = normalize(camray); 	  
   // now posv and camray gives us the full camera ray data
   
     
   // solve for intersection
   ShAttrib1f r1 = .02;
   ShAttrib1f s1 = (-posm)|camray;
   ShAttrib1f l2 = (-posm)|(-posm);
   discard(l2-s1*s1-r1*r1);

   // find the solution
   ShAttrib1f t1 = s1 - sqrt(r1*r1-l2+s1*s1);
   ShVector3f norm = normalize(mtm(0,1,2)(0,1,2) | (t1*camray+posm));
   //ShAttrib1f in = normalize(lightv) | norm;
//   color = em(mtm(0,1,2)(0,1,2) | (t1*camray+posm));
   
   ShVector3f reflv = reflect(camray,norm);
   ShVector3f refrv = refract(-camray,norm,ShAttrib1f(1.0/1.5));
   ShVector1f fres = fresnel(camray,norm,ShAttrib1f(1.5));
   //fres = 1.0;
   color = fres*em(reflv)(0,1,2) + (1.0f-fres)*em(refrv)(0,1,2); 
   //ShAttrib1f lifetime(1.0-parttime);
   //color = lerp(parttime*parttime*parttime*parttime-.2, ShConstColor3f(1.0, 1.0, 1.0), color); 

   //color = cond(l2-s1*s1-r1*r1, ShConstColor3f(0.0, 0.0, 0.0), ShConstColor3f(1.0, 1.0, 1.0));
 //     color = ShConstColor3f(1.0, 1.0, 1.0);
      
      //color = ShColor3f(col, col, col);
     } SH_END;
 
   
 particle_volume_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord2f tc;
    //ShInOutTexCoord3f partpos;
    //ShInOutTexCoord1f parttime;
    ShInputNormal3f vel; // not real normal
    ShOutputVector3f camray;
    ShOutputPoint3f posm; // position in MS
    ShOutputVector3f lightv;
    ShOutputMatrix4x4f omtm;

    
    // Compute camera ray
    //WS
    camray = ShPoint3f(pos(0,1,2)) - wCamOrig;
    //->MS
    ShMatrix4x4f mtm, imtm; // inverse model transformation matrix
    mtm[3](0,1,2) = ShVector3f(partpos); //absolute position of the particle
    mtm[1](0,1,2) = vel/3 + normalize(vel);
    mtm[0](0,1,2) = normalize(-partpos);
    mtm[2](0,1,2) = normalize(mtm[0](0,1,2) ^ mtm[1](0,1,2));
    mtm[0](0,1,2) = normalize(mtm[2](0,1,2) ^ mtm[1](0,1,2));
    mtm = transpose(mtm); 
    omtm = mtm;
    imtm = inverse(mtm);
    camray = imtm(0,1,2)(0,1,2) | camray;
    posm = (imtm | pos)(0,1,2);
    lightv = -(imtm|lightPos);
        
      // Project position
    //pos = mvd | pos;
  
    ShMatrix4x4f v = mv; // just view, no translation
    v = transpose(v);
    v[3](0,1,2) = ShVector3f(0.0, 0.0, 0.0);
    v = transpose(v);

    //ShTexCoord2f vtc = pos(0,1);
    //pos(0,1,2) = ( texpos(vtc) )(0,1,2);
    pos = v | pos;
    //pos(0,1,2) = color_tex3d.scatter(pos(0,1,2),SCALE);
    

   } SH_END;

  particle_volume_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
   ShInputTexCoord2f tc;
   //ShInputTexCoord3f partpos;
   //ShInputTexCoord1f parttime;
   ShInputVector3f camray;
   ShInputPoint3f posm;
   ShInputVector3f lightv;
   ShInputMatrix4x4f mtm;
   ShOutputColor4f color;

     color = ShConstColor4f(1.0, 1.0, 1.0, 1.0);//ShConstColor4f(1.0, 1.0, 1.0, 0.3);
     color(3) = terrt(tc)(0)*(1.0/(1*SLICEFACTOR));
   } SH_END;
*/
	
 

/*  particle_shaders = new ShProgramSet(particle_vsh, particle_fsh);
  particle_volume_shaders = new ShProgramSet(particle_volume_vsh, particle_volume_fsh);
  terrain_shaders = new ShProgramSet(terrain_vsh, terrain_fsh);
  skybox_shaders = new ShProgramSet(skybox_vsh, skybox_fsh);*/
  //plane_shaders = new ShProgramSet(plane_vsh, plane_fsh);
  dv_shaders = new ShProgramSet(vsh, dv_fsh);
  eq0_shaders = new ShProgramSet(vsh, eq0_fsh);
  eq1_shaders = new ShProgramSet(vsh, eq1_fsh);
  eq2_shaders = new ShProgramSet(vsh, eq2_fsh);
  eq3_shaders = new ShProgramSet(vsh, eq3_fsh);
  eq4_shaders = new ShProgramSet(vsh, eq4_fsh);
  collision0_shaders = new ShProgramSet(vsh, collision0_fsh);
  collision1_shaders = new ShProgramSet(vsh, collision1_fsh);
  streaming0_shaders = new ShProgramSet(vsh, streaming0_fsh);
  streaming1_shaders = new ShProgramSet(vsh, streaming1_fsh);
///test
  showvel_shaders = new ShProgramSet(vsh, showvel_fsh);
  showeq0_shaders = new ShProgramSet(vsh, showeq0_fsh);
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
