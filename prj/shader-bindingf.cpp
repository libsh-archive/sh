
#include <GL/gl.h>
#include <GL/glext.h>

#include <sh/sh.hpp>

using namespace SH;

int main(int argc, char** argv)
{

  shSetBackend("rds");

  ShMatrix4x4f mv;
  ShMatrix4x4f mvp;
  ShPoint3f lightp;
  ShColor3f diffuse;
  ShColor3f ambient;

  mv.meta("opengl:state", "state.matrix.modelview");
  mv.meta("opengl:readonly", "true");
  mvp.meta("opengl:state", "state.matrix.mvp");
  mvp.meta("opengl:readonly", "true");
  lightp.meta("opengl:state", "state.light[0].position");
  lightp.meta("opengl:readonly", "true");
  diffuse.meta("opengl:state", "state.material.diffuse");
  diffuse.meta("opengl:readonly", "true");
  ambient.meta("opengl:state", "state.material.ambient");
  ambient.meta("opengl:readonly", "true");

  ShProgram fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f ipos;
    ShInputNormal3f inrm;
    ShInputVector3f ilightv;

    ShOutputColor3f oclr;

    inrm = normalize(inrm);
    ilightv = normalize(ilightv);
    
    // standard diffuse shading
    oclr = pos(inrm|ilightv)*diffuse + ambient;
  } SH_END;

  shBind(fsh);
  
}

