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

// defines
#define NUM_PARTICLES 200

// forward declarations
void init_streams(void);
void init_shaders(void);
void update_streams(void);

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
ShAttrib1f delta(0.01);

// programs
ShProgram particle;
ShProgram particle_updateA;
ShProgram particle_updateB;

// shader data

// viewing uniform
Camera camera;
ShPoint3f lightPos;
ShMatrix4x4f mv;
ShMatrix4x4f mvd;

// shader uniforms
ShColor3f point_color = ShColor3f(1.0, 0.0, 0.0);
ShColor3f diffuse_color = ShColor3f(0.5, 0.7, 0.9);

// program
ShProgram vsh;
ShProgram particle_fsh;
ShProgram plane_fsh;

// GLUT data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

void setup_view()
  {
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
  }

void display()
  {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  shBind(vsh);
  shBind(particle_fsh);

  ShHostStoragePtr posA_storage = shref_dynamic_cast<ShHostStorage>(posA.memory()->findStorage("host"));
  if (posA_storage)
    {
    float* posA_data = (float*)posA_storage->data();

    point_color = ShColor3f(1, 0, 0);

    glBegin(GL_POINTS);
    for(int i = 0; i < NUM_PARTICLES; i++)
     {
     glVertex3fv(&posA_data[3*i]);
     }
    glEnd();
    }

  ShHostStoragePtr posB_storage = shref_dynamic_cast<ShHostStorage>(posB.memory()->findStorage("host"));
  if (posB_storage)
    {
    float* posB_data = (float*)posB_storage->data();

    point_color = ShColor3f(0, 1, 0);

    glBegin(GL_POINTS);
    for(int i = 0; i < NUM_PARTICLES; i++)
     {
     glVertex3fv(&posB_data[3*i]);
     }
    glEnd();
    }

  shBind(plane_fsh);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, -5);
  glVertex3f( 5, 0, -5);
  glVertex3f( 5, 0,  5);
  glVertex3f(-5, 0,  5);
  glEnd();
  
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
      exit(0);
      break;
    case 'i':
      {
      static bool idling = false;
      if (idling) glutIdleFunc(NULL);
      else glutIdleFunc(idle);
      idling = !idling;
      break;
      }
    case ' ':
      {
      update_streams();
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
  glEnable(GL_VERTEX_PROGRAM_ARB);
  glEnable(GL_FRAGMENT_PROGRAM_ARB);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glPointSize(5);

  try
    {
    shSetBackend("arb");

    init_shaders();
    init_streams();
  
    // Place the camera at its initial position
    setup_view();
    camera.move(0.0, 0.0, -15.0);

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
    //vel = cond((vel|vel) < 0.01, ShVector3f(0.0, 0.0, 0.0), vel);

    // integrate velocity to update position
    pos += vel*delta;
    
    // Sphere collisions
    ShAttrib1f mu(0.1), eps(0.3);

    // Handle collision with ground

    // check if below ground level
    ShAttrib1f under = pos(1) < 0.0;

    // clamp to ground level if necessary
    pos = cond(under, pos*ShAttrib3f(1.0, 0.0, 1.0), pos);

    // modify velocity in case of collision
    ShVector3f veln = vel*ShAttrib3f(0.0, 1.0, 0.0);
    ShVector3f velt = vel - veln;
    vel = cond(under, (1.0 - mu)*velt - eps*veln, vel);
    pos(1) = cond(min(under, (vel|vel) < 0.1), ShPoint1f(0.0f), pos(1));

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

  // Everything has been setup for the particle system to operate, but there
  // is no initial data. Only the 'A' data needs to be initialized, the
  // first run of particle_updateA will fill the 'B' data.

  // Use the ShHostMemory objects to fetch the raw data and cast to
  // a float pointer. Then iterate over the particle and initialize
  // their state
  float* posA_data = (float*)host_posA->hostStorage()->data();
  float* velA_data = (float*)host_velA->hostStorage()->data();
  float* posB_data = (float*)host_posB->hostStorage()->data();
  float* velB_data = (float*)host_velB->hostStorage()->data();

  for(int i = 0; i < NUM_PARTICLES; i++)
    {
    // All the particles will start at the same places
    posA_data[3*i+0] = 0;
    posA_data[3*i+1] = 1;
    posA_data[3*i+2] = 0;

    // The velocities will vary over a small cone of angles.
    float theta = 2*M_PI*((float)rand()/(RAND_MAX-1));
    float phi = 0.6*M_PI + 0.2*M_PI*((float)rand()/(RAND_MAX-1));

    float cost = cos(theta);
    float sint = sin(theta);
    float cosp = cos(phi);
    float sinp = sin(phi);

    velA_data[3*i+0] = 2*sint*cosp;
    velA_data[3*i+1] = 2*sinp;
    velA_data[3*i+2] = 2*cost*cosp;
    }
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
    ShInputPosition4f pos;
    ShOutputColor3f color;

    color = point_color;
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
    if (dir == 0)
      {
      posB & velB = particle_updateA;
      }
      else
      {
      posA & velA = particle_updateB;
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

