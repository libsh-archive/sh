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
#ifndef GLXPBUFFERSTREAMS_HPP
#define GLXPBUFFERSTREAMS_HPP

#include "ShStream.hpp"
#include "ShProgram.hpp"
#include "PBufferStreams.hpp"

namespace shgl {

  struct ShGLXPBufferInfo
    {
    ShGLXPBufferInfo() :
      extension(SH_ARB_NO_FLOAT_EXT),
      pbuffer(0),
      context(0),
      width(0),
      height(0),
      shcontext(-1)
      {
      }

    FloatExtension extension;
    GLXPbuffer pbuffer;
    GLXContext context;
    int width, height;
    int shcontext;

    bool valid() 
      { 
      if (extension == SH_ARB_NO_FLOAT_EXT ||
          pbuffer == NULL ||
          context == NULL)
        {
        return false;
        }
      else
        {
        return true;
        }
      }
    };

  struct GLXPBufferStreams: public PBufferStreams
    {
    public:
      GLXPBufferStreams(int context = 0);
      virtual ~GLXPBufferStreams();

      virtual StreamStrategy* create(int context);

    private:
      virtual FloatExtension setupContext(int width, int height);
      virtual void restoreContext(void);

    private:
      GLXDrawable orig_drawable;
      GLXContext orig_context;
      ShGLXPBufferInfo m_info;
    };

}

#endif
