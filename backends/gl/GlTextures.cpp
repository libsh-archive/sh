#include "GlTextures.hpp"
#include "GlTextureName.hpp"
#include "GlTextureStorage.hpp"

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
  return SH_CUBE_POS_X;
}

GLenum shGlInternalFormat(const ShTextureNodePtr& node)
{
  if (node->size() < 0 || node->size() > 4) return 0;
  if (node->traits().clamping() == SH::ShTextureTraits::SH_CLAMPED) {
    return node->size();
  } else if (node->traits().clamping() == SH::ShTextureTraits::SH_UNCLAMPED) {
    GLenum fpformats[4] = {GL_FLOAT_R_NV, GL_FLOAT_RG_NV, GL_FLOAT_RGB_NV, GL_FLOAT_RGBA_NV};
    return fpformats[node->size() - 1];
  }
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
    break;
  }
  // TODO: Warn or something
  return 0;
}

struct StorageFinder {
  StorageFinder(const ShTextureNodePtr& node, int context,
                bool ignoreTarget = false)
    : node(node), context(context), ignoreTarget(ignoreTarget)
  {
  }
  
  bool operator()(const ShStoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) return false;
    if (t->context() != context) return false;
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
  int context;
  bool ignoreTarget;
};

GlTextures::GlTextures(int context)
  : m_context(context)
{
}

TextureStrategy* GlTextures::create(int context)
{
  return new GlTextures(context);
}


void GlTextures::bindTexture(const ShTextureNodePtr& node,
                             GLenum target)
{
  if (!node) return;

  // TODO: Check for memories that are 0
  
  if (node->dims() == SH_TEXTURE_CUBE) {
    // Look for a cubemap that happens to have just the right storages
    
    GlTextureName::NameList::const_iterator I;
    for (I = GlTextureName::beginNames(); I != GlTextureName::endNames(); ++I) {
      const GlTextureName* name = *I;
      if (name->target() != GL_TEXTURE_CUBE_MAP) continue;
      if (name->params() != node->traits()) continue;
      
      GlTextureName::StorageList::const_iterator S;
      for (S = name->beginStorages(); S != name->endStorages(); ++S) {
        GlTextureStorage* s = dynamic_cast<GlTextureStorage*>(*S);
        if (!s) continue;
        ShCubeDirection dir = glToShCubeDir(s->target());
        if (s->memory() != node->memory(dir).object() || !StorageFinder(node, m_context, true)(s))
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
        GlTextureStoragePtr storage = new GlTextureStorage(m_context,
                                                           node->memory(dir).object(),
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
        GlTextureStorage* s = dynamic_cast<GlTextureStorage*>(*S);
        if (!s) continue;
        s->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(target));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, (*I)->value()));
    }
  } else {
    StorageFinder finder(node, m_context);
    GlTextureStoragePtr storage =
      shref_dynamic_cast<GlTextureStorage>(node->memory()->findStorage("opengl:texture", finder));
    if (!storage) {
      GlTextureNamePtr name = new GlTextureName(shGlTargets[node->dims()]);
      storage = new GlTextureStorage(m_context,
                                     node->memory().object(),
                                     shGlTargets[node->dims()],
                                     shGlFormat(node),
                                     shGlInternalFormat(node),
                                     node->width(), node->height(), node->depth(),
                                     name);
      name->params(node->traits());
    }

    SH_GL_CHECK_ERROR(glActiveTextureARB(target));
    storage->sync();
    SH_GL_CHECK_ERROR(glBindTexture(shGlTargets[node->dims()], storage->name()));
  }
}


}
