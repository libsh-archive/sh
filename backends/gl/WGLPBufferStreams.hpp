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
#ifndef WGLPBUFFERSTREAMS_HPP
#define WGLPBUFFERSTREAMS_HPP

#include "ShStream.hpp"
#include "ShProgram.hpp"
#include "PBufferStreams.hpp"

namespace shgl {

  struct ShWGLPBufferInfo
    {
    ShWGLPBufferInfo() :
      extension(SH_ARB_NO_FLOAT_EXT),
      pbuffer(0),
      pbuffer_hdc(0),
      pbuffer_hglrc(0),
      width(0),
      height(0)
      {
      }

    FloatExtension extension;
    HPBUFFERARB pbuffer;
    HDC pbuffer_hdc;
    HGLRC pbuffer_hglrc;
    int width, height;

    bool valid() 
      { 
      if (extension == SH_ARB_NO_FLOAT_EXT ||
          pbuffer == NULL ||
          pbuffer_hdc == NULL ||
          pbuffer_hglrc == NULL)
        {
        return false;
        }
      else
        {
        return true;
        }
      }
    };

  struct WGLPBufferStreams: public PBufferStreams
    {
    public:
      WGLPBufferStreams(void);
      virtual ~WGLPBufferStreams();

      virtual StreamStrategy* create(void);

    private:
      virtual FloatExtension setupContext(int width, int height);
      virtual void restoreContext(void);

    private:
      ShWGLPBufferInfo createContext(int width, int height);

    private:
      HDC m_orig_hdc;
      HGLRC m_orig_hglrc;
      std::list<ShWGLPBufferInfo> m_infos;
    };

}

#endif
