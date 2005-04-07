// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SH_GLTEXTURENAME_HPP
#define SH_GLTEXTURENAME_HPP

#include "GlBackend.hpp"
#include "ShMemory.hpp"


namespace shgl {

class GlTextureName : public SH::ShRefCountable {
public:
  GlTextureName(GLenum target);
  // Create an unmanaged texture name, i.e. one that is not allocated
  // and destroyed by this object
  GlTextureName(GLenum target, GLuint name);
  
  ~GlTextureName();

  GLuint value() const { return m_name; }
  GLenum target() const { return m_target; }
  const SH::ShTextureTraits& params() const { return m_params; }
  void params(const SH::ShTextureTraits& params);

  void addStorage(SH::ShStorage* storage);
  void removeStorage(SH::ShStorage* storage);

  typedef std::list<SH::ShStorage*> StorageList;
  StorageList::const_iterator beginStorages() const { return m_storages.begin(); }
  StorageList::const_iterator endStorages() const { return m_storages.end(); }

  typedef std::list<GlTextureName*> NameList;
  static NameList::const_iterator beginNames() { return m_names->begin(); }
  static NameList::const_iterator endNames() { return m_names->end(); }

  // Utility class to bind texture temporarily
  struct Binding {
    Binding(const SH::ShPointer<const GlTextureName>& name);
    ~Binding();
  
    GLenum target;
    GLint last;
  };
  
private:
  GLenum m_target;
  GLuint m_name;
  StorageList m_storages;
  static NameList* m_names;
  SH::ShTextureTraits m_params;

  bool m_managed; // True if we generated our own name
};

typedef SH::ShPointer<GlTextureName> GlTextureNamePtr;

}

#endif
