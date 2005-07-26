// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#endif

#include <sh/sh.hpp>

#include "Camera.hpp"

using namespace SH;
using namespace std;

// defines
#define NUM_PARTICLES 16384
#define HM_RES 256 // height map texture resolution
#define TM_RES 128 // terrain mesh resolution

//#define LIFE_TIME 2.0
#define LIFE_STEP_FACTOR 25.0
#define MIN_SPEED 0.2
#define DAMP_FACTOR 0.4

#define PCO .02 // Particle Cube Offset

ShAttrib1f lifeStep;

// forward declarations
void init_shaders(void);
void init_streams(void);
void reset_streams(void);
void update_streams(void);
void setup_terrain_stream(void);
void load_textures(void);
int gprintf(int x, int y, char* fmt, ...);

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
ShPoint3f wCamOrig;

// shader uniforms
ShColor3f diffuse_color = ShColor3f(0.5, 0.7, 0.9);

// programs
ShProgram vsh;
ShProgram particle_vsh;
ShProgram particle_fsh;
ShProgram plane_fsh;
ShProgram terrain_vsh;
ShProgram terrain_fsh;

ShProgramSet* particle_shaders;
ShProgramSet* plane_shaders;
ShProgramSet* terrain_shaders;

//hightmap and normalmap textures
ShTexture2D<ShColor3f> hm(HM_RES, HM_RES);
ShTexture2D<ShColor3f> nm(HM_RES, HM_RES);

ShImage hm_image;
ShImage nm_image;

//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;


void load_textures(void){

    hm_image.loadPng("hm2.png");
    nm_image.loadPng("nm2.png");

    hm.memory(hm_image.memory());
    nm.memory(nm_image.memory());
}

void setup_view()
  {
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());

  wCamOrig = (inverse(mv) | ShPoint3f(.0,.0,.0));
  
  }

void display()
  {
	  
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  // Bind in the programs used to shade the particles
  // The updating of the stream is done using double buffering, if we just
  // ran particle_updateA then the latest data state is in stream 'B', otherwise
  // its in stream 'A' (because particle_updateB was the last to be run). Based
  // on the value of dir, render the appropriate stream
  float* particle_positions = 0;
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

  if (terrain_positions) {
    shBind(*terrain_shaders);
    //shBind(*particle_shaders);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < (TM_RES-1)*(TM_RES-1)*6; i++)
      {
        glVertex3fv(&terrain_positions[3*i]);
      }
    glEnd();
  }

      
  if (particle_positions) {
  shBind(*particle_shaders);
  glBegin(GL_QUADS);
  for(int i = 0; i < NUM_PARTICLES; i++){
   // for(int k=0;k<3;k++) mm[3](k) = particle_positions[3*i+k];
 
  glNormal3f(0, 1, 0);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  glNormal3f(0, 0, -1);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);

  glNormal3f(1, 0, 0);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);

  glNormal3f(0, 0, 1);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  glNormal3f(-1, 0, 0);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);

  glNormal3f(0, -1, 0);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
  glVertex3f(-PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], PCO+particle_positions[3*i+2]);
  glVertex3f(PCO+particle_positions[3*i], -PCO+particle_positions[3*i+1], -PCO+particle_positions[3*i+2]);
 
/*  glNormal3f(0, 0, -1);
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
  glVertex3f(-.2, 1,  .2);*/


   }//for
  glEnd();
/*    glBegin(GL_POINTS);
    float temp[3];
    for(int i = 0; i < NUM_PARTICLES; i++)
      {
	temp[0] = particle_times[i];
	glNormal3fv(temp);
        glVertex3fv(&particle_positions[3*i]);
      }
    glEnd();*/
  }//if
  
  // Bind in the programs to shade the plane pillar
  shBind(*plane_shaders);

  // set the color and render the plane as a simple quad
 /* diffuse_color = ShColor3f(0.5, 0.7, 0.9);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, -5);
  glVertex3f( 5, 0, -5);
  glVertex3f( 5, 0,  5);
  glVertex3f(-5, 0,  5);
  glEnd();*/

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
  glEnd();

  shUnbind(*plane_shaders);
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
  }

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
    }
  glutPostRedisplay();
  }

int main(int argc, char** argv)
  {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Stream Example");

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
    if (argc > 1) {
      shUseBackend(argv[1]);
    }
      
    init_shaders();
    init_streams();
  
    // Place the camera at its initial position
    camera.move(0.0, 0.0, -15.0);
    camera.rotate(-45, 0.0, 1.0, 0.0);
    camera.rotate(30, 1.0, 0.0, 0.0);
    setup_view();

    // Set up the light position
    lightPos = ShPoint3f(5.0, 2.0, 5.0);
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
  return 0;
  }

void init_streams(void)
  {
  // Specifiy the generic particle update program, later it will
  // be specialized for the the actual particle update.
  particle = SH_BEGIN_PROGRAM("stream") {
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
    ShAttrib1f elev = (hm( (pos(0,2)+5.0)/10) )(0);
    ShAttrib1f under = pos(1) < elev;

    ShAttrib1f out = (pos(0) < -5.0) || (pos(0) > 5.0) || (pos(2) < -5.0) || (pos(2) > 5.0);
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
    ShVector3f norm( (nm((pos(0,2)+5.0)/10))(0,1,2)  );
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
  ShHostMemoryPtr host_initvelA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_initvelB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_timeA = new ShHostMemory(sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_timeB = new ShHostMemory(sizeof(float)*NUM_PARTICLES);


  ShHostMemoryPtr host_terrain = new ShHostMemory(3*sizeof(float)*(TM_RES-1)*(TM_RES-1)*6);
  
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
    terrain_temp[3*i+0] = (row-.5)*10;
    terrain_temp[3*i+1] = hm_image((HM_RES-1)*row, (HM_RES-1)*col, 0);
    terrain_temp[3*i+2] =  (col-.5)*10;

  }//for

  for(int i=0;i<(TM_RES-1)*(TM_RES-1);i++){
	for(int k=0;k<3;k++){  
  	 terrain_data[(6*i)*3+k] = terrain_temp[i*3+k];
	 terrain_data[(6*i+1)*3+k] = terrain_temp[(i+TM_RES)*3+k];
	 terrain_data[(6*i+2)*3+k] = terrain_temp[(i+1)*3+k];
	 terrain_data[(6*i+3)*3+k] = terrain_temp[(i+TM_RES)*3+k];
	 terrain_data[(6*i+4)*3+k] = terrain_temp[(i+TM_RES+1)*3+k];
	 terrain_data[(6*i+5)*3+k] = terrain_temp[(i+1)*3+k];
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
 
    terrain_vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShOutputPoint4f posp;
    //ShInOutNormal3f normal;
    ShOutputVector3f lightv;
   
    // Get the Y value from heightmap
    /*ShPoint4f*/ posp = pos;
    //posp(1) = (hm((pos(0,2)+5.0)/10))(0);
    
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
    ShInputPosition4f posp;
//ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShOutputColor3f color;

    //normal = normalize(normal);
    //lightv = normalize(lightv);
    ShVector3f norm( (nm((posp(0,2)+5.0)/10))(0,1,2)*2-1.0  );

    norm = normalize(norm);
    norm(0)=-norm(0);
    lightv = normalize(lightv);
    
    
    color = (norm|lightv)*ShColor3f(0.0,1.0,0.0); //diffuse_color;
    //color = hm((posp(0,2)+5.0)/10);
  } SH_END;

     
  // This vertex program will be used for all our shading, it
  // simply transforms the incoming position/normal and generates
  // a light vector. 
  vsh = SH_BEGIN_PROGRAM("gpu:vertex") {
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
    ShInOutNormal3f normal;
    ShOutputPoint3f posv;
    ShOutputVector3f lightv;

    // Compute viewspace position
    posv = (mv | pos)(0,1,2);

    // Compute light direction
    lightv = lightPos - posv;
    
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
  particle_fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
   ShInputNormal3f normal;
   ShInputPoint3f posv;
   ShInputVector3f lightv;
   ShOutputColor3f color;

    	    

    ShAttrib1f lifetime(1.0-normal(0));
    
    color = cond(lifetime < 0.5,
                 lerp(lifetime * 2.0,
                      ShConstColor3f(1.0, 0.0, 0.0),   // red
                      ShConstColor3f(1.0, 1.0, 0.0)),  // yellow
                 lerp(min((lifetime - 0.5) * 2.0, 1.0),
                      ShConstColor3f(0.0, 1.0, 0.0),   // green
                      ShConstColor3f(1.0, 0.0, 0.0))); // red
//      color = ShConstColor3f(1.0, 1.0, 1.0);
      
//	color = ShConstColor3f(1.0, 1.0, 1.0);    
  } SH_END;

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  plane_fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShOutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal|lightv)*diffuse_color;
  } SH_END;

  particle_shaders = new ShProgramSet(particle_vsh, particle_fsh);
  plane_shaders = new ShProgramSet(vsh, plane_fsh);
  terrain_shaders = new ShProgramSet(terrain_vsh, terrain_fsh);
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
  GLint vp[4];
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
