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
