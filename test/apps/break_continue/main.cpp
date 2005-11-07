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
#include <sh.hpp>
#include <fstream>

#define GL_GLEXT_VERBOSE 1
#define GL_GLEXT_PROTOTYPES 1

#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
# include <GL/glext.h>
# include <GL/glu.h>
#endif
#include <iostream>

using namespace SH;
using namespace std;

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */


ShMatrix4x4f mv, mvd;
ShProgram vsh, fsh;
ShProgramSet* shaders = NULL;
ShPoint3f lightPos(light_position);
ShColor3f diffusecolor(light_diffuse);

void print_matrix(const GLfloat* mat)
{
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      if (j != 0) cout << " ";
      GLfloat val = mat[i + 4*j];
      if (val <= 9 && val >= 0) cout << " ";
      cout << val;
    }
    cout << endl;
  }
}

void drawBox(void)
{
  int i;

  for (i = 0; i < 6; i++) {
    glBegin(GL_QUADS);
    
    int index=0;
    for (ShProgramNode::VarList::const_iterator it = vsh.begin_inputs();
         it != vsh.end_inputs(); it++) {
      stringstream ss;
      ss << (*it)->meta("opengl:attribindex");
      ss >> index;

      string s = (*it)->name();
      if ("normal" == s) {
        glVertexAttrib3fvARB(index, &n[i][0]);
      }
    }

    glVertexAttrib3fvARB(0, &v[faces[i][0]][0]);
    glVertexAttrib3fvARB(0, &v[faces[i][1]][0]);
    glVertexAttrib3fvARB(0, &v[faces[i][2]][0]);
    glVertexAttrib3fvARB(0, &v[faces[i][3]][0]);
    glEnd();
  }
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shBind(*shaders);
  drawBox();
  shUnbind(*shaders);
  glutSwapBuffers();
}

void init(void)
{
  /* Setup cube vertex data. */
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 1.0, /* Z far */ 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */

  /* Adjust cube position to be asthetic angle. */
  glTranslatef(0.0, 0.0, -1.0);
  glRotatef(60, 1.0, 0.0, 0.0);
  glRotatef(-20, 0.0, 0.0, 1.0);
}

void initShaders()
{
  // Initial values for the uniforms
  GLfloat mat_mv[16];
  GLfloat mat_mvd[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mat_mv);
  glGetFloatv(GL_PROJECTION_MATRIX, mat_mvd);
  for (int i=0; i < 4; i++) {
    for (int j=0; j < 4; j++) {
      mv[i][j] = mat_mv[i + 4*j];
      mvd[i][j] = mat_mvd[i + 4*j];
    }
  }
  //print_matrix(mat_mv);
  //print_matrix(mat_mvd);
  mvd = mvd * mv;

  vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutNormal3f SH_DECL(normal);
    ShInOutPosition4f SH_DECL(pos); // TODO swap the order of normal and pos

    ShPoint3f posv = (mv | pos)(0,1,2); // Compute viewspace position
    
    pos = mvd | pos; // Project position
    normal = mv | normal; // Project normal
    
  } SH_END;

  fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShOutputColor3f color;

    normal = normalize(normal);
    ShVector3f lightv = normalize(lightPos);
    
    ShColor3f col1;
    ShAttrib1f it(0.0);
//     SH_WHILE(it < 5.0) {
//       it += 1.0;
//       //SH_BREAK(it > 1);
//       SH_CONTINUE(it > 3);
//       SH_CONTINUE(it > 1);
//       col1 += ShColor3f(0.0, 0.0, 0.2);
//     } SH_ENDWHILE;

//     SH_FOR(it = 0.0, it < 5.0, it += 1.0) {
//       //SH_BREAK(it > 1);
//       SH_CONTINUE(it > 3);
//       SH_CONTINUE(it > 0);
//       col1 += ShColor3f(0.0, 0.0, 0.2);
//     } SH_ENDFOR;

//     SH_DO {
//       it += 1.0;
//       //SH_BREAK(it > 2);
//       SH_CONTINUE(it > 3);
//       SH_CONTINUE(it > 1);
//       col1 += ShColor3f(0.0, 0.0, 0.2);
//     } SH_UNTIL(it >= 5.0);
    
    color = (normal | lightv) * col1;
  } SH_END;

  vsh.meta("opengl:matching", "generic");

  shaders = new ShProgramSet(vsh, fsh);
}

void keyboard(unsigned char k, int x, int y)
{
  switch(k) {
  case 'q':
  case 'Q':
    exit(0);
    break;
  }
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(512, 512);
  glutCreateWindow("Sh Example");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);

  if (argc > 1) {
    shUseBackend(argv[1]);
  }

  init();
  initShaders();
  shBind(*shaders);

#if 1
  cout << "Fragment Unit:" << endl;
  fsh.node()->code()->print(cout);
  //cout << "--" << endl;
  //fsh.node()->ctrlGraph->print(cout, 0);
  //std::ofstream f("graph.dot");
  //fsh.node()->ctrlGraph->graphvizDump(f);
  //std::system("dot -Tps graph.dot -o graph.ps");
#endif

  glutMainLoop();
  delete shaders;
}
