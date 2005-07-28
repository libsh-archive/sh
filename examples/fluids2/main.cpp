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
//#include <GL/gl.h>
//#include <GL/glext.h>
#include <GL/glut.h>

#include <GL/glxew.h>
//#include <GL/glx.h>
//#include <GL/glxext.h>


#include <sh/sh.hpp>

using namespace std;

#include "Camera.hpp"

using namespace SH;
// using namespace std;

// defines

#define NUM_PARTICLES 8192
#define HM_RES 256 // height map texture resolution
#define TM_RES 128 // terrain mesh resolution
#define EM_RES 512 // env map resolution
#define TT_RES 256 // terrain texture resolution

// scene objects
#define TERRAIN


//#define LIFE_TIME 2.0
#define LIFE_STEP_FACTOR 25.0
#define MIN_SPEED 0.2
#define DAMP_FACTOR 0.4

#define PBUFFERSIZE 512
#define NUMSLICES 8 // actually the square root of the number of slices
#define SLICEFACTOR 4 // factor of increasing of the numberof slices when rendering the volume
#define RESOLUTION 64

#define NUMMODES 2

#define PCO .05 // Particle Cube Offset

#define SCALE 5.0f

ShAttrib1f lifeStep;

// forward declarations
void init_shaders(void);
void init_streams(void);
void init_pbuffers(void);
void reset_streams(void);
void update_streams(void);
void setup_terrain_stream(void);
void load_textures(void);
int gprintf(int x, int y, char* fmt, ...);
ShAttrib1f
fresnel (
   ShVector3f v, 
   ShNormal3f n, 
   ShAttrib1f eta
);

class Texture3D{
 
 public:
 Texture3D(ShTexture2D<ShColor4f> tex2d, unsigned int res/*size of one dimension in each slice*/, unsigned int ns/*number of slices in one row, total number of slices would be ns^2*/){
    texture = tex2d;	
    numslices = ns;
    res2d = res;
 }
 ShColor4f operator() (ShVector3d tc3d){
     ShTexCoord2f tc, tc2;
     ShAttrib1f ss = numslices*numslices; // real number of slices
     ShAttrib1f rss = 1.0/ss; // reciprocal of ss
    ShAttrib1f a =  (tc3d(2)/(2.0)+0.5)/rss ;
    ShAttrib1f a1 = floor( (tc3d(2)/(2.0)+0.5)/rss );
     ShAttrib1f a2 = a1+1.0;

     ShAttrib1f lx = tc3d(0)/numslices;
     ShAttrib1f ly = tc3d(1)/numslices;

     ShAttrib1f r2n = 2.0/numslices;

     tc  = ShTexCoord2f((lx + r2n*(-(numslices-1.0)/2 +        a1%numslices  ) )/2+ShAttrib1f(0.5),
     			(ly + r2n*(-(numslices-1.0)/2 +  floor(a1/numslices) ) )/2+ShAttrib1f(0.5));
   
     tc2 = ShTexCoord2f((lx + r2n*(-(numslices-1.0)/2 +        a2%numslices  ) )/2+ShAttrib1f(0.5),
     			(ly + r2n*(-(numslices-1.0)/2 +  floor(a2/numslices) ) )/2+ShAttrib1f(0.5));
   
 

     
     
     /*  tc2 = ShTexCoord2f((tc3d(0)/(numslices) + (2.0/numslices)*(-(numslices-1.0)/2 + floor(((tc3d(2)+(1.0/(numslices*numslices)))/(2.0)+0.5) * numslices*numslices)%numslices) )/2+ShAttrib1f(0.5),
     			(tc3d(1)/(numslices) + (2.0/numslices)*(-(numslices-1.0)/2 + floor( floor(((tc3d(2)+(1.0/(numslices*numslices)))/(2.0)+0.5) * numslices*numslices)/numslices)) )/2+ShAttrib1f(0.5));*/

     //return texture(tc);
     //return lerp( numslices*numslices*( (tc3d(2)/2.0+0.5) % (1.0 / (numslices*numslices)) )  ,texture(tc), texture(tc2));
     return lerp(  a-a1  ,texture(tc2), texture(tc));
  } 
 ShPoint3f scatter(ShPoint3f pos, ShAttrib1f scale){
    	  
    return ShPoint3f(pos(0)/(numslices*scale) + (2.0/numslices)*(-(numslices-1.0)/2 + floor(       (pos(2)/(2.0*scale)+0.5) * numslices*numslices)%numslices),
        	     pos(1)/(numslices*scale) + (2.0/numslices)*(-(numslices-1.0)/2 + floor( floor((pos(2)/(2.0*scale)+0.5) * numslices*numslices)/numslices)),
    	ShAttrib1f(0.0));
 }

 ShTexCoord2f get2DTexCoord(ShPoint3f tc3d){
 
	 return ShTexCoord2f((tc3d(0)/numslices + (2.0/numslices)*(-(numslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * numslices*numslices)%numslices)) )/2+ShAttrib1f(0.5),
     			     (tc3d(1)/numslices + (2.0/numslices)*(-(numslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * numslices*numslices)/numslices)) )/2+ShAttrib1f(0.5));

 }

// data	 
 ShTexture2D<ShColor4f> texture;
 unsigned int numslices;
 unsigned int res2d; // size of a 2d dimension
 
  
 };//class


//-------------------------------------------------------------------
// stream data
//-------------------------------------------------------------------

// direction the particle_updateX is going
int dir = 0;

// channels holding the particle data
ShChannel<ShPoint3f> posA;
ShChannel<ShPoint3f> posB;
ShChannel<ShVector3f> initvelA;
ShChannel<ShVector3f> initvelB;
ShChannel<ShVector3f> velA;
ShChannel<ShVector3f> velB;
ShChannel<ShAttrib1f> timeA;
ShChannel<ShAttrib1f> timeB;

ShChannel<ShPoint3f> terrain;

//pbuffers
 Display *dpy;
 int screen;
 GLXDrawable win;
 GLXContext ctx;
 
 GLXPbufferSGIX pb;
 GLXContext pb_ctx;
 int pb_texid;

// states
 
int fmode = 0;

// uniforms 
ShVector3f gravity(0.0, -9.8, 0.0);
ShAttrib1f delta(0.02); 
ShAttrib1f simtime(0.0);
ShConstAttrib1f timedelta(0.005);

// programs
ShProgram particle;  
ShProgram particle_updateA;
ShProgram particle_updateB;

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

// shader uniforms
ShColor3f diffuse_color = ShColor3f(0.5, 0.7, 0.9);

// programs
ShProgram vsh;
ShProgram particle_vsh;
ShProgram particle_fsh;
ShProgram particle_volume_vsh;
ShProgram particle_volume_fsh;
ShProgram plane_vsh;
ShProgram plane_fsh;
ShProgram terrain_vsh;
ShProgram terrain_fsh;
ShProgram skybox_vsh;
ShProgram skybox_fsh;

ShProgramSet* particle_shaders;
ShProgramSet* particle_volume_shaders;
ShProgramSet* plane_shaders;
ShProgramSet* terrain_shaders;
ShProgramSet* skybox_shaders;

//hightmap and normalmap textures
ShArray2D<ShColor3fub> hm(HM_RES, HM_RES);
ShTexture2D<ShColor3fub> nm(HM_RES, HM_RES);
ShTexture2D<ShColor3fub> terrt(TT_RES, TT_RES); // terrain texture

ShNoMIPFilter<ShTextureCube<ShColor3fub> > em(EM_RES, EM_RES); //env map

ShTexture2D<ShColor4f> pbtex(PBUFFERSIZE, PBUFFERSIZE);
Texture3D pbtex3d(pbtex, RESOLUTION, NUMSLICES);


ShImage hm_image;
ShImage nm_image;
ShImage em_imageA;
ShImage em_imageB;
ShImage em_imageC;
ShImage em_imageD;
ShImage em_imageE;
ShImage em_imageF;
ShImage terr_image;


//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;


void init_pbuffers(void){
  dpy = XOpenDisplay(0);
  screen = DefaultScreen(dpy);
  win = glXGetCurrentDrawable();
  ctx = glXGetCurrentContext();
  
 int num = 0;
 int attrib[] = {
  GLX_DOUBLEBUFFER,  False,
  GLX_RED_SIZE,      8,
  GLX_GREEN_SIZE,    8,
  GLX_BLUE_SIZE,    8,
  GLX_ALPHA_SIZE,    8,
  GLX_DEPTH_SIZE,	8,
  GLX_RENDER_TYPE_SGIX, GLX_RGBA_BIT_SGIX,
  GLX_DRAWABLE_TYPE_SGIX, GLX_PBUFFER_SGIX,
  //GLX_MAX_PBUFFER_WIDTH_SGIX, PBUFFERWIDTH,
  //GLX_MAX_PBUFFER_HEIGHT_SGIX, PBUFFERHEIGHT,
 None
 };

 GLXFBConfigSGIX* conf = glXChooseFBConfigSGIX(dpy, screen, attrib, &num);

  if(conf == NULL || num < 1)
    {printf("none of the %d returned configs satisfied the conditions\n", num); exit(-1);}

    int pbufferattrib[] = {GLX_PRESERVED_CONTENTS_SGIX, True, None};

    pb = glXCreateGLXPbufferSGIX(dpy, conf[0], PBUFFERSIZE, PBUFFERSIZE, pbufferattrib);
    if(!pb){printf("pb was not allocated\n"); exit(-1);}
    else{ printf("pb allocated\n");}

    pb_ctx = glXCreateContextWithConfigSGIX(dpy, conf[0], GLX_RGBA_TYPE_SGIX, ctx, true);
    //pb_ctx = glXMakeContextCurrent(dpy, conf[0], GLX_RGBA_TYPE, ctx, true);

//    glXCopyContext(dpy, ctx, pb_ctx, GL_ALL_ATTRIB_BITS);

   if(!pb_ctx){
    printf("pb_ctx was not allocated\n");
    exit(-1);
   }else
    printf("pb_ctx context created\n");

   glGenTextures(1, (GLuint*)(&pb_texid));

   char tid[5];
   sprintf(tid, "%d",pb_texid);
   pbtex.meta("opengl:texid", tid);

   printf("tid: %s\n", tid);


	
}

void load_textures(void){

    hm_image.loadPng("hm2.png");
    nm_image.loadPng("nm2.png");
    em_imageA.loadPng("sky2/A.png");    
    em_imageB.loadPng("sky2/C.png"); 
    em_imageC.loadPng("sky2/E.png");
    em_imageD.loadPng("sky2/F.png");
    em_imageE.loadPng("sky2/D.png");
    em_imageF.loadPng("sky2/B.png");
    terr_image.loadPng(/*"densitysprite2.png"*/"bumps.png");
			
    hm.memory(hm_image.memory());
    nm.memory(nm_image.memory());

    terrt.memory(terr_image.memory());

    em.memory(em_imageA.memory(), static_cast<ShCubeDirection>(0));
    em.memory(em_imageB.memory(), static_cast<ShCubeDirection>(1));
    em.memory(em_imageC.memory(), static_cast<ShCubeDirection>(2));
    em.memory(em_imageD.memory(), static_cast<ShCubeDirection>(3));
    em.memory(em_imageE.memory(), static_cast<ShCubeDirection>(4));
    em.memory(em_imageF.memory(), static_cast<ShCubeDirection>(5));




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
  wCamOrig = (imv | ShPoint3f(0, 0, 0));

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


void display()
  {
	  
// normal rendering pass
	  
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  
  // Bind in the programs used to shade the particles
  // The updating of the stream is done using double buffering, if we just
  // ran particle_updateA then the latest data state is in stream 'B', otherwise
  // its in stream 'A' (because particle_updateB was the last to be run). Based
  // on the value of dir, render the appropriate stream
  float* particle_positions = 0;
  float* particle_velocities = 0;
 if (dir == 0)
    {
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
    }
  else
    {
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr posB_storage = shref_dynamic_cast<ShHostStorage>(posB.memory()->findStorage("host"));
    if (posB_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      posB_storage->sync();

      particle_positions = (float*)posB_storage->data();
      }
    ShHostStoragePtr velB_storage = shref_dynamic_cast<ShHostStorage>(velB.memory()->findStorage("host"));
    if (velB_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      velB_storage->sync();

      particle_velocities = (float*)velB_storage->data();
      }
    }

  float* particle_times = 0;
  if (dir == 0)
    {
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
    }
  else
    {
    // Use the ShChannel object to fetch the ShHostStorage object
    ShHostStoragePtr timeB_storage = shref_dynamic_cast<ShHostStorage>(timeB.memory()->findStorage("host"));
    if (timeB_storage)
      {
      // The ShHostStorage object may be out data with respect to
      // the lastest copy on the graphics card, call sync() first
      // to make sure that the ShHostStorage updated
      timeB_storage->sync();

      particle_times = (float*)timeB_storage->data();
      }
    }


  
  float* terrain_positions = 0;
  
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
  
 // glEnable(GL_LIGHTING);

/*  
  // set a different color and draw a small pillar from
  // which the particles 'shoot' out of.
  diffuse_color = ShColor3f(0.7, 0.5, 0.3);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-.2, 1, -.2);
  glVertex3f( .2, 1, -.2);
  glVertex3f( .2, 1,  .2);
  glVertex3f(-.2, 1,  .2);

  glNormal3f(0, 0, -1);
  glVertex3f(-.2, 0, -.2);
  glVertex3f( .2, 0, -.2);
  glVertex3f( .2, 1, -.2);
  glVertex3f(-.2, 1, -.2);

  glNormal3f(1, 0, 0);
  glVertex3f( .2, 0, -.2);
  glVertex3f( .2, 0,  .2);
  glVertex3f( .2, 1,  .2);
  glVertex3f( .2, 1, -.2);

  glNormal3f(0, 0, 1);
  glVertex3f( .2, 0,  .2);
  glVertex3f(-.2, 0,  .2);
  glVertex3f(-.2, 1,  .2);
  glVertex3f( .2, 1,  .2);

  glNormal3f(-1, 0, 0);
  glVertex3f(-.2, 0,  .2);
  glVertex3f(-.2, 0, -.2);
  glVertex3f(-.2, 1, -.2);
  glVertex3f(-.2, 1,  .2);
  glEnd();*/
  
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
  shUnbind(*particle_shaders);
  }//if
// }//if fmode

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
  // diffuse_color = ShColor3f(0.5, 0.7, 0.9);
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glBegin(GL_QUADS);
  for(int i=0; i<(NUMSLICES*NUMSLICES*SLICEFACTOR); i++){
   float pz = (i*2.0*SCALE)/(NUMSLICES*NUMSLICES*SLICEFACTOR)-SCALE;
   glTexCoord2f(0,0);
   glVertex3f(-SCALE, -SCALE, pz);
   glTexCoord2f(1,0);
   glVertex3f( SCALE, -SCALE, pz);
   glTexCoord2f(1,1);
   glVertex3f( SCALE, SCALE,  pz);
   glTexCoord2f(0,1);
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
    }
  glutSwapBuffers();

  
/*  
  
// render particle positions into texture
  glXMakeCurrent(dpy, pb, pb_ctx);
  //gluLookAt(0, 0, -10,   0, 0, 0,   0, 1, 0 );
   setup_view();

  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 // glColor3f(1.0,1.0,1.0);
 
  // Set states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);


  if (particle_positions) {
  shBind(*particle_volume_shaders);
  glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
  glEnable( GL_POINT_SPRITE_ARB );
  glPointSize(5.0f);
  glBegin(GL_POINTS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   glMultiTexCoord3fARB(GL_TEXTURE1, particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
   glMultiTexCoord1fARB(GL_TEXTURE2, particle_times[i]);
   glNormal3f(particle_velocities[3*i],particle_velocities[3*i+1],particle_velocities[3*i+2]);
   glVertex3f(particle_positions[3*i], particle_positions[3*i+1], particle_positions[3*i+2]);
  }//for
  glEnd();
  glDisable( GL_POINT_SPRITE_ARB );
  }//if
  

  glDisable(GL_BLEND);
  
  // copy to texture
  // glXMakeCurrent(dpy, pb, pb_ctx);
  //glActiveTexture(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, pb_texid);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, PBUFFERSIZE, PBUFFERSIZE, 0);
  glXMakeCurrent(dpy, win, ctx);
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
  update_streams();
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
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Stream Example");
  glewInit();
 // glxewInit();
  //init_pbuffers();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glPointSize(5);

  load_textures();

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

  setup_terrain_stream();
 
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
  // Specifiy the generic particle update program, later it will
  // be specialized for the the actual particle update.
  particle = SH_BEGIN_PROGRAM("gpu:stream") {
    ShInOutPoint3f pos;
    ShInOutVector3f vel;
    ShInOutVector3f initvel;
    ShInOutAttrib1f time;
    ShInputVector3f acc;
    ShInputAttrib1f delta;

       
    // parameters controlling the amount of momentum
    // tranfer on collision
    ShAttrib1f mu(0.8);
    ShAttrib1f eps(0.6);

    // check if below ground level
   // ShAttrib1f under = pos(1) < 0.0;
    ShAttrib1f elev = (hm( (pos(0,2)+SCALE)/(2.0*SCALE)) )(0);
    ShAttrib1f under = pos(1) < elev;

    ShAttrib1f out = (pos(0) < -SCALE) || (pos(0) > SCALE) || (pos(2) < -SCALE) || (pos(2) > SCALE);
    time -= lifeStep*ShAttrib1f(LIFE_STEP_FACTOR);
    time = cond(out, ShAttrib1f(0.0), time);
    pos = cond(time, pos, ShPoint3f(0.0,1.0,0.0));
    vel = cond(time, vel, initvel);
    time = cond(time, time, ShAttrib1f(1.0));
   // time = cond(out, ShAttrib1f(1.0), time);

    
    // clamp to ground level if necessary
   // pos = cond(under, pos*ShAttrib3f(1.0, 0.0, 1.0), pos);
    pos = cond(under,ShPoint3f(pos(0), elev, pos(2)), pos);


    // modify velocity in case of collision
    ShVector3f norm( (nm((pos(0,2)+SCALE)/(2.0*SCALE)))(0,1,2)  );
    norm = norm(0,2,1)*2-1.0;
    norm = normalize(norm);
    norm(0) = -norm(0); 
    ShVector3f veln = dot(vel,norm)*norm;
    ShVector3f velt = vel - veln;
    vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);
   // pos(1) = cond(min(under, (vel|vel) < 0.1), ShPoint1f(0.0f), pos(1));

    ShAttrib1f onsurface = abs(pos(1)-elev) < 0.1;
    ShAttrib1f minspeed = length(vel)<ShAttrib1f(MIN_SPEED);
    
    // clamp acceleration to zero if particles at or below ground plane 
    acc = cond(onsurface, ShVector3f(0.0, 0.0, 0.0), acc);

    // integrate acceleration to get velocity
    vel = cond(onsurface&&minspeed, vel, vel+acc*delta);
    vel -= DAMP_FACTOR*vel*delta;

    // clamp velocity to zero if small to force particles to settle
    //vel = cond((vel|vel) < 0.5, ShVector3f(0.0, 0.0, 0.0), vel);

    // integrate velocity to update position
    pos = cond(onsurface&&minspeed,pos,pos + vel*delta);

    
    //vel = cond(length(vel)>0.0 && length(vel)<ShAttrib1f(MIN_SPEED), ShVector3f(0.0,0.0,0.0), vel);
       
    // clamp to the edge of the plane, just to make it look nice
  //  pos(0) = min(max(pos(0), -5), 5);
  //  pos(2) = min(max(pos(2), -5), 5);
  } SH_END;

  // Allocate ShHostMemory objects that will hold the particle position
  // and velocity data. We need two set of memory object since we'll
  // be double buffering the particle system updates. The memory
  // objects will allocate the memory needed internally, only the
  // size of the memory is given (in bytes, hence the 3*sizeof(float))
  ShHostMemoryPtr host_posA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_posB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_velA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_velB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_initvelA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_initvelB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_timeA = new ShHostMemory(sizeof(float)*NUM_PARTICLES, SH_FLOAT);
  ShHostMemoryPtr host_timeB = new ShHostMemory(sizeof(float)*NUM_PARTICLES, SH_FLOAT);


  ShHostMemoryPtr host_terrain = new ShHostMemory(3*sizeof(float)*(TM_RES-1)*(TM_RES-1)*6, SH_FLOAT);
  
  // Allocate the associated ShChannel objects for each of
  // the ShHostMemory objects. The type of channel will determine
  // how the memory object is interpreted, only the number of
  // elements is needed (i.e. no needed for the 3*sizeof(float),
  // this is implicit in the use of ShPoint3f/ShVector3f)
  posA = ShChannel<ShPoint3f>(host_posA, NUM_PARTICLES);
  posB = ShChannel<ShPoint3f>(host_posB, NUM_PARTICLES);
  velA = ShChannel<ShVector3f>(host_velA, NUM_PARTICLES);
  velB = ShChannel<ShVector3f>(host_velB, NUM_PARTICLES);
  initvelA = ShChannel<ShVector3f>(host_initvelA, NUM_PARTICLES);
  initvelB = ShChannel<ShVector3f>(host_initvelB, NUM_PARTICLES);
  timeA = ShChannel<ShAttrib1f>(host_timeA, NUM_PARTICLES);
  timeB = ShChannel<ShAttrib1f>(host_timeB, NUM_PARTICLES);

  terrain = ShChannel<ShPoint3f>(host_terrain, (TM_RES-1)*(TM_RES-1)*6);

  // Tie the postion and velocity data into an ShStream, one
  // for the 'A' stream and one for the 'B' stream.
  ShStream dataA = posA & velA & initvelA & timeA;
  ShStream dataB = posB & velB & initvelB & timeB;

  // Process the generic particle program into two versions, one that
  // uses the dataA stream and one that uses the dataB stream. Additionally,
  // we feed in two uniforms (gravity & delta) since these will not change
  // on a particle by particle basis (although they can be changed globally).

  
//  particle_updateA = shSwizzle(0, 1) << (particle << dataA << gravity << delta);
//  particle_updateB = shSwizzle(0, 1) << (particle << dataB << gravity << delta);

  particle_updateA = (particle << dataA << gravity << delta);
  particle_updateB = (particle << dataB << gravity << delta);

  // Everything has been setup for the particle system to operate, last
  // thing is to set the initial data.
  reset_streams();
  }

void setup_terrain_stream(void){
 
  ShHostStoragePtr terrain_storage = shref_dynamic_cast<ShHostStorage>(terrain.memory()->findStorage("host"));

  terrain_storage->dirty();

  float* terrain_data = (float*)terrain_storage->data();

  float terrain_temp[TM_RES*TM_RES*3];
  for(int i = 0; i < TM_RES*TM_RES; i++)
    {
    // All the particles will start at the same places
    float row, col;
    col = 1.0/(TM_RES-1) * (i%TM_RES) ;
    row = 1.0/(TM_RES-1) * (i/TM_RES) ;
    terrain_temp[3*i+0] = (row-.5)*(2.0*SCALE);
    terrain_temp[3*i+1] = hm_image((HM_RES-1)*row, (HM_RES-1)*col, 0);
    terrain_temp[3*i+2] =  (col-.5)*(2.0*SCALE);

  }//for

  for(int i=0;i<(TM_RES-1)*(TM_RES-1);i++){
	for(int k=0;k<3;k++){  
  	 terrain_data[(6*i)*3+k] = terrain_temp[i*3+k];
	 terrain_data[(6*i+2)*3+k] = terrain_temp[(i+TM_RES)*3+k];
	 terrain_data[(6*i+1)*3+k] = terrain_temp[(i+1)*3+k];
	 terrain_data[(6*i+3)*3+k] = terrain_temp[(i+TM_RES)*3+k];
	 terrain_data[(6*i+5)*3+k] = terrain_temp[(i+TM_RES+1)*3+k];
	 terrain_data[(6*i+4)*3+k] = terrain_temp[(i+1)*3+k];
	}

  }

  

}

void reset_streams(void)
  {
  // Set our time counter to zero
  simtime = 0.0;
    
  // Use the ShChannels objects to find the associated ShHostStorage and
  // then fetch the raw data and cast to a float pointer.
  ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
  ShHostStoragePtr velA_storage = shref_dynamic_cast<ShHostStorage>(velA.memory()->findStorage("host"));
  ShHostStoragePtr initvelA_storage = shref_dynamic_cast<ShHostStorage>(initvelA.memory()->findStorage("host"));
  ShHostStoragePtr timeA_storage = shref_dynamic_cast<ShHostStorage>(timeA.memory()->findStorage("host"));


  // these storages maybe cached on the graphics card, flag them
  // as dirty so that:
  // 1) the latest copies will be on the host 
  // 2) after reseting the stream, the graphics card copy will be updated
  posA_storage->dirty();
  velA_storage->dirty();
  initvelA_storage->dirty();
  timeA_storage->dirty();

  float* posA_data = (float*)posA_storage->data();
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

    timeA_data[i] = i*(1.0/NUM_PARTICLES);

    lifeStep = ShAttrib1f(1.0/NUM_PARTICLES);
    
    }

  // reset the dir flag to that the simulation will start with the 'A' stream
  dir = 0;
  }

void init_shaders(void)
  {
    skybox_vsh = SH_BEGIN_VERTEX_PROGRAM {
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
    //ShOutputTexCoord2f tc;
    ShOutputPoint4f posp;
    //ShInOutNormal3f normal;
    ShOutputVector3f lightv;
   
    // Get the Y value from heightmap
    posp = pos;
    //tc = pos(0,2)/(10/*2.0*SCALE*/)+.5;
       
    // Compute viewspace position
    ShPoint3f posv = (mv | posp)(0,1,2);

    // Compute light direction
    //lightv = lightPos - posv;
    lightv = ShVector3f(0.6,-0.3,0);
    

    // Project position
    pos = mvd | posp;

    // Project normal
    //normal = mv | normal;

  } SH_END;

    terrain_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    //ShInputTexCoord2f tc;
    ShInputPosition4f posp;
    //ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShOutputColor3f color;

    ShTexCoord2f tc = posp(0,2)/(2.0*SCALE)+.5;
    //normal = normalize(normal);
    //lightv = normalize(lightv);
    //ShVector3f norm( (nm((posp(0,2)+SCALE)/(2.0*SCALE)))(0,1,2)*2-1.0  );
    ShVector3f norm( ( nm(tc) )(0,1,2)*2-1.0  );

    norm = normalize(norm);
    norm(0)=-norm(0);
    lightv = normalize(lightv);
    //lightv = ShVector3f(0,1,0);
    
    
    color = (norm|lightv) * terrt(tc) *  ShColor3f(0.9,0.7,0.5); //terrain color;
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

  /* ShAttrib1f lifetime(1.0-parttime);
    
    color = cond(lifetime < 0.5,
                 in*lerp(lifetime * 2.0,
                      ShConstColor3f(1.0, 0.0, 0.0),   // red
                      ShConstColor3f(1.0, 1.0, 0.0)),  // yellow
                 in*lerp(min((lifetime - 0.5) * 2.0, 1.0),
                      ShConstColor3f(0.2, 0.2, 0.2),   // gray
                      ShConstColor3f(1.0, 0.0, 0.0))); // red*/

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

    /*
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
    lightv = -(imtm|lightPos);*/
        
      // Project position
    //pos = mvd | pos;
  
   /* pos(0) = pos(0)/(8*5) + (2.0/8)*(-3.5 + floor((pos(2)/10+0.5) * 64)%8);
    pos(1) = pos(1)/(8*5) + (2.0/8)*(-3.5 + floor( floor((pos(2)/10+0.5) * 64)/8));
    pos(2) = 0.0;
*/
    /*ShMatrix4x4f v = mv; // just view, no translation
    v = transpose(v);
    v[3](0,1,2) = ShVector3f(0.0, 0.0, 0.0);
    v = transpose(v);*/

    pos = v | pos;
    pos(0,1,2) = pbtex3d.scatter(pos(0,1,2),SCALE);
    

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

  plane_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutTexCoord2f tc;
    ShOutputPosition3f posp;
   // ShInOutTexCoord3f partpos;
   // ShInOutTexCoord1f parttime;
   // ShInputNormal3f vel; // not real normal
   // ShOutputVector3f camray;
   // ShOutputPoint3f posm; // position in MS
   // ShOutputVector3f lightv;
   // ShOutputMatrix4x4f omtm;

   /* tc(0) = (pos(0)/(8*5) + (2.0/8)*(-3.5 +        floor( (pos(2)/10+0.5) * 64)%8) )/2+.5;
    tc(1) = (pos(1)/(8*5) + (2.0/8)*(-3.5 + floor( floor( (pos(2)/10+0.5) * 64)/8) ) )/2+.5;*/
    
    posp = pos(0,1,2);
    //tc = pbtex3d.get2DTexCoord(pos(0,1,2)/5);
    pos = vd | pos;
        
    } SH_END;

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  plane_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputTexCoord2f tc;
    ShInputPosition4f posp;
    //ShInputNormal3f normal;
    //ShInputVector3f lightv;
    ShOutputColor4f color;

       
    /*ShAttrib1f density = pbtex(tc)(3);
    ShAttrib2f fw = fwidth(tc);
    ShAttrib1f w = SH::max(fw(0),fw(1));
    color = ShColor4f(1,1,1,1) * deprecated_smoothstep(-w,w,density+ShAttrib1f(0.05));*/

    //tc = pbtex3d.get2DTexCoord(posp(0,1,2)/5);

    color = pbtex3d(posp(0,1,2)/5);//pbtex(tc);
    /*tc(0) = (posp(0)/(8*5) + (2.0/8)*(-3.5 + floor(((posp(2)/10+0.5) * 64)%8)) )/2+.5;
    tc(1) = (posp(1)/(8*5) + (2.0/8)*(-3.5 + floor(((posp(2)/10+0.5) * 64)/8)) )/2+.5;*/
    //color = pbtex(tc);
    
  } SH_END;

  particle_shaders = new ShProgramSet(particle_vsh, particle_fsh);
  particle_volume_shaders = new ShProgramSet(particle_volume_vsh, particle_volume_fsh);
  plane_shaders = new ShProgramSet(plane_vsh, plane_fsh);
  terrain_shaders = new ShProgramSet(terrain_vsh, terrain_fsh);
  skybox_shaders = new ShProgramSet(skybox_vsh, skybox_fsh);
  }

void update_streams(void)
  {
  shUnbind(*plane_shaders);
  shUnbind(*particle_shaders);

  try 
    {
    // The value of dir specifcies whether to run particle_updateA, which
    // will take the data from stream 'A' and run the particle program to
    // generate stream 'B', or the opposite using 'B' to generate 'A'. The
    // rendering code uses dir in the same way to decided which stream has
    // the latest state data for draw drawing the particle.
    if (dir == 0)
      {
      posB & velB & initvelB & timeB = particle_updateA;
      }
    else
      {
      posA & velA & initvelA & timeA = particle_updateB;
      }
    dir = !dir;
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
