// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "ShScheduler.hpp"
//#include <iostream>
#include <cassert>

// for the memcpy stuff
#include <stdlib.h>

#include "ShSyntax.hpp"
#include "ShEnvironment.hpp"

#include <sh.hpp>

#include <iostream>

namespace SH {


/// this texture should really be part of the calss, but I had compiling problems
  ShTexture2D<SH::ShColor4f> gtexture = ShTexture2D<SH::ShColor4f>(SIZE, SIZE);

/// used to decide what texture I write to
  int turn = 0;

  ShScheduler::ShScheduler(){

    
    //tex = ShTexture2D<ShColor4f>( SIZE, SIZE );
    tub[0] = new ShUberbuffer( SIZE, SIZE, 1, 4, 6408 /* - hack: instead of GL_RGBA */ );
    tub[1] = new ShUberbuffer( SIZE, SIZE, 1, 4, 6408 /* - hack: instead of GL_RGBA */ );
    
    /// temporary hack
    tub[2] = new ShUberbuffer( SIZE, SIZE, 1, 4, 6408 /* - hack: instead of GL_RGBA */ );

    fb = new ShFramebuffer( SIZE, SIZE, 1, 4 ); 
    
  }


  void ShScheduler::bind(ShComplexProgram pgm1,	ShComplexProgram pgm2){

    complex_shader[0] = pgm1;
    complex_shader[1] = pgm2;

    // clear the arrays
    vprograms.clear();
    fprograms.clear();
       
    /// the vertex shader always has 2 programs
    vprograms.push_back(complex_shader[0]->getProgram(0));

    /// assume that N>=1
    int N = complex_shader[1]->numPrograms();
    if(N==0)
      std::cout<<"No programs to bind!"<<std::endl;

    fprograms.push_back(complex_shader[1]->getProgram(0));

    for(int i=1;i<N;i+=1)
        fprograms.push_back(shHideInputs(complex_shader[1]->getProgram(i), (i-1)%2));
  }
	


/// haced for now
ShMatrix4x4f ortho(int x1, int y1, int x2, int y2)
{
    
    ShMatrix4x4f ret;

    float rl = 1.0 / (x2 - x1);
    float tb = 1.0 / (y2 - y1);

    float A = 2 * rl;
    float B = 2 * tb;
    float C = 1;

    float tx = -(x1 + x2) * rl;
    float ty = -(y1+ y2) * tb;
    float tz = 0;

    
    ret[0](0) = A;
    ret[0](1) = 0.0;
    ret[0](2) = 0.0;
    ret[0](3) = tx;

    ret[1](0) = 0.0;
    ret[1](1) = B;
    ret[1](2) = 0.0;
    ret[1](3) = ty;

    ret[2](0) = 0.0;
    ret[2](1) = 0.0;
    ret[2](2) = C;
    ret[2](3) = tz;


    ret[3](0) = 0.0;
    ret[3](1) = 0.0;
    ret[3](2) = 0.0;
    ret[3](3) = 1.0;

    
    return ret;
}


  void ShScheduler::run(ShVertexArray& data, ShMatrix4x4f& mv, ShMatrix4x4f& mvp){
	    
    std::cout<<"In RUN!"<<std::endl;

    // save the old mv and mvp
    ShMatrix4x4f mv_bak = mv;
    ShMatrix4x4f mvp_bak = mvp;

    ShMatrix4x4f orth = ortho(0, 0, SIZE, SIZE);
    ShMatrix4x4f ID; /* I hope that the default is the identity matrix */

    /// keep the old transformation
    mv = ID;
    mvp = orth | mv;

    /* FIRST PASSES */
    
    // I set it only ONCE here
    shDrawBuffer(fb);

    /// nasty hack
///    for(int i=1;i<fprograms.size();i+=1){
    //   gtexture.attach( tub[2]);
    //shBindShader(vprograms[0]);
    //  shBindShader(fprograms[i]);
    // std::cout<<"Dummy memory object is: "<<tub[2]->mem()<<std::endl;
    //}

    for(int i=0;i<fprograms.size()-1;i+=1){

      std::cout<<"Loop "<<i<<std::endl;

      /// bind the shaders first
      shBindShader(vprograms[0]);
      shBindShader(fprograms[i]);

      std::cout<<"break 1"<<std::endl;

      // attach the correct mem object
      if(i>0)
	 gtexture.attach( tub[(i-1)%2]);
      
      std::cout<<"break2"<<std::endl;

      
      fb->bind(tub[i%2]);

      ShEnvironment::backend->render_planar(data);

      // detach the memory objects
      fb->bind(0);
      if(i>0)
	gtexture.attach(tub[2]);

      std::cout<<"End Loop "<<i<<std::endl;

    }


    /* SECOND PASS */
    std::cout<<"Second Pass!"<<std::endl;

    /// restore the transformation matrices
    mv = mv_bak;
    mvp = mvp_bak;

    std::cout<<"Before bind!"<<std::endl;

    int N = fprograms.size()-1;
    shBindShader(vprograms[0]);
    shBindShader(fprograms[N]);
    //gtexture.attach( tub[(N-1)%2]);
    gtexture.attach( tub[2]);

    std::cout<<"After bind!"<<std::endl;

    // call the backend to render
    ShEnvironment::backend->render(data);

    /// intro hacker
    gtexture.attach( tub[2]);
    
    std::cout<<"Out RUN!"<<std::endl;

}



ShProgram  shHideInputs(ShProgram prgm, int turn){ 	   
  ShProgram hider = SH_BEGIN_PROGRAM {
    ShProgramNode::VarList::const_iterator I;
    int count = 0;

    for (I = prgm->inputs.begin(); I != prgm->inputs.end(); ++I) {
      count +=1;
    }	

    ShInputTexCoord2f t;
    ShOutputAttrib3f output;

    output = gtexture(t)(0,1,2);
  
  } SH_END_PROGRAM;


  ShProgram result = connect(hider, prgm);
  
  std::cout<<"Printing hider: "<<std::endl;
  hider->print();

  std::cout<<"Printing the original program: "<<std::endl;
  prgm->print();

  std::cout<<"Printing the final program: "<<std::endl;
  result->print();

  return result;
}



} // namespace sh


