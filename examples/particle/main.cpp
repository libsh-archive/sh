#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include <sh/sh.hpp>

#include "Camera.hpp"

using namespace SH;
using namespace std;

// defines
#define NUM_PARTICLES 4096

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

// channels holding the particle data
ShChannel<ShPoint3f> posA;
ShChannel<ShPoint3f> posB;
ShChannel<ShVector3f> velA;
ShChannel<ShVector3f> velB;

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
ShMatrix4x4f mv;
ShMatrix4x4f mvd;

// shader uniforms
ShColor3f diffuse_color = ShColor3f(0.5, 0.7, 0.9);

// programs
ShProgram vsh;
ShProgram particle_fsh;
ShProgram plane_fsh;

//-------------------------------------------------------------------
// GLUT data
//-------------------------------------------------------------------
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;

void setup_view()
  {
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
  }

void display()
  {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // Bind in the programs used to shade the particles
  shBind(vsh);
  shBind(particle_fsh);

  // The updating of the stream is done using double buffering, if we just
  // ran particle_updateA then the latest data state is in stream 'B', otherwise
  // its in stream 'A' (because particle_updateB was the last to be run). Based
  // on the value of dir, render the appropriate stream
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
      float* posA_data = (float*)posA_storage->data();
      glBegin(GL_POINTS);
      for(int i = 0; i < NUM_PARTICLES; i++)
       {
       glVertex3fv(&posA_data[3*i]);
       }
      glEnd();
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

      float* posB_data = (float*)posB_storage->data();

      // fetch the raw data pointer and simply render the particles as points
      glBegin(GL_POINTS);
      for(int i = 0; i < NUM_PARTICLES; i++)
       {
       glVertex3fv(&posB_data[3*i]);
       }
      glEnd();
      }
    }

  // Bind in the fragment program to shade the plane pillar
  shBind(plane_fsh);

  // set the color and render the plane as a simple quad
  diffuse_color = ShColor3f(0.5, 0.7, 0.9);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, -5);
  glVertex3f( 5, 0, -5);
  glVertex3f( 5, 0,  5);
  glVertex3f(-5, 0,  5);
  glEnd();

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

  try
    {
    shSetBackend("glsl");

    init_shaders();
    init_streams();
  
    // Place the camera at its initial position
    camera.move(0.0, 0.0, -15.0);
    camera.rotate(-45, 0.0, 1.0, 0.0);
    camera.rotate(30, 1.0, 0.0, 0.0);
    setup_view();

    // Set up the light position
    lightPos = ShPoint3f(5.0, 5.0, 5.0);
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

#if 1
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  particle_fsh.node()->code()->print(cout);
  cout << "--" << endl;
  plane_fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif
 
  glutMainLoop();
  return 0;
  }

void init_streams(void)
  {
  // Specifiy the generic particle update program, later it will
  // be specialized for the the actual particle update.
  particle = SH_BEGIN_PROGRAM("gpu:stream") {
    ShInOutPoint3f pos;
    ShInOutVector3f vel;
    ShInputVector3f acc;
    ShInputAttrib1f delta;

    // clamp acceleration to zero if particles at or below ground plane 
    acc = cond(abs(pos(1)) < 0.05, ShVector3f(0.0, 0.0, 0.0), acc);

    // integrate acceleration to get velocity
    vel += acc*delta;

    // clamp velocity to zero if small to force particles to settle
    //vel = cond((vel|vel) < 0.5, ShVector3f(0.0, 0.0, 0.0), vel);

    // integrate velocity to update position
    pos += vel*delta;
    
    // parameters controlling the amount of momentum
    // tranfer on collision
    ShAttrib1f mu(0.3);
    ShAttrib1f eps(0.6);

    // check if below ground level
    ShAttrib1f under = pos(1) < 0.0;

    // clamp to ground level if necessary
    pos = cond(under, pos*ShAttrib3f(1.0, 0.0, 1.0), pos);

    // modify velocity in case of collision
    ShVector3f veln = vel*ShAttrib3f(0.0, 1.0, 0.0);
    ShVector3f velt = vel - veln;
    vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);
    pos(1) = cond(min(under, (vel|vel) < 0.1), ShPoint1f(0.0f), pos(1));

    // clamp to the edge of the plane, just to make it look nice
    pos(0) = min(max(pos(0), -5), 5);
    pos(2) = min(max(pos(2), -5), 5);
  } SH_END;

  // Allocate ShHostMemory objects that will hold the particle position
  // and velocity data. We need two set of memory object since we'll
  // be double buffering the particle system updates. The memory
  // objects will allocate the memory needed internally, only the
  // size of the memory is given (in bytes, hence the 3*sizeof(float))
  ShHostMemoryPtr host_posA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_posB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_velA = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);
  ShHostMemoryPtr host_velB = new ShHostMemory(3*sizeof(float)*NUM_PARTICLES);

  // Allocate the associated ShChannel objects for each of
  // the ShHostMemory objects. The type of channel will determine
  // how the memory object is interpreted, only the number of
  // elements is needed (i.e. no needed for the 3*sizeof(float),
  // this is implicit in the use of ShPoint3f/ShVector3f)
  posA = ShChannel<ShPoint3f>(host_posA, NUM_PARTICLES);
  posB = ShChannel<ShPoint3f>(host_posB, NUM_PARTICLES);
  velA = ShChannel<ShVector3f>(host_velA, NUM_PARTICLES);
  velB = ShChannel<ShVector3f>(host_velB, NUM_PARTICLES);

  // Tie the postion and velocity data into an ShStream, one
  // for the 'A' stream and one for the 'B' stream.
  ShStream dataA = posA & velA;
  ShStream dataB = posB & velB;

  // Process the generic particle program into two versions, one that
  // uses the dataA stream and one that uses the dataB stream. Additionally,
  // we feed in two uniforms (gravity & delta) since these will not change
  // on a particle by particle basis (although they can be changed globally).
  particle_updateA = shSwizzle(0, 1) << (particle << dataA << gravity << delta);
  particle_updateB = shSwizzle(0, 1) << (particle << dataB << gravity << delta);

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
  ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
  ShHostStoragePtr velA_storage = shref_dynamic_cast<ShHostStorage>(velA.memory()->findStorage("host"));

  // these storages maybe cached on the graphics card, flag them
  // as dirty so that:
  // 1) the latest copies will be on the host 
  // 2) after reseting the stream, the graphics card copy will be updated
  posA_storage->dirty();
  velA_storage->dirty();

  float* posA_data = (float*)posA_storage->data();
  float* velA_data = (float*)velA_storage->data();

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
    float power = 5 + 3*((float)rand()/(RAND_MAX-1));

    velA_data[3*i+0] = power*sint*cosp;
    velA_data[3*i+1] = power*sinp;
    velA_data[3*i+2] = power*cost*cosp;
    }

  // reset the dir flag to that the simulation will start with the 'A' stream
  dir = 0;
  }

void init_shaders(void)
  {
  // This vertex program will be used for all our shading, it
  // simply transforms the incoming position/normal and generates
  // a light vector. 
  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    // Compute viewspace position
    ShPoint3f posv = (mv | pos)(0,1,2);

    // Compute light direction
    lightv = lightPos - posv;
    
    // Project position
    pos = mvd | pos;

    // Project normal
    normal = mv | normal;

  } SH_END;

  // This fragment shader will be used to shade our particle. Its
  // just assigns a uniform color (point_color). The above vertex
  // program will be used in conjunction with this fragment program
  // the transformed normal and generated light vector are simply
  // ignored.
  particle_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShOutputColor3f color;

    color = cond(simtime < 0.5,
                 lerp(simtime * 2.0,
                      ShConstColor3f(1.0, 0.0, 0.0),   // red
                      ShConstColor3f(1.0, 1.0, 0.0)),  // yellow
                 lerp(min((simtime - 0.5) * 2.0, 1.0),
                      ShConstColor3f(0.0, 0.0, 1.0),   // blue
                      ShConstColor3f(1.0, 0.0, 0.0))); // red
  } SH_END;

  // This fragment shader will be used to shade the other pieces
  // of geometry (the plane and particle shooter). Its just a simple
  // diffuse shader (using the global uniform diffuse_color).
  plane_fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShOutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal|lightv)*diffuse_color;
  } SH_END;
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
      posB & velB = particle_updateA;
      }
    else
      {
      posA & velA = particle_updateB;
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

  // just in case, turn lighting and
  // texturing off and disable depth testing
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  shUnbind(vsh);
  shUnbind(particle_fsh);
  shUnbind(plane_fsh);

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
