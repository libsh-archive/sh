#include <GL/gl.h>
#include <GL/glext.h>
#include "ShArb.hpp"
#include "ShArbMemory.hpp"
#include "ShDebug.hpp"

namespace ShArb {

using namespace SH;

const unsigned int shGlTargets[] = {
  GL_TEXTURE_1D,
  GL_TEXTURE_2D,
  GL_TEXTURE_RECTANGLE_NV,
  GL_TEXTURE_3D,
  GL_TEXTURE_CUBE_MAP,
};

const unsigned int shGlCubeMapTargets[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

ShCubeDirection glToShCubeDir(GLuint target)
{
  switch (target) {
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    return SH_CUBE_POS_X;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    return SH_CUBE_NEG_X;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    return SH_CUBE_POS_Y;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    return SH_CUBE_NEG_Y;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    return SH_CUBE_POS_Z;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    return SH_CUBE_NEG_Z;
  }
}

GLenum shGlInternalFormat(const ShTextureNodePtr& node)
{
  // TODO: floating point etc.
  return node->size();
}

GLenum shGlFormat(const ShTextureNodePtr& node)
{
  switch (node->size()) {
  case 1:
    return GL_LUMINANCE;
  case 2:
    return GL_LUMINANCE_ALPHA;
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    // TODO: Warn or something
    return 0;
  }
}

void ArbCode::bindTextures()
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    bindTexture(*I);
  }
}

struct StorageFinder {
  StorageFinder(const ShTextureNodePtr& node, bool ignoreTarget = false)
    : node(node), ignoreTarget(ignoreTarget)
  {
  }
  
  bool operator()(const ShStoragePtr& storage) const
  {
    TextureStoragePtr t = shref_dynamic_cast<TextureStorage>(storage);
    if (!t) return false;
    if (!ignoreTarget) {
      if (t->texName()->params() != node->traits()) return false;
      if (t->target() != shGlTargets[node->dims()]) return false;
    }
    if (t->width() != node->width()) return false;
    if (t->height() != node->height()) return false;
    if (t->depth() != node->depth()) return false;
    return true;
  }
  
  const ShTextureNodePtr& node;
  bool ignoreTarget;
};


void ArbCode::bindTexture(const ShTextureNodePtr& node)
{
  if (!node) return;

  RegMap::const_iterator texRegIterator = m_registers.find(node);
  SH_DEBUG_ASSERT(texRegIterator != m_registers.end());
  const ArbReg& texReg = texRegIterator->second;

  // TODO: Check for memories that are 0
  
  if (node->dims() == SH_TEXTURE_CUBE) {
    // Look for a cubemap that happens to have just the right storages
    
    TextureName::NameList::const_iterator I;
    for (I = TextureName::beginNames(); I != TextureName::endNames(); ++I) {
      const TextureName* name = *I;
      if (name->target() != GL_TEXTURE_CUBE_MAP) continue;
      if (name->params() != node->traits()) continue;
      
      TextureName::StorageList::const_iterator S;
      for (S = name->beginStorages(); S != name->endStorages(); ++S) {
        ShCubeDirection dir = glToShCubeDir((*S)->target());
        if ((*S)->memory() != node->memory(dir).object() || !StorageFinder(node, true)(*S))
          break;
      }
      // If we got through the whole list, we've found a matching list.
      if (S == name->endStorages()) break;
    }
    
    if (I == TextureName::endNames()) {
      // Need to allocate new storages
      TextureNamePtr texname = new TextureName(GL_TEXTURE_CUBE_MAP);
      texname->params(node->traits());
      for (int i = 0; i < 6; i++) {
        ShCubeDirection dir = static_cast<ShCubeDirection>(i);
        TextureStoragePtr storage = new TextureStorage(node->memory(dir).object(),
                                                       shGlCubeMapTargets[i],
                                                       shGlFormat(node),
                                                       shGlInternalFormat(node),
                                                       node->width(), node->height(),
                                                       node->depth(),
                                                       texname);
        storage->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(GL_TEXTURE0 + texReg.index));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texname->value()));
    } else {
      // Just synchronize the storages
      TextureName::StorageList::const_iterator S;
      for (S = (*I)->beginStorages(); S != (*I)->endStorages(); ++S) {
        (*S)->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(GL_TEXTURE0 + texReg.index));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, (*I)->value()));
    }
  } else {
    StorageFinder finder(node);
    TextureStoragePtr storage =
      shref_dynamic_cast<TextureStorage>(node->memory()->findStorage("arb:texture", finder));
    if (!storage) {
      TextureNamePtr name = new TextureName(shGlTargets[node->dims()]);
      storage = new TextureStorage(node->memory().object(),
                                   shGlTargets[node->dims()],
                                   shGlFormat(node),
                                   shGlInternalFormat(node),
                                   node->width(), node->height(), node->depth(),
                                   name);
      name->params(node->traits());
    }

    SH_GL_CHECK_ERROR(glActiveTextureARB(GL_TEXTURE0 + texReg.index));
    storage->sync();
    SH_GL_CHECK_ERROR(glBindTexture(shGlTargets[node->dims()], storage->name()));
  }
}

void ArbCode::allocTextures()
{
  for (ShProgramNode::TexList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int index;
    index = m_numTextures;
    m_registers[node] = ArbReg(SH_ARB_REG_TEXTURE, index);
    m_numTextures++;
  }
}

}
