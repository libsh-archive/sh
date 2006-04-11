// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#include <cstdio>

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
#define NUM_PARTICLES 1600000

// forward declarations
void init_shaders(void);
void init_streams(void);
void reset_streams(void);
void update_streams(void);
int gprintf(int x, int y, char* fmt, ...);

//-------------------------------------------------------------------
// stream data
//-------------------------------------------------------------------

// direction the particle_updateX is going
int dir = 0;
int nump = 0;

// channels holding the particle data
Channel<Point3f> pos_stream;
Channel<Vector3f> vel_stream;

HostMemoryPtr pos_memory[2];
HostMemoryPtr vel_memory[2];

// uniforms 
Vector3f gravity(0.0, -9.8, 0.0);
Attrib1f delta(0.015);
Attrib1f simtime(0.0);
ConstAttrib1f timedelta(0.0005);

// programs
Program particle;

//-------------------------------------------------------------------
// shader data
//-------------------------------------------------------------------

// viewing uniforms
Camera camera;
Point3f lightPos;
Matrix4x4f mv;
Matrix4x4f mvd;
Point3f disp;

// shader uniforms
Color3f diffuse_color = Color3f(0.5, 0.7, 0.9);
unsigned int particle_counts[8];
Attrib1f particle_index;

Attrib1f matrix_mode = 0.0;
Attrib1f prematrix_speed;

// programs
ProgramSet* particle_shaders;
ProgramSet* plane_shaders;

const int num_spheres = 1;
struct Sphere {
  Point3f center;
  Attrib1f radius;
};
Sphere spheres[num_spheres];

//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;

void setup_view()
{
  mv = camera.modelView();
  mvd = camera.modelViewProjection(Matrix4x4f());
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

  pos_memory[dir]->hostStorage()->sync();
  particle_positions = reinterpret_cast<float*>(pos_memory[dir]->hostStorage()->data());
  float* particle_positions_old = reinterpret_cast<float*>(pos_memory[1 - dir]->hostStorage()->data());

  float* particle_velocities = 0;

  vel_memory[dir]->hostStorage()->sync();
  particle_velocities = reinterpret_cast<float*>(vel_memory[dir]->hostStorage()->data());

  // Bind in the programs to shade the plane pillar
  bind(*plane_shaders);

  // set the color and render the plane as a simple quad
  diffuse_color = Color3f(0.5, 0.7, 0.9);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5000, 0, -5000);
  glVertex3f( 5000, 0, -5000);
  glVertex3f( 5000, 0,  5000);
  glVertex3f(-5000, 0,  5000);
  glEnd();

  diffuse_color = Color3f(0.7, 0.5, 0.3);

  disp = spheres[0].center;
  glutSolidSphere(spheres[0].radius.getValue(0), 50, 50);
  disp = Point3f(0,0,0);

  unbind(*plane_shaders);

  

  if (particle_positions) {
    // TODO: Get these externally instead
    for (int division = 0; division < 8; division++) {
      particle_counts[division] = nump/8;
      if (division == 0) particle_counts[division] += nump % 8;
    }
    bind(*particle_shaders);
    unsigned int offset = 0;
    for (int division = 0; division < 8; division++) {
      particle_index = division;
      //      glBegin(GL_POINTS);
      glBegin(GL_LINES);
      for (unsigned int i = 0; i < particle_counts[division]; i++) {
	glNormal3fv(&particle_velocities[3 * (i + offset)]);
        glVertex3fv(&particle_positions[3 * (i + offset)]);
        glVertex3fv(&particle_positions_old[3 * (i + offset)]);
      }
      glEnd();
      offset += particle_counts[division];
    }
  }
  
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
    case ' ':
      {
	static bool idling = false;
	if (idling) glutIdleFunc(NULL);
	else glutIdleFunc(idle);
	idling = !idling;
	break;
      }
      
    case 'l':
      delta *= 0.8;
      break;
      
    case 'o':
      delta /= 0.8;
      break;
    
    case 'r':
    case 'R':
      {
	reset_streams();
	break;
      }
    case 'i':
    case 'I':
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
    case 'd':
      spheres[0].center[0] += 0.05;
      break;
    case 'a':
      spheres[0].center[0] -= 0.05;
      break;
    case 'q':
      spheres[0].center[2] += 0.05;
      break;
    case 'e':
      spheres[0].center[2] -= 0.05;
      break;
    case 'w':
      spheres[0].center[1] += 0.05;
      break;
    case 's':
      spheres[0].center[1] -= 0.05;
      break;
    case 'm':
      matrix_mode = 1.0 - matrix_mode.getValue(0);
      if (matrix_mode.getValue(0)) {
	prematrix_speed = delta;
	delta = 0.002 * 0.1;
      } else {
	delta = prematrix_speed;
      }
      break;
    case '+':
      spheres[0].radius += 0.05;
      break;
    case '-':
      spheres[0].radius -= 0.05;
      break;
    }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
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
  glLineWidth(3);

  try
    {
      if (argc > 1) {
	useBackend(argv[1]);
      }
      
      init_shaders();
      init_streams();
  
      // Place the camera at its initial position
      camera.move(0.0, 0.0, -15.0);
      camera.rotate(-45, 0.0, 1.0, 0.0);
      camera.rotate(30, 1.0, 0.0, 0.0);
      setup_view();

      // Set up the light position
      lightPos = Point3f(5.0, 5.0, 5.0);
      disp = Point3f(0, 0, 0);
    }
  catch (const Exception& e)
    {
      std::cerr << "SH Exception: " << e.message() << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << "Unknown exception caught." << std::endl;
      return 1;
    }
 
  glutMainLoop();
  return 0;
}

void init_streams(void)
{
  // Specifiy the generic particle update program, later it will
  // be specialized for the the actual particle update.
  particle = SH_BEGIN_PROGRAM("stream") {
    InOutPoint3f  DECL(pos);
    InOutVector3f DECL(vel);
    InputVector3f DECL(acc);
    InputAttrib1f DECL(delta);

    // clamp acceleration to zero if particles at or below ground plane 
    acc = cond(abs(pos(1)) < 0.05, Vector3f(0.0, 0.0, 0.0), acc);

    // integrate acceleration to get velocity
    vel += acc*delta;

    // clamp velocity to zero if small to force particles to settle
    //vel = cond((vel|vel) < 0.5, Vector3f(0.0, 0.0, 0.0), vel);

    // integrate velocity to update position
    pos += vel*delta;
    
    // parameters controlling the amount of momentum
    // tranfer on collision
    Attrib1f mu(0.3);
    Attrib1f eps(0.6);

    for (int i = 0; i < num_spheres; i++) { // Sphere collisions
      Point3f C = spheres[i].center;
      Attrib1f r = spheres[i].radius;
      Vector3f PosC = pos - C;
      Vector3f N = normalize(PosC);
      Point3f S = C + N * r;
      Attrib1f collide = ((PosC|PosC) < r * r) * ((vel|N) < 0);
      pos = cond(collide, pos - 2.0 * ((pos - S)|N) * N, pos);
      Vector3f Vn = (vel|N) * N;
      Vector3f Vt = vel - Vn;
      vel = cond(collide, (1.0 - mu) * Vt - eps * Vn, vel);
    }

    // check if below ground level
    Attrib1f under = pos(1) < 0.0;

    // clamp to ground level if necessary
    pos = cond(under, pos*Attrib3f(1.0, 0.0, 1.0), pos);

    // modify velocity in case of collision
    Vector3f veln = vel*Attrib3f(0.0, 1.0, 0.0);
    Vector3f velt = vel - veln;
    vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);
    pos(1) = cond(min(under, (vel|vel) < 0.1), Point1f(0.0f), pos(1));
  } SH_END;

  // Allocate HostMemory objects that will hold the particle position
  // and velocity data. We need two set of memory object since we'll
  // be double buffering the particle system updates. The memory
  // objects will allocate the memory needed internally, only the
  // size of the memory is given (in bytes, hence the 3*sizeof(float))
  for (int i = 0; i < 2; i++) {
    pos_memory[i] = new HostMemory(3*sizeof(float) * NUM_PARTICLES, FLOAT);
    vel_memory[i] = new HostMemory(3*sizeof(float)*NUM_PARTICLES, FLOAT);
  }

  // Allocate the associated Channel objects for each of
  // the HostMemory objects. The type of channel will determine
  // how the memory object is interpreted, only the number of
  // elements is needed (i.e. no needed for the 3*sizeof(float),
  // this is implicit in the use of Point3f/Vector3f)
  pos_stream = Channel<Point3f>(pos_memory[0], NUM_PARTICLES);
  vel_stream = Channel<Vector3f>(vel_memory[0], NUM_PARTICLES);

  // Process the generic particle program into two versions, one that
  // uses the dataA stream and one that uses the dataB stream. Additionally,
  // we feed in two uniforms (gravity & delta) since these will not change
  // on a particle by particle basis (although they can be changed globally).
  particle = swizzle(0, 1) << (particle << pos_stream << vel_stream << gravity << delta);

  // Everything has been setup for the particle system to operate, last
  // thing is to set the initial data.
  reset_streams();
}

void reset_streams(void)
{
  // Set our time counter to zero
  simtime = 0.0;
  
  spheres[0].center = Point3f(0, 3, 0);
  spheres[0].radius = Attrib1f(1);
    
  // these storages maybe cached on the graphics card, flag them
  // as dirty so that:
  // 1) the latest copies will be on the host 
  // 2) after reseting the stream, the graphics card copy will be updated
  pos_memory[0]->hostStorage()->dirty();
  vel_memory[0]->hostStorage()->dirty();

  float* pos_data = reinterpret_cast<float*>(pos_memory[0]->hostStorage()->data());
  float* vel_data = reinterpret_cast<float*>(vel_memory[0]->hostStorage()->data());

  // Iterate over the particle and initialize their state
  for(int i = 0; i < NUM_PARTICLES; i++) {
    // All the particles will start at the same places
    pos_data[3*i+0] = 1.0*((float)rand()/(RAND_MAX-1))-0.5;
    pos_data[3*i+1] = 5;
    pos_data[3*i+2] = 0;

    // The velocity direction will vary over a small cone of angles.

    /*
      float theta = 2*M_PI*((float)rand()/(RAND_MAX-1));
      float phi = 0.7*(M_PI/2) + 0.2*(M_PI/2)*((float)rand()/(RAND_MAX-1));

      float cost = cos(theta);
      float sint = sin(theta);
      float cosp = cos(phi);
      float sinp = sin(phi);
    */
    // Additionally, vary the speed (i.e. magnitude of the velocity vector)
    // float power = 5 + 3*((float)rand()/(RAND_MAX-1));

    vel_data[3*i+0] = 0.04*((float)rand()/(RAND_MAX-1)) - 0.02;
    vel_data[3*i+1] = 0;
    vel_data[3*i+2] = 1.9 + 0.2*((float)rand()/(RAND_MAX-1));

    for (int m = 0; m < 3; m++) {
      pos_data[3*i+m] += /* vel_data[3*i+m] * */ ((float)rand()/(RAND_MAX-1)) * 0.1;
    }
  }

  // reset the dir flag to that the simulation will start with the 'A' stream
  dir = 0;
  
  pos_stream.count(8);
  vel_stream.count(8);
  nump = 8;
}

void init_shaders(void)
{
  // This vertex program will be used for all our shading, it
  // simply transforms the incoming position/normal and generates
  // a light vector. 
  Program plane_vsh = SH_BEGIN_PROGRAM("gpu:vertex") {
    InOutPosition4f pos;
    InOutNormal3f normal;
    OutputVector3f lightv;

    pos(0,1,2) += disp;
    // Compute viewspace position
    Point3f posv = (mv | pos)(0,1,2);

    // Compute light direction
    lightv = lightPos - posv;
    
    // Project position
    pos = mvd | pos;

    // Project normal
    normal = mv | normal;
  } SH_END;

  // This vertex program will be used for all our shading, it
  // simply transforms the incoming position/normal and generates
  // a light vector. 
  Program particle_vsh = SH_BEGIN_PROGRAM("gpu:vertex") {
    InOutPosition4f pos;
    InOutNormal3f normal;

    // Compute viewspace position
    Point3f posv = (mv | pos)(0,1,2);

    // Project position
    pos = mvd | pos;
  } SH_END;

  // This fragment shader will be used to shade our particle. Its
  // just assigns a uniform color (point_color). The above vertex
  // program will be used in conjunction with this fragment program
  // the transformed normal and generated light vector are simply
  // ignored.
  Program particle_fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
    InputNormal3f normal;
    OutputColor4f color;

    Attrib1f speed = length(normal);

    normal = normalize(normal);
    Vector3f light = normalize(Vector3f(0.0, -1.0, -1.0));
    Attrib1f interpolator = dot(normal, light);

    Color3f warm(0.8, 0.3, 0.2), cold(0.3, 0.1, 0.0);

    color(0,1,2) = lerp(interpolator * 0.5 + 0.5, warm, cold);
    
    color(0,1,2) += speed * Color3f(0.1, 0.1, 0.1);

    // color(0,1,2) = abs(join(sin(particle_index), cos(particle_index), 1.0/(particle_index + 1.0)));
    color(3) = 1.0;

    Color3f matrix_color(0.0, 0.0, 0.0);
    matrix_color(1) = length(color(0,1,2));
    color(0,1,2) = cond(matrix_mode, matrix_color, color(0,1,2));
  } SH_END;

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  Program plane_fsh = SH_BEGIN_PROGRAM("gpu:fragment") {
    InputPosition4f pos;
    InputNormal3f normal;
    InputVector3f lightv;
    OutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal|lightv)*diffuse_color;

    Color3f matrix_color(0.0, 0.0, 0.0);
    matrix_color(1) = length(color(0,1,2));
    color(0,1,2) = cond(matrix_mode, matrix_color, color(0,1,2));
  } SH_END;

  particle_shaders = new ProgramSet(particle_vsh, particle_fsh);
  plane_shaders = new ProgramSet(plane_vsh, plane_fsh);
}

void update_streams(void)
{
  unbind(*plane_shaders);
  unbind(*particle_shaders);


  try {
    // The value of dir specifcies whether to run particle_updateA, which
    // will take the data from stream 'A' and run the particle program to
    // generate stream 'B', or the opposite using 'B' to generate 'A'. The
    // rendering code uses dir in the same way to decided which stream has
    // the latest state data for draw drawing the particle.
    if (dir == 0) {
      nump += 64;
      std::cerr << "Setting size to " << nump << std::endl;
      pos_stream.count(nump);
      vel_stream.count(nump);
    }

    Channel<Point3f> pos_dest(pos_memory[1 - dir], pos_stream.count());
    Channel<Vector3f> vel_dest(vel_memory[1 - dir], vel_stream.count());

    pos_stream.memory(pos_memory[dir], pos_stream.count());
    vel_stream.memory(vel_memory[dir], vel_stream.count());

    pos_dest & vel_dest = particle;
    dir = !dir;
    simtime += timedelta;
  } catch (const Exception& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
  } catch (...) {
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
