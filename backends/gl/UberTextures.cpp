#include "UberTextures.hpp"
#include "GlTextureName.hpp"
#include "UberStorage.hpp"

namespace shgl {

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

struct StorageFinder {
  StorageFinder(const ShTextureNodePtr& node, bool ignoreTarget = false)
    : node(node), ignoreTarget(ignoreTarget)
  {
  }
  
  bool operator()(const ShStoragePtr& storage) const
  {
    UberStoragePtr t = shref_dynamic_cast<UberStorage>(storage);
    if (!t) return false;
    if (!ignoreTarget) {
      /*
      if (t->texName()->params() != node->traits()) return false;
      if (t->target() != shGlTargets[node->dims()]) return false;
      */ // TODO
    }
    if (t->width() != node->width()) return false;
    if (t->height() != node->height()) return false;
    if (t->pitch() != node->dims()) return false;
    //    if (t->depth() != node->depth()) return false;
    return true;
  }
  
  const ShTextureNodePtr& node;
  bool ignoreTarget;
};


UberTextures::UberTextures()
{
}

void UberTextures::bindTexture(const ShTextureNodePtr& node,
                             GLenum target)
{
  if (!node) return;


  // TODO: Check for memories that are 0

  if (node->dims() == SH_TEXTURE_CUBE) {
    // Look for a cubemap that happens to have just the right storages

    /* TODO
    GlTextureName::NameList::const_iterator I;
    for (I = GlTextureName::beginNames(); I != GlTextureName::endNames(); ++I) {
      const GlTextureName* name = *I;
      if (name->target() != GL_TEXTURE_CUBE_MAP) continue;
      if (name->params() != node->traits()) continue;
      
      GlTextureName::StorageList::const_iterator S;
      for (S = name->beginStorages(); S != name->endStorages(); ++S) {
        ShCubeDirection dir = glToShCubeDir((*S)->target());
        if ((*S)->memory() != node->memory(dir).object() || !StorageFinder(node, true)(*S))
          break;
      }
      // If we got through the whole list, we've found a matching list.
      if (S == name->endStorages()) break;
    }
    
    if (I == GlTextureName::endNames()) {
      // Need to allocate new storages
      GlTextureNamePtr texname = new GlTextureName(GL_TEXTURE_CUBE_MAP);
      texname->params(node->traits());
      for (int i = 0; i < 6; i++) {
        ShCubeDirection dir = static_cast<ShCubeDirection>(i);
        GlTextureStoragePtr storage = new GlTextureStorage(node->memory(dir).object(),
                                                       shGlCubeMapTargets[i],
                                                       shGlFormat(node),
                                                       shGlInternalFormat(node),
                                                       node->width(), node->height(),
                                                       node->depth(),
                                                       texname);
        storage->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(target));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texname->value()));
    } else {
      // Just synchronize the storages
      GlTextureName::StorageList::const_iterator S;
      for (S = (*I)->beginStorages(); S != (*I)->endStorages(); ++S) {
        (*S)->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(target));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, (*I)->value()));
    }
    */
  } else {
    // Everything but cubemaps
    StorageFinder finder(node);
    UberStoragePtr storage =
      shref_dynamic_cast<UberStorage>(node->memory()->findStorage("uberbuffer", finder));
    if (!storage) {
      GlTextureNamePtr name = new GlTextureName(shGlTargets[node->dims()]);
      storage = new UberStorage(node->memory().object(),
                                node->width(), node->height(), node->dims());
      // TODO: name management
      name->params(node->traits());
    }

    SH_GL_CHECK_ERROR(glActiveTextureARB(target));
    storage->sync();
    SH_GL_CHECK_ERROR(glBindTexture(shGlTargets[node->dims()], storage->name()));
  }
}


}
