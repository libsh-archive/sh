#include "GlTextureName.hpp"

namespace shgl {

using namespace SH;

GlTextureName::GlTextureName(GLenum target)
  : m_target(target)
{
  glGenTextures(1, &m_name);
  m_names.push_back(this);
}

GlTextureName::~GlTextureName()
{
  m_names.erase(std::remove(m_names.begin(), m_names.end(), this));
  glDeleteTextures(1, &m_name);
}

void GlTextureName::addStorage(SH::ShStorage* storage)
{
  m_storages.push_back(storage);
}

void GlTextureName::removeStorage(SH::ShStorage* storage)
{
  m_storages.erase(std::remove(m_storages.begin(), m_storages.end(), storage));
}

GlTextureName::NameList GlTextureName::m_names = GlTextureName::NameList();

}
