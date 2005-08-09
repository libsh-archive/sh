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

#define INTERNAL_FORMAT GL_RGBA16 //GL_RGBA_FLOAT32_ATI

#define NUMMODES 1

#define SO1 0.005
#define SO2 0.3
#define SCALE 5
#define GRID_2D_RES 512
#define SQR_GRID_3D_RES 8
#define GRID_3D_RES SQR_GRID_3D_RES*SQR_GRID_3D_RES
#define NUM_GRID_CELLS GRID_3D_RES*GRID_3D_RES*GRID_3D_RES

#define RCPSQR2 0.707106781186

#define TAU 5.2
#define INV_TAU (1.0f/TAU)

// forward declarations
void init_shaders(void);
void init_textures(void);
void init_FBO(void);
void initGL(void);

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
ShProgram redonly_fsh;
ShProgram greenonly_fsh;
ShProgram mouse0_fsh;
ShProgram mouse1_fsh;



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
ShProgramSet* redonly_shaders;
ShProgramSet* greenonly_shaders;
ShProgramSet* mouse0_shaders;
ShProgramSet* mouse1_shaders;

//test vars
ShAttrib3f dirs; 


#ifndef USING_STREAMS
//test
ShImage test_image;
ShArray1D<ShColor4f> testt;

//distribution package texture ids
GLuint dpt_ids[5];
GLuint dptc_ids[5];
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

ShArray2D<ShColor4f> colort;

Texture3D dptc3d0(&dptc0, SQR_GRID_3D_RES);
Texture3D dptc3d1(&dptc1, SQR_GRID_3D_RES);
Texture3D dptc3d2(&dptc2, SQR_GRID_3D_RES);
Texture3D dptc3d3(&dptc3, SQR_GRID_3D_RES);
Texture3D dptc3d4(&dptc4, SQR_GRID_3D_RES);

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
    testdata[4][i*4+2] = 0.001;
  }

  for(int i=0;i<5;i++){
		glBindTexture(GL_TEXTURE_2D, dpt_ids[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, GRID_2D_RES, GRID_2D_RES, 0, GL_RGBA, GL_FLOAT, testdata[i]);  
  }//for
  char tid[5];
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
 
  glBindTexture(GL_TEXTURE_2D, 0);

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
 
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, eqdpt_ids[4], 0);

  shBind(*eq4_shaders);
   drawQuad();
  shUnbind(*eq4_shaders);

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


// Perform streaming:
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, dpt_ids[0], 0);

 
  shBind(*streaming0_shaders);
   drawQuad();
  shUnbind(*streaming0_shaders);

 if(buttons[0] == GLUT_DOWN){ 
  dirs(0) = ((float)ddx)/5;
  dirs(1) = ((float)ddy)/5;
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
  

/////////////
  
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

  shBind(*showvel_shaders);
   drawQuad();
  shUnbind(*showvel_shaders);

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

    //constants:
    ShAttrib1f A, B, C, D;
    A = 0.05555555555;
    B = 0.16666666666;
    C = 0.25;
    D = -0.0833333333;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = ((dvt(tc))(0,1,2))*2-1.0;
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
    A = 0.027777777777;
    B = 0.083333333333;
    C = 0.125;
    D = -0.04166666666;
    
    // inner products:
    ShAttrib4f eu;
    ShVector3f u = (dvt(tc)(0,1,2))*2-1.0;
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
    A = 0.027777777777;
    B = 0.083333333333;
    C = 0.125;
    D = -0.04166666666; 
    
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
    A = 0.027777777777;
    B = 0.083333333333;
    C = 0.125;
    D = -0.04166666666;   
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
    A = 0.05555555555;
    B = 0.16666666666;
    C = 0.25;
    D = -0.0833333333;   
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
    color(2) = (0.3333333 - 0.5*uu)*dvt(tc)(3); 
    
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

    //tc(0) = tc(0)+0.01;   
    color(0) = (dptc3d0.find12(tc))(0);
    color(1) = (dptc3d0.find13(tc))(1);
    color(2) = (dptc3d0.find15(tc))(2);
    color(3) = (dptc3d0.find10(tc))(3);

    
  } SH_END;

  streaming1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //tc(0) = tc(0)+0.01;   
    color(0) = (dptc3d1.find14(tc))(0);
    color(1) = (dptc3d1.find11(tc))(1);
    color(2) = (dptc3d1.find9(tc))(2);
    color(3) = (dptc3d1.find16(tc))(3);

    
  } SH_END;

  streaming4_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //tc(0) = tc(0)+0.01;   
    color(0) = dptc4(tc)(0);//(dptc3d4.find14(tc))(0);
    color(1) = dptc4(tc)(1);//(dptc3d4.find11(tc))(1);
    color(2) = dptc4(tc)(2);
    color(3) = 0;

    
  } SH_END;

  showvel_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    color(0,2) = abs((dvt(tc)(0,1))*2-1.0);
	color(1) = 0;
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
    color(0) = clamp(dirs|ShAttrib3f(1,0,0), 0, 1.0);
    color(1) = clamp(dirs|ShAttrib3f(-1,0,0), 0, 1.0);
    color(2) = clamp(dirs|ShAttrib3f(0,1,0), 0, 1.0);
    color(3) = clamp(dirs|ShAttrib3f(0,-1,0), 0, 1.0);
   } SH_END;

   mouse1_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShOutputColor4f color;

    //color = ShConstColor4f(1,0,0,1);
    color(0) = RCPSQR2*clamp(dirs|ShAttrib3f(1,1,0), 0, 1.0);
    color(1) = RCPSQR2*clamp(dirs|ShAttrib3f(-1,-1,0), 0, 1.0);
    color(2) = RCPSQR2*clamp(dirs|ShAttrib3f(1,-1,0), 0, 1.0);
    color(3) = RCPSQR2*clamp(dirs|ShAttrib3f(-1,1,0), 0, 1.0);
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
