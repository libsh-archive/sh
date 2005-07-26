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
#include <sh/sh.hpp>
#ifdef __APPLE__
# include <CoreFoundation/CoreFoundation.h>
# include <GLUT/glut.h>
# include <OpenGL/glext.h>
# include <OpenGL/glu.h>
#else
# include <GL/glut.h>
# include <GL/glext.h>
# include <GL/glu.h>
#endif
#include "Camera.hpp"
#include <iostream>

using namespace SH;
using namespace std;

ShMatrix4x4f mv, mvd;
ShPoint3f lightPos;
Camera camera;
ShProgramSet* shaders;

ShMIPFilter<ShTexture2D<ShColor3fub> > kd(512, 512);
ShNoMIPFilter<ShTexture2D<ShColor3fub> > ks(512, 512);

int gprintf(int x, int y, char* fmt, ...);

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;

const int MIPMAP_LEVELS = 10;

ShImage kd_images[MIPMAP_LEVELS + 1];
ShImage ks_images[2];

void rustTexture()
{
  kd.memory(kd_images[0].memory(), 0);
  kd.build_mipmaps();
  ks.memory(ks_images[0].memory(), 0);
}

void xTexture()
{
  for (int i=0; i < MIPMAP_LEVELS; i++) {
    kd.memory(kd_images[1 + i].memory(), i);
  }
  ks.memory(ks_images[1].memory(), 0);
}

void initShaders()
{
  ShProgram vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutTexCoord2f u;
    ShInOutPosition4f pos;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    ShOutputPoint3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    lightv = lightPos - posv; // Compute light direction
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
  } SH_END;

  ShColor3f SH_DECL(diffusecolor) = ShColor3f(0.2, 0.2, 0.2);
  ShAttrib1f exponent(30.0);

  ShProgram fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputTexCoord2f u;
    ShInputPosition4f position;
    ShInputNormal3f normal;
    ShInputVector3f lightv;
    ShInputPoint3f posv;

    ShOutputColor3f color;

    normal = normalize(normal);
    lightv = normalize(lightv);

    color = (normal | lightv) * diffusecolor;

    ShVector3f vv = normalize(-posv);
    ShVector3f hv = normalize(lightv + vv);
    ShNormal3f nv = normal;
    color += color*kd(u) + ks(u)*pow(pos(hv | nv), exponent);
  } SH_END;

  shaders = new ShProgramSet(vsh, fsh);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shBind(*shaders);
  
  glFrontFace(GL_CW);
  glutSolidTeapot(2.5);
  glFrontFace(GL_CCW);

  shUnbind(*shaders);
  
  // Help information
  if (show_help) {
    gprintf(30, 100, "Sh Texture Example Help");
    gprintf(30, 80,  "  '1' - Default texture");
    gprintf(30, 65,  "  '2' - Rusty texture");
    gprintf(30, 10,  "  'Q' - Quit");
  } else {
    gprintf(10, 10, "'H' for help...");
  }
  
  glutSwapBuffers();
}

void setupView()
{
  mv = camera.shModelView();
  mvd = camera.shModelViewProjection(ShMatrix4x4f());
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
  setupView();
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
    setupView();
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

void keyboard(unsigned char k, int x, int y)
{
  switch(k) {
  case '1':
    xTexture();
    break;
  case '2':
    rustTexture();
    break;
  case 'q':
  case 'Q':
    exit(0);
    break;
  case 'h':
  case 'H':
    show_help = !show_help;
    break;
  }
  glutPostRedisplay();
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

  // just in case, turn lighting and
  // texturing off and disable depth testing
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  // render the character through glut
  char* p = temp;
  glRasterPos2f(x, y);
  while(*p) {
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

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Texture Example");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);

  if (argc > 1) {
    shUseBackend(argv[1]);
  }

  initShaders();

  try {
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFStringRef s;
    char* filename;
    
    s = CFURLCopyFileSystemPath(CFBundleCopyResourceURL(mainBundle, CFSTR("rustkd"), CFSTR("png"), NULL),
                                kCFURLPOSIXPathStyle);
    filename = new char[CFStringGetLength(s) + 1];
    CFStringGetCString(s, filename, CFStringGetLength(s) + 1, kCFStringEncodingASCII);
    kd_images[0].loadPng(filename);
    delete [] filename;

    s = CFURLCopyFileSystemPath(CFBundleCopyResourceURL(mainBundle, CFSTR("rustks"), CFSTR("png"), NULL),
                                kCFURLPOSIXPathStyle);
    filename = new char[CFStringGetLength(s) + 1];
    CFStringGetCString(s, filename, CFStringGetLength(s) + 1, kCFStringEncodingASCII);
    ks_images[0].loadPng(filename);
    delete [] filename;

    s = CFURLCopyFileSystemPath(CFBundleCopyResourceURL(mainBundle, CFSTR("kd"), CFSTR("png"), NULL),
                                kCFURLPOSIXPathStyle);
    filename = new char[CFStringGetLength(s) + 1];
    CFStringGetCString(s, filename, CFStringGetLength(s) + 1, kCFStringEncodingASCII);
    kd_images[1].loadPng(filename);
    delete [] filename;
    
    s = CFURLCopyFileSystemPath(CFBundleCopyResourceURL(mainBundle, CFSTR("ks"), CFSTR("png"), NULL),
                                kCFURLPOSIXPathStyle);
    filename = new char[CFStringGetLength(s) + 1];
    CFStringGetCString(s, filename, CFStringGetLength(s) + 1, kCFStringEncodingASCII);
    ks_images[1].loadPng(filename);
    delete [] filename;
#else
    kd_images[0].loadPng("tex_rustkd.png");
    ks_images[0].loadPng("tex_rustks.png");
    kd_images[1].loadPng("tex_kd.png");
    ks_images[1].loadPng("tex_ks.png");

    for (int i=1; i < MIPMAP_LEVELS; i++) {
      stringstream s;
      s << "kd" << i << ".png";
      kd_images[1 + i].loadPng(s.str());
    }
#endif
  } 
  catch (const ShException& e) {
#ifdef __APPLE__
    CFStringRef s = CFStringCreateWithCString(kCFAllocatorDefault, e.message().c_str(), kCFStringEncodingASCII);
    CFOptionFlags flags;
    CFUserNotificationDisplayAlert(0, kCFUserNotificationStopAlertLevel,
                                  NULL, NULL, NULL, CFSTR("Error loading images"),
                                  s, NULL, NULL, NULL, &flags);
    CFRelease(s);
#else
    std::cerr << e.message() << std::endl;
#endif
    exit(1);
  } catch (const std::exception& e) {
#ifdef __APPLE__
    CFStringRef s = CFStringCreateWithCString(kCFAllocatorDefault, e.what(), kCFStringEncodingASCII);
    CFOptionFlags flags;
    CFUserNotificationDisplayAlert(0, kCFUserNotificationStopAlertLevel,
                                   NULL, NULL, NULL, CFSTR("Error loading images"),
                                   s, NULL, NULL, NULL, &flags);
    CFRelease(s);
#else
    std::cerr << e.what() << std::endl;
#endif
    exit(1);
  }
  
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  setupView();

  // Place the camera at its initial position
  camera.move(0.0, 0.0, -15.0);

  // Set up the light position
  lightPos = ShPoint3f(5.0, 5.0, 5.0);
  
  initShaders();

  // Set the initial texture
  xTexture();
  
  shBind(*shaders);
  
#if 0
  cout << "Vertex Unit:" << endl;
  vsh.node()->code()->print(cout);
  cout << "--" << endl;
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  cout << "--" << endl;
#endif
  
  glutMainLoop();

  delete shaders;
}
