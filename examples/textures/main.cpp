#include <sh/sh.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include "Camera.hpp"

using namespace SH;

ShMatrix4x4f mv, mvd;
ShPoint3f lightPos;
Camera camera;
ShProgram vsh, fsh;

ShTexture2D<ShColor3f> kd(512, 512);
ShTexture2D<ShColor3f> ks(512, 512);

int gprintf(int x, int y, char* fmt, ...);

// Glut data
int buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};
int cur_x, cur_y;

bool show_help = false;

ShImage kd_images[2];
ShImage ks_images[2];

void rustTexture()
{
  kd.memory(kd_images[0].memory());
  ks.memory(ks_images[0].memory());
}

void xTexture()
{
  kd.memory(kd_images[1].memory());
  ks.memory(ks_images[1].memory());
}

void initShaders()
{
  vsh = SH_BEGIN_VERTEX_PROGRAM {
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

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
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
}

void display()
{
  shUpdate();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFrontFace(GL_CW);
  glutSolidTeapot(2.5);
  glFrontFace(GL_CCW);

  // Help information
  if (show_help) {
    gprintf(30, 100, "Sh Texture Example Help");
    gprintf(30, 80,  "  '1' - Default texture");
    gprintf(30, 65,  "  '2' - Rusty texture");
    gprintf(30, 50,  "  'U' - Invoke shUpdate()");
    gprintf(30, 30,  "  'Q' - Quit");
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
  case 'u':
  case 'U':
    shUpdate();
    break;
  case 'h':
  case 'H':
    show_help = !show_help;
    break;
  }
  glutPostRedisplay();
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
    
  shSetBackend("arb");

  initShaders();

  kd_images[0].loadPng("rustkd.png");
  ks_images[0].loadPng("rustks.png");
  kd_images[1].loadPng("kd.png");
  ks_images[1].loadPng("ks.png");
  
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

  shBind(vsh);
  shBind(fsh);
  
  glutMainLoop();
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
  shUnbind(fsh);

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
