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
#ifndef DXBACKEND_HPP
#define DXBACKEND_HPP

#include "ShProgram.hpp"
#include "ShStream.hpp"
#include "ShTextureNode.hpp"

#ifdef WIN32

#include <windows.h>

#include <d3d9.h>

#endif /* WIN32 */

namespace shdx {

struct TextureStrategy {
  virtual TextureStrategy* create(int context) = 0;
  
  virtual void bindTexture(const SH::ShTextureNodePtr& texture,
                           int target) = 0;
};

struct StreamStrategy {
  virtual StreamStrategy* create(int context) = 0;
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest) = 0;
};

struct CodeStrategy {
  virtual CodeStrategy* create(int context) = 0;
  virtual SH::ShBackendCodePtr generate(const std::string& target,
                                        const SH::ShProgramNodeCPtr& shader,
                                        TextureStrategy* textures) = 0;

  void setD3DDevice(LPDIRECT3DDEVICE9 pD3DDevice) { m_pD3DDevice = pD3DDevice; }

protected:

  // Direct3D device
  LPDIRECT3DDEVICE9 m_pD3DDevice;

};

class DxBackend : public SH::ShBackend {
public:
  virtual SH::ShBackendCodePtr generateCode(const std::string& target,
                                            const SH::ShProgramNodeCPtr& shader);

  // execute a stream program, if supported
  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

  virtual int newContext();
  virtual int context() const;
  virtual void setContext(int context);
  virtual void destroyContext();

  // Pass data to the backend (only used for DirectX)
  virtual void setBackendData(void *ptr);

protected:
  DxBackend(CodeStrategy* code, TextureStrategy* texture,
            StreamStrategy* stream);
  
  // Direct3D device
  LPDIRECT3DDEVICE9 m_pD3DDevice;
  
private:
  int m_curContext;

  struct Context {
    Context(CodeStrategy* code,
            TextureStrategy* textures,
            StreamStrategy* streams)
      : code(code), textures(textures), streams(streams)
    {
    }
    
    CodeStrategy* code;
    TextureStrategy* textures;
    StreamStrategy* streams;
  };
  
  std::vector<Context> m_contexts;

  // NOT IMPLEMENTED
  DxBackend(const DxBackend& other);
  DxBackend& operator=(const DxBackend& other);
};

}

#endif
