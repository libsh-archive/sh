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
#ifndef PBUFFERSTREAMS_HPP
#define PBUFFERSTREAMS_HPP

#include "ShProgram.hpp"
#include "GlBackend.hpp"

namespace shgl {

enum FloatExtension {
  SH_ARB_NV_FLOAT_BUFFER,
  SH_ARB_ATI_PIXEL_FORMAT_FLOAT,
  SH_ARB_NO_FLOAT_EXT
};

struct PBufferStreams : public StreamStrategy {
  PBufferStreams(int context = 0);
  virtual ~PBufferStreams();

  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

private:
  virtual FloatExtension setupContext(int width, int height) = 0;
  virtual void restoreContext(void) = 0;

private:
  int m_context;

  int m_setup_vp;
  SH::ShProgram m_vp;
};

}

#endif
