#ifndef SHARBMEMORY_HPP
#define SHARBMEMORY_HPP

#include <list>
#include <algorithm>
#include <GL/gl.h>
#include "ShRefCount.hpp"
#include "ShTextureNode.hpp"
#include "ShMemory.hpp"

namespace ShArb {

class TextureStorage;

class TextureName : public SH::ShRefCountable {
public:
  TextureName(GLenum target);
  ~TextureName();

  GLuint value() const { return m_name; }
  GLenum target() const { return m_target; }
  unsigned int params() const { return m_params; }
  void params(unsigned int params);

  void addStorage(TextureStorage* storage);
  void removeStorage(TextureStorage* storage);

  typedef std::list<TextureStorage*> StorageList;
  StorageList::const_iterator beginStorages() const { return m_storages.begin(); }
  StorageList::const_iterator endStorages() const { return m_storages.end(); }

  typedef std::list<TextureName*> NameList;
  static NameList::const_iterator beginNames() { return m_names.begin(); }
  static NameList::const_iterator endNames() { return m_names.end(); }
  
private:
  GLenum m_target;
  GLuint m_name;
  StorageList m_storages;
  static NameList m_names;
  unsigned int m_params;
};

typedef SH::ShRefCount<TextureName> TextureNamePtr;

class TextureStorage : public SH::ShStorage {
public:
  TextureStorage(SH::ShMemory* memory, GLenum target,
                 GLenum format, GLint internalFormat,
                 int width, int height, int depth,
                 TextureNamePtr name);

  ~TextureStorage();
  
  std::string id() const { return "arb:texture"; }
  
  GLuint name() const { return m_name->value(); }
  const TextureNamePtr& texName() const { return m_name; }
  GLenum target() const { return m_target; }
  GLenum format() const { return m_format; }
  GLint internalFormat() const { return m_internalFormat; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int depth() const { return m_depth; }

private:
  TextureNamePtr m_name;
  GLenum m_target;
  GLenum m_format;
  GLint m_internalFormat;
  int m_width, m_height, m_depth;
  
  unsigned int m_params;

  TextureStorage(const TextureStorage&);
  TextureStorage& operator=(const TextureStorage&);
  
  // TODO: Mipmapping?
};

typedef SH::ShRefCount<TextureStorage> TextureStoragePtr;

}

#endif
