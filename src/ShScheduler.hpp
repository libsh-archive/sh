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
#ifndef SHSCHEDULER_HPP
#define SHSCHEDULER_HPP

class ShScheduler;

#include <vector>
#include <list>
#include <iosfwd>

#include "ShProgramGraph.hpp"
#include "ShArrayData.hpp"

#include "ShFramebuffer.hpp"
#include "ShUberbuffer.hpp"

#include "ShAttrib.hpp"
#include "ShMatrix.hpp"
namespace SH {

#define SIZE 512 


#define UN (10)

  class ShScheduler {
  public:
    
    ShScheduler();

    void allocate_uberbuffers(int);

    /// currnet bound multi-pass shader programs
    ShComplexProgram complex_shader[2];


	/// bind the program
	void bind(ShComplexProgram,ShComplexProgram);
	
	/// render data
	void run(ShVertexArray& data, ShMatrix4x4f& mv, ShMatrix4x4f& mvp);

	// memory objects
	unsigned int  m_mem;
	unsigned int  m_tex;// use only one texture
	
	void CreateDummyTexture();
	// dummy texture
	unsigned char* texture;

	void getError();
	int error;

  private: /* uber buffer stuff */
    //  ShTexture2D<ShColor4f>* tex;
    ShUberbufferPtr tub[UN]; 
    ShFramebufferPtr fb; 


    /// the sequence of dummy programs
    std::vector<ShProgram> vprograms, fprograms;

};



}

#endif
